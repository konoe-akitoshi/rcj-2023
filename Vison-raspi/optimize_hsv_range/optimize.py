import cv2
import numpy as np

import random
import time
import os


# たくさん出てくるので
def rand255():
  return random.randint(0, 255)


# range を random に生成
def generate_random():

  low = np.array([random.randint(-90, 179), rand255(), rand255()])
  high = np.array([low[0]+random.randint(1, 40), rand255(), rand255()])

  return np.array([low, high])


# range をちゃんとやる
def correct_range(hsv_range):

  h_low = hsv_range[0][0] % 180
  h_high = hsv_range[1][0] % 180

  if (h_high - h_low) % 180 > 90: # h の range が 90 以下であってほしい
    h_low, h_high = h_high, h_low
  
  if h_low > h_high:
    h_low -= 180

  hsv_range[0][0] = h_low
  hsv_range[1][0] = h_high

  for i in range(1, 3):

    # 長くなるので一時的に tmp でもつ
    tmp0 = hsv_range[0][i]
    tmp1 = hsv_range[1][i]
    tmp0, tmp1 = min(tmp0, tmp1), max(tmp0, tmp1)

    if tmp0 == tmp1: # これを許すとスコアが 0 に収束する可能性がある
      if tmp1 < 256:
        tmp1 += 1
      else:
        tmp0 -= 1

    hsv_range[0][i], hsv_range[1][i] = max(0, min(255, tmp0)), min(256, max(1, tmp1))
  
  return hsv_range


def color_detect(img, hsv_range):
  hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
  
  hsv_min, hsv_max = hsv_range
  mask = cv2.inRange(hsv, hsv_min, hsv_max)

  return mask


def calc_score(hsv_range, sample_quantity, h = 300, w = 300):
  res = 0
  
  for sample_num in range(sample_quantity):
    img = cv2.imread(f"photo_sample/sample{sample_num}.jpg")
    mask = color_detect(img, hsv_range)
    
    with open(f"target_mark/marks{sample_num}.txt", mode = "r") as f:
      pxl_mark = f.readlines()
      pxl_mark = [list(map(int, row.split())) for row in pxl_mark]
    
    for i in range(h):
      for j in range(w):
        if mask[i][j]:
          res += max(-1, pxl_mark[i][j]*10) # ここの重みは適宜調整する必要がありそう
  
  return res


def main(sample_quantity = 10):
  h, w = 300, 300
  
  cnd_ranges = [generate_random() for _ in range(25)]
  
  start_time = time.time()
  
  while time.time() - start_time < 50:
    nxt_cnd = [generate_random() for _ in range(int(10-time.time()))]
    
    cnd_ranges.sort(key = lambda x: calc_score(x, sample_quantity, h, w), reverse = True)
    
    par = cnd_ranges[:5]
    
    for cnd_par in par:
      nxt_cnd.append(cnd_par)
    
    rem = 25- len(nxt_cnd)
    
    for _ in range(rem):
      typ = random.randint(0, 1)
      
      if typ == 0: # 優秀な親を少し変えた子を生成
        chd = np.copy(random.choice(par))
        for i in range(2):
          for j in range(3):
            chd[i][j] += random.randint(-3, 3)
      
      else: # 優秀な親を 3 つ選び、掛け合わせる
        chd = np.zeros((2, 3))
        for j in range(3):
          tmp = random.choice(par)
          for i in range(2):
            chd[i][j] = tmp[i][j]
      
      chd = correct_range(chd)
      nxt_cnd.append(chd)
    
    cnd_ranges = [val for val in nxt_cnd]
  
  res = par[0]
  
  return res


if __name__ == '__main__':
  
  if not (os.path.isdir("optimized_blob")):
    os.mkdir("optimized_blob")
  
  res = main(6)
  print(calc_score(res, 6))
  
  for sample_num in range(6):
    
    img = cv2.imread(f"photo_sample/sample{sample_num}.jpg")
    
    mask = color_detect(img, res)
    
    cv2.imwrite(f"optimized_blob/result{sample_num}.jpg", mask)
  
  
  hand = np.array([[-10, 85, 153], [10, 153, 255]])
  
  print(calc_score(hand, 6))
  
  with open("hsv_range.txt", mode = "w") as f:
    for row in res:
      f.write(", ".join([str(v) for v in row]) + "\n")
