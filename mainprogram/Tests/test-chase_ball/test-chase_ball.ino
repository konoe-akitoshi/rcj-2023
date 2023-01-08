#include "components/setup_handler.hpp"
#include "components/open_cv.hpp"
#include "components/motor_controller.hpp"
#include "components/gyro.hpp"
#include "utils/printer.hpp"
#include "pin.hpp"

component::SetupHandler Handler;
const component::MotorController MotorController(Handler, raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);
const component::Gyro Gyro(Handler, 115200);
component::OpenCV OpenCV(Handler, 19200);

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    OpenCV.WaitData();
    const auto exist_ball = OpenCV.GetBallExistence();
    const auto ball_pos = OpenCV.GetBallPosition();
    const auto rotate = Gyro.GetRotation();
    util::PrintBool("exist_ball", exist_ball);
    util::PrintVector("ball_pos", ball_pos);
    util::PrintInt("rotate", rotate);

    if (exist_ball == false) {
        return;
    }
    const float az = Vector2::Angle(ball_pos);
    MotorController.Drive(az, 30, -rotate);
}

