#I2C通信テスト

from machine import I2C
import time

count = 0


def on_receive(data):
    print("on_receive:", data)


def on_transmit():
    count = count + 1
    print("on_transmit, send:", count)
    return "12345"[:(count) % 5 + 1]


def on_event(event):
    print("on_event:", event)


i2c = I2C(I2C.I2C0,
          mode=I2C.MODE_SLAVE,
          scl=20,
          sda=21,
          addr=0x24,
          addr_size=7,
          on_receive=on_receive,
          on_transmit=on_transmit,
          on_event=on_event)

while True:
    time.sleep(1)
