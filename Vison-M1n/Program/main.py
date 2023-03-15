import sensor
import time
import array
from machine import I2C
from ucollections import OrderedDict, deque

DEBUG_MODE = False

if DEBUG_MODE:
    import lcd


CAMERA_ID = 0

I2C_SCL = 8
I2C_SDA = 15
I2C_ADDRESS = [0x21, 0x22, 0x23, 0x24][CAMERA_ID]

CAMERA_MATRIX = [
    # (fx, fy, cx, cy)
    (161.68672766, 158.61336206, 157.79629692, 110.74143006)
][CAMERA_ID]
CAMERA_MATRIX2 = [
    # (fx, fy, cx, cy)
    ()
][CAMERA_ID]
CAMERA_DISTORTION_COEFFICIENTS = [
    # (k1, k2, p1, p2, k3)
    (-0.22385482, 0.02753241, 0.00142692, -0.00068454, 0.00303857)
][CAMERA_ID]

# (L Min, L Max, A Min, A Max, B Min, B Max)
ORAMGE_COLOR_THRESHOLD = [(14, 100, 25, 127, 0, 127)]
YELLOW_COLOR_THRESHOLD = [(20, 100, -10, 0, 16, 65)]
BLUE_COLOR_THRESHOLD = [(0, 30, -20, 127, -128, -10)]

is_adjusting_white_balance = False
current_white_balance = (0, 0, 0)


class Coordinate:
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h


i2c = None
i2c_received_id = -1
i2c_continued_id_count = 0
orange_ball_pos = Coordinate(-1, -1, -1, -1)
yellow_goal_pos = Coordinate(-1, -1, -1, -1)
blue_goal_pos = Coordinate(-1, -1, -1, -1)
snapshot_image = None


def main():
    global orange_ball_pos, yellow_goal_pos, blue_goal_pos, snapshot_image

    setup_i2c(scl=I2C_SCL, sda=I2C_SDA, address=I2C_ADDRESS)
    setup_sensor(dual_buff=True)
    calibration = Calibration(320, 240)

    if DEBUG_MODE:
        clock = time.clock()
        lcd.init(type=1, freq=15000000)

    while True:
        if is_adjusting_white_balance:
            auto_white_blance()
        if DEBUG_MODE:
            clock.tick()
        image = sensor.snapshot()

        orange = track_color(
            image=image,
            color_threshold=ORAMGE_COLOR_THRESHOLD,
            pixels_threshold=10,
            area_threshold=10,
            # draw_color=(255, 0, 0)
        )

        yellow = track_color(
            image=image,
            color_threshold=YELLOW_COLOR_THRESHOLD,
            pixels_threshold=250,
            area_threshold=250,
            # draw_color=(0, 255, 0)
        )

        blue = track_color(
            image=image,
            color_threshold=BLUE_COLOR_THRESHOLD,
            pixels_threshold=250,
            area_threshold=250,
            # draw_color=(0, 0, 255)
        )

        snapshot_image = image
        orange_ball_pos = calibration.calc(orange)
        yellow_goal_pos = calibration.calc(yellow)
        blue_goal_pos = calibration.calc(blue)

        if DEBUG_MODE:
            print("fps: {}".format(clock.fps()))
            print_coordinate(orange_ball_pos, name="Orange ball")
            print_coordinate(yellow_goal_pos, name="Yellow goal")
            print_coordinate(blue_goal_pos,   name="Blue goal  ")
            lcd.display(image)


class Calibration:
    def _pos(self, x, y):
        return y * self.width + x

    def __init__(self, width, height):
        global CAMERA_MATRIX, CAMERA_MATRIX2, CAMERA_DISTORTION_COEFFICIENTS
        self.width = width
        self.height = height
        self._next_index_x = array.array("b")
        self._next_index_y = array.array("b")
        fx, fy, cx, cy = CAMERA_MATRIX
        fx2, fy2, cx2, cy2 = CAMERA_MATRIX2
        k1, k2, p1, p2, k3 = CAMERA_DISTORTION_COEFFICIENTS
        INF = 127
        for _ in range(height*width):
            self._next_index_x.append(INF)
            self._next_index_y.append(INF)

        for v in range(height):
            for u in range(width):
                x = (u - cx2) / fx2
                y = (v - cy2) / fy2
                rr = x ** 2 + y ** 2
                K = 1 + k1*rr + k2*(rr**2) + k3*(rr**3)

                x2 = x*K + 2*p1*x*y + p2*(rr+2*(x**2))
                y2 = y*K + p1*(rr+2*(y**2)) + 2*p2*x*y
                org_x = round(x2*fx + cx)
                org_y = round(y2*fy + cy)

                if 0 <= org_x < width and 0 <= org_y < height:
                    self._next_index_x[self._pos(org_x, org_y)] = u - org_x
                    self._next_index_y[self._pos(org_x, org_y)] = v - org_y

        # NOTE: Fill array by BFS
        def fill_next_index(next_index, h, w, flg):
            seen = OrderedDict()
            for v in range(h):
                for u in range(w):
                    if next_index[self._pos(u, v)] != INF:
                        continue
                    deq = deque((), 32, False)
                    deq.append((u, v))
                    seen.clear()
                    seen[self._pos(u, v)] = True
                    while deq:
                        x, y = deq.popleft()
                        if next_index[self._pos(x, y)] != INF:
                            next_index[self._pos(u, v)] = next_index[self._pos(
                                x, y)] + (x, y)[flg] - (u, v)[flg]
                            break
                        for i in range(-1, 2):
                            for j in range(-1, 2):
                                if (0 <= x + i < w) and (0 <= y + j < h) and (self._pos(x+i, y+j) not in seen):
                                    deq.append((x+i, y+j))
                                    seen[self._pos(x+i, y+j)] = True

        fill_next_index(self._next_index_x, height, width, 0)
        fill_next_index(self._next_index_y, height, width, 1)

    def calc(self, coordinate):
        x = coordinate.x + self._next_index_x[self._pos(coordinate.x, coordinate.y)]
        y = coordinate.y + self._next_index_y[self._pos(coordinate.x, coordinate.y)]
        return Coordinate(x=x, y=y, w=coordinate.w, h=coordinate.h)


def on_receive(request):
    global i2c_received_id, i2c_continued_id_count, is_adjusting_white_balance
    i2c_received_id = request
    i2c_continued_id_count = -1
    if request == 1:
        is_adjusting_white_balance = True


def on_transmit():
    global i2c_received_id, i2c_continued_id_count
    i2c_continued_id_count += 1

    if i2c_received_id >= 100:
        ret = (
            orange_ball_pos.x, orange_ball_pos.y,
            yellow_goal_pos.x, yellow_goal_pos.y, yellow_goal_pos.w,
            blue_goal_pos.x, blue_goal_pos.y, blue_goal_pos.w
        )[i2c_received_id - 100]
        if i2c_continued_id_count == 0:
            return min(200, ret) if ret != -1 else 0
        else:
            return max(200, ret - 200) if ret != -1 else 255
    if i2c_received_id == 1:
        if i2c_continued_id_count == 0:
            if is_adjusting_white_balance:
                i2c_continued_id_count -= 1
                return 2
            else:
                return 1
        else:
            color = i2c_continued_id_count - 1
            return current_white_balance[color]
    else:
        width = snapshot_image.width()
        count = i2c_continued_id_count // 2
        y = count // width
        x = count % width
        rgb = snapshot_image.get_pixel(x, y)
        if i2c_continued_id_count % 2 == 0:
            # top 8 bit of rgb565 format (RRRRRGGG)
            r = rgb[0] >> 3
            g = rgb[1] >> 2
            return (r << 3) | (g >> 3)
        else:
            # lower 8 bit of rgb565 format (GGGBBBBB)
            g = rgb[1] >> 2
            b = rgb[2] >> 3
            return ((g & 0b111) << 5) | b


def on_event(_):
    pass


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


def auto_white_blance():
    # TODO: impl
    return 0


def setup_i2c(scl, sda, address):
    global i2c
    i2c = I2C(
        I2C.I2C0,
        mode=I2C.MODE_SLAVE,
        scl=scl,
        sda=sda,
        addr=address,
        addr_size=7,
        on_receive=on_receive,
        on_transmit=on_transmit,
        on_event=on_event,
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

    # TODO: gein値の読み出し
    camera_gain = 0
    sensor.set_auto_gain(False, camera_gain)


def print_coordinate(coordinate, name=None,  end="\n"):
    if name is not None:
        print(name + ": ", end="")
    print("({}, {}, {}, {})".format(
        coordinate.x,
        coordinate.y,
        coordinate.h,
        coordinate.w,
    ), end=end)


if __name__ == "__main__":
    main()
