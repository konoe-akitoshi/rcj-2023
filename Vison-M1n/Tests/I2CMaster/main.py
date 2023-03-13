from machine import I2C
import utime

save_data = None


def on_receive(data):
    global save_data
    save_data = data
    print("on_receive:", data)


def on_transmit():
    global save_data
    print("on_transmit, send:", save_data)
    return save_data


def on_event(event):
    print("on_event:", event)


i2c = I2C(
    I2C.I2C0,
    mode=I2C.MODE_SLAVE,
    scl=8,
    sda=15,
    addr=0x24,
    addr_size=7,
    on_receive=on_receive,
    on_transmit=on_transmit,
    on_event=on_event,
)

while True:
    utime.sleep_ms(100)
