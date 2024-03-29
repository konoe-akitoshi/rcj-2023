import sensor
import array
from machine import I2C
from ucollections import OrderedDict, deque
from micropython import const

CAMERA_ID = const(0)

I2C_SCL = const(8)
I2C_SDA = const(15)
I2C_ADDRESS = [0x21, 0x22, 0x23, 0x24][CAMERA_ID % 4]

CAMERA_MATRIX = [
    # (fx, fy, cx, cy)
    (178.26219383, 174.55480262, 164.05832996, 120.72683426),
    (168.31718333, 162.07631628, 164.15257426, 132.58607849),
    (196.14281507, 193.15226204, 152.70212396, 121.6860914),
    (176.72885013, 175.7561839, 155.8685601, 100.75748632),
    (190.6726142, 189.95512237, 161.17992394, 122.43791516),
    (183.77940283, 180.58932889, 159.05666669, 119.84969543),
    (168.24346836, 161.36620577, 154.91805431, 121.86099262),
    (177.25703082, 173.98876482, 158.56059344, 113.91893924)
][CAMERA_ID]
CAMERA_MATRIX2 = [
    # (fx, fy, cx, cy)
    (115.6967926, 112.57094574, 159.81323153, 117.38469573),
    (98.77628326, 97.59822845, 162.38411724, 116.27783629),
    (128.9127655, 123.95737457, 161.36715257, 119.45370213),
    (104.95248413, 106.48150635, 160.70338671, 110.96302721),
    (113.10139465, 115.09963989, 162.66912637, 114.34472715),
    (112.60968018, 111.57545471, 156.47516689, 116.5741862),
    (106.77736664, 104.08617401, 156.69910472, 114.7890009),
    (127.91661835, 123.61178589, 161.84704617, 111.42362326)
][CAMERA_ID]
CAMERA_DISTORTION_COEFFICIENTS = [
    # (k1, k2, p1, p2, k3)
    (-0.27371726, 0.0161859, -0.00416123, -0.00365649, 0.01695662),
    (-0.26118542, 0.03938639, -0.01656912, -0.00070669, 0.00137975),
    (-0.40380245, 0.13560102, -0.00242414, 0.0095991, -0.00866844),
    (-0.25877132, 0.04121341, 0.01078522, 0.00426109, 0.0005979),
    (-0.29822902, 0.04730201, -0.00787524, 0.0020719, 0.00352898),
    (-0.32647266, 0.08234189, -0.00273987, -0.00180897, -0.00505706),
    (-0.27500382, 0.02699462, -0.0097967, 0.00344366, 0.01149935),
    (-0.18636316, -0.18311137, -0.00424549, 0.00649478, 0.12430401)
][CAMERA_ID]

# (L Min, L Max, A Min, A Max, B Min, B Max)
ORAMGE_COLOR_THRESHOLD = [(0, 80, 40, 60, 20, 40)]
YELLOW_COLOR_THRESHOLD = [(65, 85, -15, 5, 20, 40)]
BLUE_COLOR_THRESHOLD = [(30, 65, -5, 20, -40, -25)]

is_adjusting_white_balance = False
current_white_balance = (0, 0, 0)

data_register = array.array("B", [
    25,      # section of orange ball
    25, 0,  # section of yellow goal, half width of yellow goal
    25, 0   # section of blue goal, half width of blue goal
])

i2c = None
i2c_received_id = -1
i2c_continued_id_count = 0
snapshot_image = None


def main():
    global data_register, snapshot_image

    setup_sensor(dual_buff=True)
    calibration = Calibration(320, 240)
    setup_i2c(scl=I2C_SCL, sda=I2C_SDA, address=I2C_ADDRESS)

    while True:
        if is_adjusting_white_balance:
            auto_white_blance()
        image = sensor.snapshot()

        orange = track_color(
            image=image,
            color_threshold=ORAMGE_COLOR_THRESHOLD,
            pixels_threshold=10,
            area_threshold=10
        )

        yellow = track_color(
            image=image,
            color_threshold=YELLOW_COLOR_THRESHOLD,
            pixels_threshold=250,
            area_threshold=250
        )

        blue = track_color(
            image=image,
            color_threshold=BLUE_COLOR_THRESHOLD,
            pixels_threshold=250,
            area_threshold=250
        )

        fixed_orange = calibration.calc(orange[0], orange[1])
        fixed_yellow = calibration.calc(yellow[0], yellow[1])
        fixed_blue = calibration.calc(blue[0], blue[1])

        data_register[0] = separate_section(*fixed_orange)
        data_register[1] = separate_section(*fixed_yellow)
        data_register[2] = yellow[2] // 2
        data_register[3] = separate_section(*fixed_blue)
        data_register[4] = blue[2] // 2
        snapshot_image = image


class Calibration:
    def _pos(self, x, y):
        return y * (self.width // 2) + x

    def __init__(self, width, height):
        global CAMERA_MATRIX, CAMERA_MATRIX2, CAMERA_DISTORTION_COEFFICIENTS
        self.width = width
        self.height = height
        self._next_index_x = array.array("b")
        self._next_index_y = array.array("b")
        fx, fy, cx, cy = CAMERA_MATRIX
        fx2, fy2, cx2, cy2 = CAMERA_MATRIX2
        k1, k2, p1, p2, k3 = CAMERA_DISTORTION_COEFFICIENTS
        INF = const(127)
        for _ in range(height*width//4):
            self._next_index_x.append(INF)
            self._next_index_y.append(INF)

        for v in range(0, height, 2):
            for u in range(0, width, 2):
                x = (u - cx2) / fx2
                y = (v - cy2) / fy2
                rr = x ** 2 + y ** 2
                K = 1 + k1*rr + k2*(rr**2) + k3*(rr**3)

                x2 = x*K + 2*p1*x*y + p2*(rr+2*(x**2))
                y2 = y*K + p1*(rr+2*(y**2)) + 2*p2*x*y
                org_x = round(x2*fx + cx)
                org_y = round(y2*fy + cy)

                if 0 <= org_x < width and 0 <= org_y < height:
                    self._next_index_x[self._pos(
                        org_x//2, org_y//2)] = u - org_x
                    self._next_index_y[self._pos(
                        org_x//2, org_y//2)] = v - org_y

        # NOTE: Fill array by BFS
        def fill_next_index(next_index, h, w, flg):
            seen = OrderedDict()
            for v in range(h//2):
                for u in range(w//2):
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
                                x, y)] + (x, y)[flg] * 2 - (u, v)[flg] * 2
                            break
                        for i in range(-1, 2):
                            for j in range(-1, 2):
                                if (0 <= x + i < w) and (0 <= y + j < h) and (self._pos(x+i, y+j) not in seen):
                                    deq.append((x+i, y+j))
                                    seen[self._pos(x+i, y+j)] = True

        fill_next_index(self._next_index_x, height, width, 0)
        fill_next_index(self._next_index_y, height, width, 1)

    def calc(self, x,  y):
        if x < 0 or y < 0:
            return -1, -1
        ret_x = x + self._next_index_x[self._pos(x//2, y//2)]
        ret_y = y + self._next_index_y[self._pos(x//2, y//2)]
        return ret_x, ret_y


def separate_section(x, y):
    if x < 0:
        return 25
    section_x = x // 40
    section_y = y // 80
    return section_x + section_y * 8 + 1


def on_receive(request):
    global i2c_received_id, i2c_continued_id_count, is_adjusting_white_balance
    i2c_received_id = request
    i2c_continued_id_count = -1
    if request == 1:
        is_adjusting_white_balance = True


def on_transmit():
    global i2c_received_id, i2c_continued_id_count
    i2c_continued_id_count += 1

    if i2c_received_id == 100:
        return data_register[i2c_continued_id_count % 5]
    elif i2c_received_id == 1:
        if i2c_continued_id_count == 0:
            if is_adjusting_white_balance:
                i2c_continued_id_count -= 1
                return 2
            else:
                return 1
        else:
            color = i2c_continued_id_count - 1
            return current_white_balance[color]
    elif i2c_received_id == 2:
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
    elif i2c_received_id == 3:
        return 2 if snapshot_image is None else 1
    else:
        return 0


def on_event(_):
    pass


def track_color(image,
                color_threshold,
                pixels_threshold,
                area_threshold):
    """Tracks specified color and returns max blob data
    Args:
      image (Image): the image object you want to color track.
      color_threshold (List<Tupple>): [(l_lo, l_hi, a_lo, a_hi, b_lo, b_hi)]
      pixels_threshold (int):
      area_threshold (int):
    Returns:
        Coordinate
    """
    blobs = image.find_blobs(
        color_threshold,
        pixels_threshold=pixels_threshold,
        area_threshold=area_threshold
    )
    if (blobs is None) or (len(blobs) == 0):
        return -1, -1, 0
    max_blob = max(blobs, key=lambda x: x.area())
    return max_blob.cx(), max_blob.cy(), max_blob.w()


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
        freq=400000,
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


if __name__ == "__main__":
    main()
