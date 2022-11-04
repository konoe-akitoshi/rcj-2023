#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
[summary]
  cam remapping test
[description]
  -
"""

from __future__ import (
    division, absolute_import, print_function, unicode_literals)

import cv2 as cv

import argparse
import copy

import math
import numpy as np
import random

def get_args():

    parser = argparse.ArgumentParser()

    parser.add_argument("--width", help='capture width', type=int, default=960)
    parser.add_argument(
        "--height", help='capture height', type=int, default=540)

    args = parser.parse_args()

    return args

def main():
    args = get_args()
    cap_width = args.width
    cap_height = args.height

    cap = cv.VideoCapture('image/1.mp4')
    cap.set(cv.CAP_PROP_FRAME_WIDTH, cap_width)
    cap.set(cv.CAP_PROP_FRAME_HEIGHT, cap_height)
    
    b = 42.7
    b2 = pow(b,2)
    c = 59.9
    c2 = pow(c,2)
    d = 13.125
    Z = 182.5
    f = 3.04 #[mm]   # * 0.001  #[m]
    f2 = pow(f,2)
    H = 200
    while True: 
        ret, frame = cap.read()
        if not ret:
            print('cap.read() error')
            break

        h, w = frame.shape[:2]
        #print(h,w)

        map_x = np.zeros((h, w), dtype=np.float32)
        map_y = np.zeros((h, w), dtype=np.float32)
        pitch = 50
        
        for i in range(h):
            for j in range(w):
                x = (j-w/2)
                y = (i-h/2)
                if x == 0:
                    x = 0.0001
                X = x*((pow(b,2)+pow(c,2))*Z-2*b*c*math.sqrt(pow(y,2)+pow(x,2)+pow(Z,2)))/f*(pow(b,2)-pow(c,2))
                Y = y*((pow(b,2)+pow(c,2))*Z-2*b*c*math.sqrt(pow(y,2)+pow(x,2)+pow(Z,2)))/f*(pow(b,2)-pow(c,2))
                sc = 10
                #print(X,Y)
                if   i < (h/2) and j > (w/2):
                    x_a =  -X/sc
                    map_x[i,j] = (w/2)+(x_a)
                    y_a =  -Y/sc
                    map_y[i,j] = (h/2)+(y_a)
                    #print(x_a,y_a)
                if   i < (h/2) and j < (w/2):
                    x_a =  X/sc
                    map_x[i,j] = (w/2)+(x_a)
                    y_a =  Y/sc
                    map_y[i,j] = (h/2)+(y_a)
                    #print(x_a,y_a)
                if   i > (h/2) and j > (w/2):
                    x_a =  -X/sc
                    map_x[i,j] = (w/2)+(x_a)
                    y_a =  -Y/sc
                    map_y[i,j] = (h/2)+(y_a)
                    #print(x_a,y_a)
                if   i > (h/2) and j < (w/2):
                    x_a =  X/sc
                    map_x[i,j] = (w/2)+(x_a)
                    y_a =  Y/sc
                    map_y[i,j] = (h/2)+(y_a)
                    #print(x_a,y_a)
        dst = cv.remap(frame, map_x, map_y, cv.INTER_CUBIC)

# Insert any filename with path
img = cv.imread(dst)

def white_balance_loops(img):
    result = cv.cvtColor(img, cv.COLOR_BGR2LAB)
    avg_a = np.average(result[:, :, 1])
    avg_b = np.average(result[:, :, 2])
    for x in range(result.shape[0]):
        for y in range(result.shape[1]):
            l, a, b = result[x, y, :]
            # fix for CV correction
            l *= 100 / 255.0
            result[x, y, 1] = a - ((avg_a - 128) * (l / 100.0) * 1.1)
            result[x, y, 2] = b - ((avg_b - 128) * (l / 100.0) * 1.1)
    result = cv.cvtColor(result, cv.COLOR_LAB2BGR)
    return result

final = np.hstack((img, white_balance_loops(img)))
show(final)
cv.imwrite('result.jpg', final)