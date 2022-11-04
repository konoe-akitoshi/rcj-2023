#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
[summary]
  cam remapping test
[description]
  -
"""

import argparse
import copy

import cv2 as cv

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
                rp = math.sqrt(pow(x,2)+pow(y,2))
                Rp = (  -(b2-c2)*H*rp ) \
                     /( (b2 + c2)*f - 2*b*c*math.sqrt(math.sqrt(pow(rp,2)+f2)))                    
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

        cv.imshow('ORIGINAL', frame)
        cv.imshow('POLAR', dst)
        #cv.imshow('linear_polar_inverse_image', linear_polar_inverse_image)

        key = cv.waitKey(50)
        if key == 27:  # ESC
            break

    cap.release()
    cv.destroyAllWindows()


if __name__ == '__main__':
    main()