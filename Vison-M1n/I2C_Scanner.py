#　slaveのI2C Address 読み出しテスト

from machine import I2C

i2c = I2C(I2C.I2C0, freq=100000, scl=6, sda=8)
devices = i2c.scan()
print(devices)