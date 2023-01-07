#ifndef COMPONENTS_MOTOR_CONTOROLER2_HPP
#define COMPONENTS_MOTOR_CONTOROLER2_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/wire.hpp"
#else
#include <Wire.h>
#endif

#include "setup_handler.hpp"

namespace component
{
class MotorController
{
  public:
    /**
     * @param address the address of the I2C slave for transmission processing
     */
    explicit MotorController(SetupHandler& handler, const int sda, const int scl, const int address);

    /**
     * Contorol 4 motors.
     *
     * Move to the `azimuth` direction and turn to the `rotation` direction with the `power`.
     *
     * @param azimuth radian(counter-clockwise) (rightward is 0, frontward is PI/2, backward is 3*PI/2)
     * @param power [-100, 100]
     * @param rotation [-100, 100] (positive is clockwise, negative is counter-clockwise)
     */
    void Drive(float azimuth, int power, int rotation) const;

    /**
     * Stop all motors.
     */
    void StopAll() const;

    /**
     * Free all motors.
     */
    void FreeAll() const;

  private:
    const int ADDRESS_;
    uint8_t FormatPower_(const int power) const;
    void Transmit_(const uint8_t* motorPower, const int size) const;
};

inline MotorController::MotorController(SetupHandler& handler, const int sda, const int scl, const int address) : ADDRESS_(address) {
    handler.SetCallback([=] {
        Wire1.setSDA(sda);
        Wire1.setSCL(scl);
        Wire1.begin();
    });
}

inline uint8_t MotorController::FormatPower_(const int power) const {
    const int p = min(abs(power), 100);

    // bit 8: モーターの回転方向 (0: 正転, 1: 逆転)
    // bit 1~7: モーターの出力(%)
    // 0b10000000: ショートブレーキ
    // 0b00000000: フリーストップ
    uint8_t ret;

    if (power == 0) {
        ret = 0b10000000;
    } else if (power > 0) {
        ret = p;
    } else {
        ret = (p | 0b10000000);
    }
    return ret;
}

inline void MotorController::Transmit_(const uint8_t* motorPower, const int size) const {
    Wire1.beginTransmission(ADDRESS_);
    Wire1.write(motorPower, size);
    Wire1.endTransmission();
}

inline void MotorController::Drive(float azimuth, int power, int rotation) const {
    // それぞれのモーターは次のように配置されている
    // 各モーターのパワーの比率を x[0] ~ x[3] に計算する
    //  motor1 right-top motor. forward-direction: ↖︎
    //  motor2 right-bottom motor. forward-direction: ↗︎
    //  motor3 left-bottom motor. forward-direction: ↘︎
    //  motor4 left-top motor. forward-direction: ↙︎
    // --
    //   ④ ①   ↙︎ ↖︎
    //   ③ ②   ↘︎ ↗︎
    // --

    // clamp(power|rotation, -100, 100)
    power = power > 100 ? 100 : power < -100 ? -100 : power;
    rotation = rotation > 100 ? 100 : rotation < -100 ? -100 : rotation;

    float x[4];
    x[0] = sin(azimuth - (PI / 4));
    x[1] = sin(azimuth + (PI / 4));
    x[2] = -sin(azimuth - (PI / 4));
    x[3] = -sin(azimuth + (PI / 4));

    float x_max = 0;
    for (int i = 0; i < 4; ++i) {
        if (x_max < abs(x[i])) {
            x_max = abs(x[i]);
        }
    }
    for (int i = 0; i < 4; ++i) {
        x[i] = (x[i] / x_max) - (rotation / 100.0);
    }

    x_max = 0;
    for (int i = 0; i < 4; ++i) {
        if (x_max < abs(x[i])) {
            x_max = abs(x[i]);
        }
    }
    for (int i = 0; i < 4; ++i) {
        x[i] /= x_max;
    }

    // NOTE: 受け取り側が6で受け取るようになってるので、配列のサイズは6にすること
    uint8_t motorPower[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        motorPower[i] = FormatPower_(x[i] * power);
    }
    Transmit_(motorPower, 6);
}

inline void MotorController::StopAll() const {
    // NOTE: 受け取り側が6で受け取るようになってるので、配列のサイズは6にすること
    uint8_t motorPower[6];
    for (int i = 0; i < 6; ++i) {
        motorPower[i] = 0b10000000;
    }
    Transmit_(motorPower, 6);
}

inline void MotorController::FreeAll() const {
    // NOTE: 受け取り側が6で受け取るようになってるので、配列のサイズは6にすること
    uint8_t motorPower[6] = {0, 0, 0, 0, 0, 0};
    Transmit_(motorPower, 6);
}
}  // namespace component

/* Appendix
MotorController::Drive の 配列x の数値確認用スクリプト
```typescript
const sin = Math.sin;
const cos = Math.cos;
const fround = (x: number, e: number) => {
  const p = 10 ** e;
  return Math.round(x * p) / p
}
const PI = 3.14159265358979323846

const calc = (s: number) => {
  let x = [];
  x[0] = sin(s - (PI / 4))
  x[1] = sin(s + (PI / 4))
  x[2] = -sin(s - (PI / 4))
  x[3] = -sin(s + (PI / 4))

  let max_x = Math.max(...x.map(x => Math.abs(x)))
  for (let i = 0; i < 4; i++) {
    x[i] /= max_x;
    x[i] = fround(x[i], 2);
  }

  let arrow = [];
  arrow[0] = x[0] == 0 ? '*' : x[0] > 0 ? '↖︎' : '↘︎';
  arrow[1] = x[1] == 0 ? '*' : x[1] > 0 ? '↗︎' : '↙︎';
  arrow[2] = x[2] == 0 ? '*' : x[2] > 0 ? '↘︎' : '↖︎';
  arrow[3] = x[3] == 0 ? '*' : x[3] > 0 ? '↙︎' : '↗︎';

  console.log(` ${fround(s, 3)}
  ${x[3].toString().padStart(5, ' ')} ${arrow[3]} ${arrow[0]} ${x[0]}
  ${x[2].toString().padStart(5, ' ')} ${arrow[2]} ${arrow[1]} ${x[1]}
  `)
}

for(let s = 0; s < PI; s += PI/8) {
  calc(s);
}
```
 */
#endif
