import sensor
import time
from machine import I2C

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# Note: You may pass up to 16 thresholds above. However, it's not really
#       possible to segment any scene with 16 thresholds before color
#       thresholds start to overlap heavily.
ORAMGE_COLOR_THRESHOLD = [(14, 100, 25, 127, 0, 127)]
YELLOW_COLOR_THRESHOLD = [(20, 100, -10, 0, 16, 65)]
BLUE_COLOR_THRESHOLD = [(0, 30, -20, 127, -128, -10)]


class Coordinate:
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h


orange_ball = Coordinate(0, 0, 0, 0)


def main():
    global orange_ball
    setup_sensor(True)
    clock = time.clock()

    while True:
        clock.tick()
        image = sensor.snapshot()

        orange_ball = track_color(
            image=image,
            color_threshold=ORAMGE_COLOR_THRESHOLD,
            pixels_threshold=10,
            area_threshold=10,
            draw_color=(255, 0, 0)
        )


def setup_sensor(dual_buff):
    """Setup sensor
    dual_buff: Double buffering is allowed, which will increase the frame rate,
                but the memory usage will also increase (about 384KiB)
    """
    sensor.reset(freq=24000000, set_regs=True, dual_buff=dual_buff)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)  # 320x240
    sensor.skip_frames(time=56)
    sensor.set_auto_gain(False, gain_db=15)
    sensor.set_auto_whitebal(False, rgb_gain_db=(63.00, 60.21, 63.62))
    sensor.run(1)


def track_color(image,
                color_threshold,
                pixels_threshold,
                area_threshold,
                draw_color=None):
    """Tracks specified color and returns max blob data
    Args:
      image (Image): the image object you want to color track.
      color_threshold (List<Tupple>): [(l_lo, l_hi, a_lo, a_hi, b_lo, b_hi)]
      pixels_threshold (int):
      area_threshold (int):
      draw_color (Tupple): (r, g, b)
    Returns:
        Coordinate
    """
    blobs = image.find_blobs(
        color_threshold,
        pixels_threshold=pixels_threshold,
        area_threshold=area_threshold
    )
    if (blobs is None) or (len(blobs) == 0):
        return Coordinate(-1, -1, -1, -1)
    if draw_color is not None:
        for b in blobs:
            image.draw_rectangle(b.rect(), color=draw_color)
            image.draw_cross(b.cx(), b.cy())
    max_blob = max(blobs, key=lambda x: x.area())
    return Coordinate(
        x=max_blob.cx(),
        y=max_blob.cy(),
        w=max_blob.w(),
        h=max_blob.h(),
    )


def on_receive(received_data):
    pass


def on_transmit():
    global orange_ball
    res = 0
    if orange_ball.x != -1:
        res = 1

    return res


def on_event(event):
    pass


i2c = I2C(
    I2C.I2C0,
    mode=I2C.MODE_SLAVE,
    scl=8,
    sda=15,
    addr=0x23,
    addr_size=7,
    on_receive=on_receive,
    on_transmit=on_transmit,
    on_event=on_event
)


if __name__ == "__main__":
    main()
