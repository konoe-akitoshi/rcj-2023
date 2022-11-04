import cv2
import numpy as np
import math
from numpy.core.defchararray import greater
from numpy.core.numeric import NaN
from numpy.core.records import get_remaining_size
import serial

ser = serial.Serial('/dev/tty1', '9600', timeout=0.1)

lower_green=np.array([100,100,70])
upper_green=np.array([120,225,220])

lower_yellow=np.array([10,100,125])
upper_yellow=np.array([22,235,180])

lower_ball=np.array([0,100,53])
upper_ball=np.array([0,100,100])

cap = cv2.VideoCapture(-1)
while True:
    i_o=0
    i_y=0
    i_b=0
    x_list_grren=[]
    y_list_green=[]
    x_list_yellow=[]
    y_list_yellow=[]
    green=[]
    yellow=[]
    ball=[]
    ball_pos=[]
    min_range_green=()
    min_range_yellow=()
    _,frame=cap.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
    mask_ball=cv2.inRange(hsv, lower_ball, upper_ball)
    _,contours, hierarchy  = cv2.findContours(mask_ball, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    rects = []
    for contour in contours:
        approx = cv2.convexHull(contour)
        rect = cv2.boundingRect(approx)
        rects.append(np.array(rect))
    if len(rects) > 0:
        ball = max(rects, key=(lambda x: x[2] * x[3])).tolist()
        if ball[2]*ball[3]<=10:
            ball=0
    _,goal_green,trash_green=cv2.findContours(mask_green, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    _,goal_yellow,trash_yellow=cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if len(goal_green)>0:
        green_pos=max(goal_green,key=cv2.contourArea)
        if cv2.contourArea(green_pos)>1000:
            buf_green=green_pos.flatten()
            for i, elem in enumerate(buf_green):
                if i%2==0:
                    x_list_grren.append(elem)
                else:
                    y_list_green.append(elem*(-1))
            green.append(x_list_grren)
            green.append(y_list_green)
        else:
            green=np.nan
            green=[[green,green]]
    else:
        green=np.nan
        green=[[green,green]]
    green=np.array(green).T.tolist()
    
    if len(goal_yellow)>0:
        yellow_pos=max(goal_yellow,key=cv2.contourArea)
        if cv2.contourArea(yellow_pos)>20:
            buf_yellow=yellow_pos.flatten()
            for i, elem in enumerate(buf_yellow):
                if i%2==0:
                    x_list_yellow.append(elem)
                else:
                    y_list_yellow.append(elem*(-1))
            yellow.append(x_list_yellow)
            yellow.append(y_list_yellow)
        else:
            yellow=np.nan
            yellow=[[yellow,yellow]]
    else:
        yellow=np.nan
        yellow=[[yellow,yellow]]
    yellow=np.array(yellow).T.tolist()
    
    if not math.isnan(green[0][0]):
        min_range_green=min(green,key=lambda i: (i[0]-160)**2+(120-i[1])**2)
    else:
        i_b=i_b+1
        min_range_green=[0,0]
    if not math.isnan(yellow[0][0]):
        min_range_yellow=min(yellow,key=lambda i: (i[0]-160)**2+(120-i[1])**2)
    else:
        i_y=i_y+1
        min_range_yellow[0,0]
    if type(ball)!=list:
        if ball==0:
            i_o=i_o+1
    else:
        if ball==[]:
            i_o=i_o+1
            ball_pos=[0,0]
        else:
            ball_pos=[ball[0]+(ball[2]/2),ball[1]+(ball[3]/2)]
    len(ball_pos)
    print('\r green is'+str(min_range_green)+'yellow is'+str(min_range_yellow)+'ball is '+str(ball_pos))
    ser.write(bytes([254]))
    ser.write(bytes(i_o))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([ball_pos[0] & 0b00000000000111111]))
    ser.write(bytes([(ball_pos[0] & 0b0000111111000000)>>6]))
    ser.write(bytes([ball_pos[1] & 0b00000000000111111]))
    ser.write(bytes([(ball_pos[1] & 0b0000111111000000)>>6]))
    ser.write(bytes([253]))
    ser.write(bytes([i_y]))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([min_range_yellow[0] & 0b00000000000111111]))
    ser.write(bytes([(min_range_yellow[0] & 0b0000111111000000)>>6]))
    ser.write(bytes([min_range_yellow[1] & 0b00000000000111111]))
    ser.write(bytes([(min_range_yellow[1] & 0b0000111111000000)>>6]))
    ser.write(bytes([252]))
    ser.write(bytes([i_b]))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([0]))
    ser.write(bytes([min_range_green[0] & 0b00000000000111111]))
    ser.write(bytes([(min_range_green[0] & 0b0000111111000000)>>6]))
    ser.write(bytes([min_range_green[1] & 0b00000000000111111]))
    ser.write(bytes([(min_range_green[1] & 0b0000111111000000)>>6]))

    cv2.imshow(frame,frame)