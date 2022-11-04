#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
[summary]
  cam remapping test
[description]
  -
"""
"""
GUIDE

np.average (numpy)
https://deepage.net/features/numpy-average.html

cv.VideoCapture() 
https://note.nkmk.me/python-opencv-videocapture-file-camera/

"""

#from __future__ import (division, absolute_import, print_function, unicode_literals)

import cv2 as cv

import argparse
import copy

import math
import numpy as np
import random

#ホワイトバランス用の画像読み込み
#img = cv.imread('image')



#ホワイトバランス（動画の切り抜きのL*A*B*値を調整）
def white_balance_loops(img):

    result = cv.cvtColor(img, cv.COLOR_BGR2LAB)  #画像をBGR->L*A*B*に変換  result[ 画像x座標, 画像y座標, 0=L_1=a_2=b]

    avg_a = np.average(result[:, :, 1])  #画像全体のA*B*値の平均を取得　avg_a,avg_b=float?
    avg_b = np.average(result[:, :, 2])

    for x in range(result.shape[0]):  #各ピクセルでL*A*B*値を調整(ホワイトバランス)
        for y in range(result.shape[1]):
            l, a, b = result[x, y, :]  #座標(x,y)のL*A*B*値を変数にいれる  l,a,b=float?
            l *= 100 / 255.0  #調整(L値はそのまま？保存しないのかな)
            result[x, y, 1] = a - ((avg_a - 128) * (l / 100.0) * 1.1)
            result[x, y, 2] = b - ((avg_b - 128) * (l / 100.0) * 1.1)

    result = cv.cvtColor(result, cv.COLOR_LAB2BGR)  #ホワイトバランスしたresult[]をL*A*B*->BGRに変換

    return result
#final = np.hstack((img, white_balance_loops(img)))


#remapping
def get_args():

    parser = argparse.ArgumentParser()
    parser.add_argument("--width", help='capture width', type=int, default=960)
    parser.add_argument(
        "--height", help='capture height', type=int, default=540)

    args = parser.parse_args()

    return args

#メイン
def main():
    #args = get_args()
    #cap_width = args.width
    #cap_height = args.height

    #動画の読み込み(バグったらVideo I/Oの確認) cap=<class 'cv2.VideoCapture'>
    cap = cv.VideoCapture(r'D:\Creation\RCJ\guto_code\Test.mp4')
    #cap = cv.VideoCapture(1)
    #cap = final

    #縦横の長さの変更
    cap.set(cv.CAP_PROP_FRAME_HEIGHT, 100)
    cap.set(cv.CAP_PROP_FRAME_WIDTH, 100)
    
    b = 42.7
    b2 = pow(b,2)
    c = 59.9
    c2 = pow(c,2)
    d = 13.125
    Z = 182.5
    f = -3.04 #[mm]   # * 0.001  #[m]
    f2 = pow(f,2)
    H = 200

    #各フレームで実行するためのWhileループ
    while True: 

        #フレーム用意
        ret, frame = cap.read()  #フレームの読み込み&次のフレームに移行 ret=bool(読み込めたか),frame=<class 'numpy.ndarray'>
        if not ret:  #読み込み失敗orフレームの終わりの時
            print('cap.read() error')
            break

        #下準備
        h, w = frame.shape[:2]  #height,widthをh,wにいれる
        map_x = np.zeros((h, w), dtype=np.float32)  #h*w次元配列を作る map_x,map_y=<class 'numpy.ndarray'>
        map_y = np.zeros((h, w), dtype=np.float32)
        pitch = 50
        
        #
        for i in range(h):  #Y座標
            for j in range(w):  #X座標

                x = (j-w/2)  #原点を左上から中心に移動
                y = (i-h/2)

                if x == 0:  #0除算回避
                    x = 0.0001
                
                #数学タイム
                rp = math.sqrt(pow(x,2)+pow(y,2))  #原点からの距離？
                Rp = (  -(b2-c2)*H*rp ) / ( (b2 + c2)*f - 2*b*c*math.sqrt(math.sqrt(pow(rp,2)+f2)))
                X = Rp/(math.sqrt(1+(pow(y,2)/pow(x,2))))
                Y = (y/x)*X
                sc = 5
                
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
        final = white_balance_loops(frame)
        dst = cv.remap(final, map_x, map_y, cv.INTER_CUBIC)
        
        #cv.imshow('ORIGINAL', frame)
        #cv.resize(dst,dsize=(20,20))

        #表示
        cv.imshow('output',dst)

        #緊急脱出
        key = cv.waitKey(50)
        if key == 27:  # ESC
            break

    cap.release()  #動画を閉じる
    cv.destroyAllWindows()  #ウィンドウ全削除


#本体
if __name__ == '__main__':
    main()
