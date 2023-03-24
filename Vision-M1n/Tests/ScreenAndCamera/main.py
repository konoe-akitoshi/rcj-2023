import sensor
import lcd
import time


def setup_lcd():
    """Setup LCD
    """
    lcd.init(type=1, freq=15000000)


def setup_sensor(dual_buff):
    """Setup sensor
    dual_buff: Double buffering is allowed, which will increase the frame rate,
                but the memory usage will also increase (about 384KiB)
    """
    sensor.reset(freq=24000000, set_regs=True, dual_buff=dual_buff)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames(time=2000)
    sensor.run(1)


if __name__ == "__main__":
    setup_lcd()
    setup_sensor(True)
    clock = time.clock()
    while True:
        clock.tick()
        lcd.display(sensor.snapshot())
        print(clock.fps())
