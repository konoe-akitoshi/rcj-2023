#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
[summary]
cam remapping test
[description]
"""
"""
GUIDE

np.average (numpy)
https://deepage.net/features/numpy-average.html

cv.VideoCapture() 
https://note.nkmk.me/python-opencv-videocapture-file-camera/

https://pystyle.info/opencv-resize/

数学タイムはこれの5.2のとこ
https://www.slideshare.net/AkitoshiSaeki/2020-245976441
"""
#目標

#前回 320x240*(30-40)FPS
#今回 640x480*(30-40)FPS
"""
n*C回検知
n分割 C層
連続する角度が一番広い角度を使用

"""

#from __future__ import (division, absolute_import, print_function, unicode_literals)

import cv2 as cv

import argparse
import copy

import math
import numpy as np
import random

import time

import serial

# ホワイトバランス（動画の切り抜きのL*A*B*値を調整）
def white_balance_loops(img, h, w):

    result = cv.cvtColor(img, cv.COLOR_BGR2LAB)  # 画像をBGR->L*A*B*に変換  result[ 画像x座標, 画像y座標, 0=L_1=a_2=b]

    avg_a = int(256*np.average(result[:, :, 1]))-32768  # 画像全体のA*B*値の平均を取得　avg_a,avg_b=double
    avg_b = int(256*np.average(result[:, :, 2]))-32768  # 32768=128*256

    for i in range(h):  # 各ピクセルでL*A*B*値を調整(ホワイトバランス)
        for j in range(w):
            l, a, b = result[i, j, :]  # 座標(x,y)のL*A*B*値を変数にいれる  l,a,b=int?
            l *= 35  # 調整  35.2=1.1/256*(2**13)
            result[i, j, 1] = ((a<<21) - (avg_a * l))>>21
            result[i, j, 2] = ((b<<21) - (avg_b * l))>>21

    result = cv.cvtColor(result, cv.COLOR_LAB2BGR)  # ホワイトバランスしたresult[]をL*A*B*->BGRに変換

    return result

# 色検知
def color_detect(img):
    # HSV色空間に変換
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)

    # 赤色のHSVの値域1
    hsv_min_red = np.array([-7, 25, 140])
    hsv_max_red = np.array([15, 140, 217])
    mask_red = cv.inRange(hsv, hsv_min_red, hsv_max_red)

    # 黄色のHSVの値域
    hsv_min_yellow = np.array([24, 102, 204])
    hsv_max_yellow = np.array([26, 153, 255])
    mask_yellow = cv.inRange(hsv, hsv_min_yellow, hsv_max_yellow)

    # 青色のHSVの値域
    hsv_min_blue = np.array([98, 175, 190])
    hsv_max_blue = np.array([101, 230, 230])
    mask_blue = cv.inRange(hsv, hsv_min_blue, hsv_max_blue)

    return mask_red, mask_yellow, mask_blue

# ブロブ解析
def analysis_blob(binary_img):
    # 2値画像のラベリング処理
    label = cv.connectedComponentsWithStats(binary_img)

    # ブロブ情報を項目別に抽出
    n = label[0] - 1
    data = np.delete(label[2], 0, 0)
    center = np.delete(label[3], 0, 0)
    
    # 面積最大ブロブの情報格納用
    maxblob = {}

    data = np.append(data,np.array([[0,0,0,0,0]]),axis=0)
    # ブロブ面積最大のインデックス
    max_index = np.argmax(data[:, 4])

    #data[:,4]=0の時は見つからなかったとき
    if data[:, 4][max_index]:
        # 面積最大ブロブの各種情報を取得
        maxblob["upper_left"] = (data[:, 0][max_index], data[:, 1][max_index]) # 左上座標
        maxblob["width"] = data[:, 2][max_index]  # 幅
        maxblob["height"] = data[:, 3][max_index]  # 高さ
        maxblob["area"] = data[:, 4][max_index]   # 面積
        maxblob["center"] = center[max_index]  # 中心座標
        maxblob["found"] = True  #見つかったかどうか
    else:
        maxblob["found"] = False
    
    return maxblob

# データ送信
def send_data(target_data, target_type):

    if target_data["found"]:
        i_o=200
        i_y=201
        i_b=202

        x_data = int(target_data["center"][0])
        y_data = int(target_data["center"][1])
        w_data = int(target_data["width"])
        h_data = int(target_data["height"])
        
        uart.write(bytes([target_type]))
        uart.write(bytes([i_o]))
        uart.write(bytes([0]))
        uart.write(bytes([0]))
        uart.write(bytes([0]))
        uart.write(bytes([x_data & 0b00000000000111111]))
        uart.write(bytes([(x_data & 0b0000111111000000)>>6]))
        uart.write(bytes([y_data & 0b00000000100111111]))
        uart.write(bytes([(y_data & 0b0000111111000000)>>6]))
        uart.write(bytes([w_data & 0b00000000000111111]))
        uart.write(bytes([(w_data & 0b0000111111000000)>>6]))
        uart.write(bytes([h_data & 0b00000000100111111]))
        uart.write(bytes([(h_data & 0b0000111111000000)>>6]))

        print(" ***** ", end="")
        print(" Orange ball_No=%d" % i_o, end="")
        print(" Orange ball_x=%d" % x_data, end="")
        print(" Orange ball_y=%d" % y_data, end="")
        print(" Orange ball_h=%d" % w_data, end="")
        print(" Orange ball_w=%d" % h_data, end="")
        print(" ***** ")

    else:
        for _ in range(12):
            uart.write(bytes([-1]))



# 
def coordinate_test(ball_data, yellow_goal_data, blue_goal_data):

    input()

    if ball_data["found"]:
        ball_xy = ball_data["center"]
    else:
        ball_xy = [-1, -1]
    print(f"ball_data: {ball_xy}")
    if yellow_goal_data["found"]:
        yellow_goal_xy = yellow_goal_data["center"]
    else:
        yellow_goal_xy = [-1, -1]
    print(f"yellow_goal_data: {yellow_goal_xy}")
    if blue_goal_data["found"]:
        blue_goal_xy = blue_goal_data["center"]
    else:
        blue_goal_xy = [-1, -1]
    print(f"blue_goal_data: {blue_goal_xy}")


#メイン
def main(debug_flug = False):
    print('LOADING...')
    
    # ホワイトバランス用の画像読み込み
    #img = cv.imread('image')

    # 動画の読み込み(バグったらVideo I/Oの確認) cap=<class 'cv2.VideoCapture'>
    #cap = cv.VideoCapture(r'D:\Creation\RCJ\guto_code\1.mp4')
    cap = cv.VideoCapture(0)
    #cap = final

    # 縦横の長さの変更
    Height=600
    Width=800

    print('HEIGHT',end='---')
    print(cap.set(cv.CAP_PROP_FRAME_HEIGHT, Height))
    print('WIDTH ',end='---')
    print(cap.set(cv.CAP_PROP_FRAME_WIDTH , Width))
    print('FPS',end='---')
    print(cap.set(cv.CAP_PROP_FPS, 40), end='---')
    print(cap.get(cv.CAP_PROP_FPS))

    #フレーム用意
    ret, frame = cap.read()  #フレームの読み込み&次のフレームに移行 ret=bool(読み込めたか),frame=<class 'numpy.ndarray'>
    if not ret:  #読み込み失敗orフレームの終わりの時
        print('cap.read() error')
    
    # cv.imwrite("default0.jpg", frame)

    # カメラの個体値(お触り禁止)
    b = 42.7
    b2 = pow(b,2)
    c = 59.9
    c2 = pow(c,2)
    d = 13.125
    Z = 182.5
    f = -3.04 #[mm]   # * 0.001  #[m]
    f2 = pow(f,2)
    H = 200
    

    #FPS計測器
    count=0
    start_time=0

    #下準備
    h, w = frame.shape[:2]  #height,widthをh,wにいれる
    gap = (w - h) // 2
    frame = frame[0:h, gap:w-gap]
    h, w = frame.shape[:2]

    map_x = np.zeros((h, w), dtype=np.float32)  #h*w次元配列を作る map_x,map_y=<class 'numpy.ndarray'>
    map_y = np.zeros((h, w), dtype=np.float32)



    #数学タイム用
    Half_height=int(h/2)
    Half_width=int(w/2)
    sc = 5
    
    #map_x,map_yを作ろう
    for i in range(h):  #Y座標
        for j in range(w):  #X座標
            
            x = (j-Half_width)  #原点を左上から中心に移動
            y = (i-Half_height)
            
            if x == 0:  #0除算回避
                x = 0.0001
                
            #数学タイム
            rp = math.sqrt(pow(x,2)+pow(y,2))  #原点からの距離？
            Rp = (  -(b2-c2)*H*rp ) / ( (b2 + c2)*f - 2*b*c*math.sqrt(math.sqrt(pow(rp,2)+f2)))
            X = Rp/(math.sqrt(1+(pow(y,2)/pow(x,2))))/sc
            Y = (y/x)*X

            if j > Half_width:
                map_x[i,j] = Half_width-X
                map_y[i,j] = Half_height-Y
            else:
                map_x[i,j] = Half_width+X
                map_y[i,j] = Half_height+Y
    
    
    # カラートラッキングに使うやつ

    R_num=2 # 円の数
    R_array=np.array([100,200]) #それぞれの円の半径
    Sep_num=180 # 円を分ける数

    xT = np.zeros((Sep_num,R_num), dtype=np.int16)#テーブル作り
    yT = np.zeros((Sep_num,R_num), dtype=np.int16)
    for i in range(Sep_num):
        for j in range(R_num):
            xT[i,j] = Half_width+R_array[j]*math.sin(2*math.pi*i/Sep_num)
            yT[i,j] = Half_height-R_array[j]*math.cos(2*math.pi*i/Sep_num)
            
    #閾値
    lower_yellow=np.array([0,100,125])
    upper_yellow=np.array([179,235,180])



    #どね！
    print('DONE!')
    
    #各フレームで実行するためのWhileループ -----------------------------------------------
    while True: 
    
        #時間計測
        count = count+1
        if time.perf_counter()-start_time > 1.0:
            print('FPS= ',end='')
            print(count)
            count=0
            start_time=time.perf_counter()
        
        #フレーム用意
        ret, frame = cap.read()  #フレームの読み込み&次のフレームに移行 ret=bool(読み込めたか),frame=<class 'numpy.ndarray'>
        if not ret:  #読み込み失敗orフレームの終わりの時
            print('cap.read() error')
            break
        
        h, w = frame.shape[:2]
        gap = (w - h) // 2
        frame = frame[0:h, gap:w-gap]
        h, w = frame.shape[:2]

        if debug_flug:
            cv.imwrite("default.jpg", frame)
        
        #ホワイトバランス
        # frame = white_balance_loops(frame, h, w)
        frame = cv.remap(frame, map_x, map_y, cv.INTER_LINEAR)

        if debug_flug:
            cv.imwrite("remap0.jpg", frame)

        frame = cv.blur(frame, (5, 5))
        
        if debug_flug:
            cv.imwrite("remap1.jpg", frame)

        frame = cv.resize(frame, dsize = (w // 2, h // 2))

        
        # 赤, 黄, 青色検出
        mask_red, mask_yellow, mask_blue = color_detect(frame)
        

        if debug_flug:
            cv.imwrite("remap2.jpg", frame)

            cv.imwrite("detect_red.jpg", mask_red)
            cv.imwrite("detect_yellow.jpg", mask_yellow)
            cv.imwrite("detect_blue.jpg", mask_blue)


        # マスク画像をブロブ解析（面積最大のブロブ情報を取得）
        ball_data = analysis_blob(mask_red)
        # print(target["found"])

        # send_data(ball_data, 254)


        # 黄色ゴールのブロブ解析
        yellow_goal_data = analysis_blob(mask_yellow)

        # send_data(yellow_goal_data, 253)


        # 青色ゴールのブロブ解析
        blue_goal_data = analysis_blob(mask_blue)

        # send_data(blue_goal_data, 252)
        
        if debug_flug:
            # Enter される度にボール、ゴールの座標をターミナルに出力
            coordinate_test(ball_data, yellow_goal_data, blue_goal_data)

    
    cap.release()  #動画を閉じる
    cv.destroyAllWindows()  #ウィンドウ全削除




#本体
if __name__ == '__main__':
    uart = serial.Serial("/dev/ttyS0", 19200, timeout=1000)

    main(debug_flug = True)

    uart.close()
