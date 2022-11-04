# Orange Ball Tracking program for RCJJ
# Multi Color Version
# written by H.Saeki
# Sep.22.2019
#

import sensor, image, time
from pyb import UART
from pyb import LED

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
obj_color_orange =   [(0, 64, 27, 127, -44, 127)]      # Orange Ball
obj_color_yellow =   [(33, 78, -12, 6, 32, 127)]      # Yellow Goal
obj_color_blue   =   [(20, 28, -59, 11, -28, -7)]     # Blue Goal

# You may pass up to 16 thresholds above. However, it's not really possible to segment any
# scene with 16 thresholds before color thresholds start to overlap heavily.

x_data_ball=0
y_data_ball=0
w_data_ball=0
h_data_ball=0
ball_area=0

x_data_goal_y=0
y_data_goal_y=0
w_data_goal_y=0
h_data_goal_y=0
goal_y_area=0

x_data_goal_b=0
y_data_goal_b=0
w_data_goal_b=0
h_data_goal_b=0
goal_b_area=0

i_o=0
i_y=0
i_b=0

red_led   = LED(1)
green_led = LED(2)
blue_led  = LED(3)
ir_led    = LED(4)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA) #320x240
sensor.skip_frames(time = 56)
sensor.set_auto_gain(False, gain_db = 23) # must be turned off for color tracking
sensor.set_auto_whitebal(False, rgb_gain_db = (63.49, 60.21, 64.32)) # must be turned off for color tracking
clock = time.clock()
uart = UART(3, 19200, timeout_char = 1000)

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. Don't set "merge=True" becuase that will merge blobs which we don't want here.

while(True):
    clock.tick()
    img = sensor.snapshot()

# check Orange Ball
    i_o = 0
    balls_o1 = img.find_blobs(obj_color_orange, pixels_threshold=10, area_threshold=10)
    balls_o2 = sorted(balls_o1, key = lambda x: x.area())
    if balls_o1 != None:
        for ball in balls_o2:
            x_data_ball=ball.cx()
            y_data_ball=ball.cy()
            w_data_ball=ball.w()
            h_data_ball=ball.h()
            ball_area=ball.area()

            img.draw_rectangle(ball.rect(), color=(255,0,0)) # red
            img.draw_cross(ball.cx(), ball.cy())
            i_o=i_o+1
#            print("Orange Ball ", end="")
#            print(" number=%d" % i_o, end="")
#            print(" ball_area=%d" % ball_area, end="")
#            print(" ball_x=%d" % x_data_ball, end="")
#            print(" ball_y=%d" % y_data_ball)
    if i_o != 0:
        green_led.off()
    else:
        green_led.off()
        x_data_ball=-1
        y_data_ball=-1
        w_data_ball=-1
        h_data_ball=-1
        ball_area=-1

# check Yellow Goal
    i_y = 0
    balls_y1 = img.find_blobs(obj_color_yellow, pixels_threshold=100, area_threshold=100)
    balls_y2 = sorted(balls_y1, key = lambda x: x.area())
    if balls_y1 != None:
        for ball in balls_y2:
            x_data_goal_y=ball.cx()
            y_data_goal_y=ball.cy()
            w_data_goal_y=ball.w()
            h_data_goal_y=ball.h()
            goal_y_area=ball.area()

            img.draw_rectangle(ball.rect(), color=(0,255,0)) # green
            img.draw_cross(ball.cx(), ball.cy())
            i_y=i_y+1
#            print("Yellow Goal ", end="")
#            print(" number=%d" % i_y, end="")
#            print(" ball_area=%d" % goal_y_area, end="")
#            print(" ball_x=%d" % x_data_goal_y, end="")
#            print(" ball_y=%d" % y_data_goal_y)
    if i_y != 0:
        red_led.off()
    else:
        red_led.off()
        x_data_goal_y=-1
        y_data_goal_y=-1
        w_data_goal_y=-1
        h_data_goal_y=-1
        goal_y_area=-1

# check Blue Goal
    i_b = 0
    balls_b1 = img.find_blobs(obj_color_blue, pixels_threshold=100, area_threshold=100)
    balls_b2 = sorted(balls_b1, key = lambda x: x.area())
    if balls_b1 != None:
        for ball in balls_b2:
            x_data_goal_b=ball.cx()
            y_data_goal_b=ball.cy()
            w_data_goal_b=ball.w()
            h_data_goal_b=ball.h()
            goal_b_area=ball.area()

            img.draw_rectangle(ball.rect(), color=(0,0,255)) # green
            img.draw_cross(ball.cx(), ball.cy())
            i_b=i_b+1
#            print("Blue Goal ", end="")
#            print(" number=%d" % i_y, end="")
#            print(" ball_area=%d" % goal_b_area, end="")
#            print(" ball_x=%d" % x_data_goal_b, end="")
#            print(" ball_y=%d" % y_data_goal_b)
    if i_b != 0:
        blue_led.off()
    else:
        blue_led.off()
        x_data_goal_b=-1
        y_data_goal_b=-1
        w_data_goal_b=-1
        h_data_goal_b=-1
        goal_b_area=-1

# Send Data
# Send Orange Ball Data
    uart.writechar(254)
    uart.writechar(i_o)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(x_data_ball & 0b00000000000111111)
    uart.writechar((x_data_ball & 0b0000111111000000)>>6)
    uart.writechar(y_data_ball & 0b00000000100111111)
    uart.writechar((y_data_ball & 0b0000111111000000)>>6)
    uart.writechar(w_data_ball & 0b00000000000111111)
    uart.writechar((w_data_ball & 0b0000111111000000)>>6)
    uart.writechar(h_data_ball & 0b00000000100111111)
    uart.writechar((h_data_ball & 0b0000111111000000)>>6)
    print(clock.fps(), end="")
    print(" ***** ", end="")
    print(" Orange ball_No=%d" % i_o, end="")
    print(" Orange ball_x=%d" % x_data_ball, end="")
    print(" Orange ball_y=%d" % y_data_ball, end="")
    print(" Orange ball_h=%d" % w_data_ball, end="")
    print(" Orange ball_w=%d" % h_data_ball, end="")
    print(" ***** ")

# Send Yellow Goal Data
    uart.writechar(253)
    uart.writechar(i_y)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(x_data_goal_y & 0b00000000000111111)
    uart.writechar((x_data_goal_y & 0b0000111111000000)>>6)
    uart.writechar(y_data_goal_y & 0b00000000100111111)
    uart.writechar((y_data_goal_y & 0b0000111111000000)>>6)
    uart.writechar(w_data_goal_y & 0b00000000000111111)
    uart.writechar((w_data_goal_y & 0b0000111111000000)>>6)
    uart.writechar(h_data_goal_y & 0b00000000100111111)
    uart.writechar((h_data_goal_y & 0b0000111111000000)>>6)
    print(" ***** ", end="")
    print(" Yellow Goal_No=%d" % i_y, end="")
    print(" Yellow Goal_x=%d" % x_data_goal_y, end="")
    print(" Yellow Goal_y=%d" % y_data_goal_y, end="")
    print(" Yellow Goal_h=%d" % w_data_goal_y, end="")
    print(" Yellow Goal_w=%d" % h_data_goal_y, end="")
    print(" ***** ")

# Send Blue Goal Data
    uart.writechar(252)
    uart.writechar(i_b)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(0)
    uart.writechar(x_data_goal_b & 0b00000000000111111)
    uart.writechar((x_data_goal_b & 0b0000111111000000)>>6)
    uart.writechar(y_data_goal_b & 0b00000000100111111)
    uart.writechar((y_data_goal_b & 0b0000111111000000)>>6)
    uart.writechar(w_data_goal_b & 0b00000000000111111)
    uart.writechar((w_data_goal_b & 0b0000111111000000)>>6)
    uart.writechar(h_data_goal_b & 0b00000000100111111)
    uart.writechar((h_data_goal_b & 0b0000111111000000)>>6)
    print(" ***** ", end="")
    print(" Blue   Goal_No=%d" % i_b, end="")
    print(" Blue   Goal_x=%d" % x_data_goal_b, end="")
    print(" Blue   Goal_y=%d" % y_data_goal_b, end="")
    print(" Blue   Goal_h=%d" % w_data_goal_b, end="")
    print(" Blue   Goal_w=%d" % h_data_goal_b, end="")
    print(" ***** ")

#    time.sleep(500)
