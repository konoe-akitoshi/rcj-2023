import cv2
import numpy as np
import os
import glob
import shutil

# NOTE: if you use `deque` in MicroPython, you must use `ucollections.deque`
from collections import deque

# サークルグリットの点の数
CIRCLEGRID = (12, 8)

# cv2.TERM_CRITERIA_EPS
#    指定された精度(epsilon)に到達したら繰り返し計算を終了する
# cv2.TERM_CRITERIA_MAX_ITER
#    指定された繰り返し回数(max_iter)に到達したら繰り返し計算を終了する
# cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER
#    上記のどちらかの条件が満たされた時に繰り返し計算を終了する
CRITERIA = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)


def main():
    clean("./undistorted/")
    clean("./corners/")
    clean("./undistorted_hand/")
    clean("./undistorted_reverse/")

    targets = Targets("./images/*.jpg")
    h, w = targets.size()
    objectPoints, imagePoints = extractPaths(targets)
    status, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(
        objectPoints,
        imagePoints,
        (w, h),
        None,
        None
    )

    # キャリブレーションによって得られたパラメータを使用してカメラ行列を改良する
    newcameramtx, roi = cv2.getOptimalNewCameraMatrix(
        mtx,     # カメラ行列
        dist,    # 歪み係数のベクトル
        (w, h),  # 元の画像のサイズ
        1,       # 歪み修正後の画像にできるの隙間をどうするか (0 ~ 1)
                 #   0: zoomする、1: 黒で塗りつぶす
        (w, h)   # 歪み修正後の画像サイズ (省略すると第3引数と同じ値になる)
    )

    print("Camera matrix:")
    print(mtx)
    print("New camera matrix:")
    print(newcameramtx)
    print("dist:")
    print(dist)
    print("rvecs:")
    print(rvecs)
    print("tvecs:")
    print(tvecs)

    mapx, mapy = cv2.initUndistortRectifyMap(
        mtx,           # カメラ行列
        dist,          # 歪み係数のベクトル
        None,          # 空間の回転行列(3*3)
        newcameramtx,  # 補正後の画像のスケーリングや移動を行えるようの別のカメラ行列
                       #   省略すると、第2引数と同じ値になる
        (w, h),        # 歪み補正後の画像サイズ
        5              # = CV_32FC1
    )
    hand_mapx, hand_mapy = create_remap(h, w, mtx, newcameramtx, dist)
    next_x, next_y = create_reverse_map(h, w, mtx, newcameramtx, dist)

    for image in targets.images:
        dst = cv2.remap(image.image, mapx, mapy, cv2.INTER_LINEAR)
        cv2.imwrite("./undistorted/" + image.name, dst)

        dst = cv2.remap(image.image, hand_mapx, hand_mapy, cv2.INTER_LINEAR)
        cv2.imwrite("./undistorted_hand/" + image.name, dst)

        dst = np.zeros((h, w, 3))
        dst += [255, 0, 0][::-1]  # fill red
        for i in range(h):
            for j in range(w):
                x, y = next_x[i][j], next_y[i][j]
                dst[x][y] = image.image[i, j]
        cv2.imwrite("./undistorted_reverse/" + image.name, dst)


class ImageData:
    def __init__(self, filename):
        self.name = str(os.path.basename(filename))
        self.image = cv2.imread(filename)
        self.h, self.w = self.image.shape[:2]


class Targets:
    def __init__(self, path):
        self.filenames = sorted(glob.glob(path))
        if len(self.filenames) == 0:
            print("Not found images")
            exit(1)
        self.images = []
        for fname in self.filenames:
            self.images.append(ImageData(fname))

    def size(self):
        """
        Returns: (h, w)
        """
        ret_h = self.images[0].h
        ret_w = self.images[0].w
        for img in self.images:
            if ret_h != img.h or ret_w != img.w:
                print("image size is not same")
                exit(1)
        return (ret_h, ret_w)


def clean(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)


def create_remap(h, w, mtx, newcameramtx, dist):
    cx, cy = mtx[0][2], mtx[1][2]
    fx, fy = mtx[0][0], mtx[1][1]
    cx2, cy2 = newcameramtx[0][2], newcameramtx[1][2]
    fx2, fy2 = newcameramtx[0][0], newcameramtx[1][1]
    k1, k2, p1, p2, k3 = dist[0]

    mapx = np.zeros((w, h), dtype=np.float32)
    mapy = np.zeros((w, h), dtype=np.float32)
    for u in range(w):
        for v in range(h):
            x = (u - cx2) / fx2
            y = (v - cy2) / fy2
            rr = x ** 2 + y ** 2
            K = 1 + k1*rr + k2*(rr**2) + k3*(rr**3)

            x2 = x*K + 2*p1*x*y + p2*(rr+2*(x**2))
            y2 = y*K + p1*(rr+2*(y**2)) + 2*p2*x*y
            mapx[u][v] = x2*fx + cx
            mapy[u][v] = y2*fy + cy
    return (mapx, mapy)


def create_reverse_map(height, width, mtx, newcameramtx, dist):
    cx, cy = mtx[0][2], mtx[1][2]
    fx, fy = mtx[0][0], mtx[1][1]
    cx2, cy2 = newcameramtx[0][2], newcameramtx[1][2]
    fx2, fy2 = newcameramtx[0][0], newcameramtx[1][1]
    k1, k2, p1, p2, k3 = dist[0]

    next_index_x = [[None] * width for _ in range(height)]
    next_index_y = [[None] * width for _ in range(height)]
    for u in range(width):
        for v in range(height):
            x = (u - cx2) / fx2
            y = (v - cy2) / fy2
            rr = x ** 2 + y ** 2
            K = 1 + k1*rr + k2*(rr**2) + k3*(rr**3)

            x2 = x*K + 2*p1*x*y + p2*(rr+2*(x**2))
            y2 = y*K + p1*(rr+2*(y**2)) + 2*p2*x*y
            org_x = round(x2*fx + cx)
            org_y = round(y2*fy + cy)

            if 0 <= org_x < width and 0 <= org_y < height:
                next_index_x[org_y][org_x] = v
                next_index_y[org_y][org_x] = u

    # NOTE: Fill array by BFS
    def fill_next_index(next_index, h, w):
        for i in range(h):
            for j in range(w):
                if next_index[i][j] is None:
                    deq = deque([(i, j)])
                    while deq:
                        x, y = deq.popleft()
                        if next_index[x][y] is not None:
                            next_index[i][j] = next_index[x][y]
                            break
                        if 0 < x:
                            deq.append((x-1, y))
                        if x < h - 1:
                            deq.append((x+1, y))
                        if 0 < y:
                            deq.append((x, y-1))
                        if y < w - 1:
                            deq.append((x, y+1))
    fill_next_index(next_index_x, height, width)
    fill_next_index(next_index_y, height, width)
    return (next_index_x, next_index_y)


def extractPaths(targets):
    # ワールド座標系におけるキャリブレーションボードの各点の座標
    objp = np.zeros((1, CIRCLEGRID[0]*CIRCLEGRID[1], 3), np.float32)
    objp[0, :, :2] = np.mgrid[0:CIRCLEGRID[0],
                              0:CIRCLEGRID[1]].T.reshape(-1, 2)

    # キャリブレーションで利用する座標
    objpoints = []  # Vectors of 3D points for each CIRCLEGRID image
    imgpoints = []  # Vectors of 2D points for each CIRCLEGRID image

    for image in targets.images:
        print(image.name)

        # グレースケールを使って、キャリブレーションボード内の点の座標を取得
        gray = cv2.cvtColor(image.image, cv2.COLOR_BGR2GRAY)
        found, corners = cv2.findCirclesGrid(
            gray,
            CIRCLEGRID,
            cv2.CALIB_CB_SYMMETRIC_GRID
        )

        if found:
            print("found:", image.name)
            objpoints.append(objp)

            # 座標の精度を上げる
            corners2 = cv2.cornerSubPix(
                gray,
                corners,
                (11, 11),
                (-1, -1),
                CRITERIA
            )
            imgpoints.append(corners2)

            # コーナーが描かれた画像を生成
            corners_img = cv2.drawChessboardCorners(
                image.image.copy(),
                CIRCLEGRID,
                corners2,
                True
            )
            cv2.imwrite("./corners/" + image.name, corners_img)
    return (objpoints, imgpoints)


if __name__ == "__main__":
    main()
