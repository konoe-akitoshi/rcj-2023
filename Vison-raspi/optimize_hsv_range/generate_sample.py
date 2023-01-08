import cv2 as cv
import numpy as np
import math

cap = cv.VideoCapture(0)

Height = 600
Width = 800

print('HEIGHT', end = '---')
print(cap.set(cv.CAP_PROP_FRAME_HEIGHT, Height))
print('WIDTH ', end = '---')
print(cap.set(cv.CAP_PROP_FRAME_WIDTH, Width))
print('FPS', end = '---')
print(cap.set(cv.CAP_PROP_FPS, 40), end = '---')
print(cap.get(cv.CAP_PROP_FPS))

ret, frame = cap.read()

b = 42.7
b2 = pow(b, 2)
c = 59.9
c2 = pow(c, 2)
d = 13.125
Z = 182.5
f = -3.04
f2 = pow(f, 2)
H = 200

h, w = frame.shape[:2]
gap = (w - h) // 2
frame = frame[0:h, gap:w-gap]
h, w = frame.shape[:2]

map_x = np.zeros((h, w), dtype = np.float32)
map_y = np.zeros((h, w), dtype = np.float32)

Half_height = h // 2
Half_width = w // 2
sc = 5

for i in range(h):
  for j in range(w):
      
    x = j - Half_width
    y = i - Half_height
    
    if x == 0:
      x = 0.0001
    

    rp = math.sqrt(pow(x, 2) + pow(y, 2))
    Rp = (-(b2 - c2) * H * rp) / ((b2 + c2) * f - 2 * b * c * math.sqrt(math.sqrt(pow(rp, 2) + f2)))
    X = Rp / (math.sqrt(1 + (pow(y, 2) / pow(x, 2)))) / sc
    Y = (y / x) * X

    if j > Half_width:
      map_x[i,j] = Half_width - X
      map_y[i,j] = Half_height - Y
    else:
      map_x[i,j] = Half_width + X
      map_y[i,j] = Half_height + Y

# ここより上は cam_docking と同じ


print("Ready to Generate!")

for sample_num in range(10):
  
  # サンプルを取る準備ができたら Enter
  input()

  # 最初の数フレームが調子悪いため
  for i in range(10):
  
    ret, frame = cap.read()
    
    h, w = frame.shape[:2]
    gap = (w - h) // 2
    frame = frame[0:h, gap:w-gap]
    h, w = frame.shape[:2]
    
    # remap
    frame = cv.remap(frame, map_x, map_y, cv.INTER_LINEAR)
    
    # 平均化
    frame = cv.blur(frame, (5, 5))
    
    # 縦横それぞれ半分に resize
    frame = cv.resize(frame, dsize = (w//2, h//2))
    
    cv.imwrite(f"photo_sample/sample{sample_num}.jpg", frame)

  print(f"photo_sample/saved sample{sample_num}.jpg")
