import cv2
import numpy as np
import os


def main(sample_quantity = 10):

  def click_function(event, x, y, flag, param):
    h, w = 300, 300
    
    if event == cv2.EVENT_LBUTTONDOWN:
      
      for dx in range(-r, r+1):
        for dy in range(-r, r+1):
          if dx and dy:
            continue
          
          if not (0 <= x + dx < h and 0 <= y + dy < w):
            continue
          
          stat[y+dy][x+dx] = mode
          
          if mode == 1:
            img[y+dy][x+dx] = np.zeros(3)
          else:
            img[y+dy][x+dx] = origin[y+dy][x+dx]


  for sample_num in range(sample_quantity):
    img = cv2.imread(f"photo_sample/sample{sample_num}.jpg")
    origin = np.copy(img)
    
    cv2.namedWindow(f"sample{sample_num}")
    cv2.setMouseCallback(f"sample{sample_num}", click_function)

    h, w = 300, 300
    stat = [[-1] * w for _ in range(h)]
    
    r = 1 # x, y をクリックしたら、そこからマンハッタン距離が r 以下のところを設定
    mode = 1 # 1 の時クリックすると target に設定、-1 だと target を解除

    while True:
      cv2.imshow(f"sample{sample_num}", img)
      k = cv2.waitKey(1)
      if k == 27: # ESC キーで break
        break
      if k == 114: # r キーで r を 0->1 もしくは 1->0
        r ^= 1
      if k == 109: # m キーで mode 切り替え
        mode *= -1

    with open(f"target_mark/marks{sample_num}.txt", mode = "w") as f:
      for row in stat:
        f.write(" ".join([str(v) for v in row]) + "\n")

  cv2.destroyAllWindows()


if __name__ == "__main__":
  
  if not os.path.isdir("target_mark"):
    os.mkdir("target_mark")
  
  main()
