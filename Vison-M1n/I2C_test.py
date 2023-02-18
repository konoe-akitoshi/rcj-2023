from machine import I2C
import struct
import utime

# I2Cオブジェクトを作成し、スレーブモードに設定する
i2c = I2C(I2C.I2C0, mode=I2C.SLAVE, addr=0x24, freq=100000, scl=28, sda=29)

# 2バイトの数値を作成する
data = struct.pack('H', 0x1234)


# I2Cスレーブデバイスからのデータを受信するためのコールバック関数を定義する
def i2c_receive(data):
    print("Received data:", data)


# コールバック関数をI2Cオブジェクトに登録する
i2c.on("receive", i2c_receive)

# データを送信する
i2c.writeto(0x24, data)

# 送信が完了するまで待つ
utime.sleep_ms(100)