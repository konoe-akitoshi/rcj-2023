import cv2
import glob
import os
import shutil

images = glob.glob('./images/*.jpg')

output_dir = './image_resized/'
if os.path.exists(output_dir):
    shutil.rmtree(output_dir)
os.makedirs(output_dir)

for filename in images:
    img = cv2.imread(filename)

    height = img.shape[0]
    width = img.shape[1]
    rate = max(height, width) / 1000
    img_resize = cv2.resize(img, (int(width/rate), int(height/rate)))

    outname = output_dir + str(os.path.basename(filename))
    print(outname)
    cv2.imwrite(outname, img_resize)

