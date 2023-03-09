import cv2
import numpy as np
import os
import glob
import shutil

# サークルグリットの点の数
# CIRCLEGRID = (6,9)
CIRCLEGRID = (12, 8)

# cv2.TERM_CRITERIA_EPS
#    指定された精度(epsilon)に到達したら繰り返し計算を終了する
# cv2.TERM_CRITERIA_MAX_ITER
#    指定された繰り返し回数(max_iter)に到達したら繰り返し計算を終了する
# cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER
#    上記のどちらかの条件が満たされた時に繰り返し計算を終了する
CRITERIA = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)


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


def extractPaths(targets):
    # Defining the world coordinates for 3D points
    objp = np.zeros((1, CIRCLEGRID[0]*CIRCLEGRID[1], 3), np.float32)
    objp[0, :, :2] = np.mgrid[0:CIRCLEGRID[0],
                              0:CIRCLEGRID[1]].T.reshape(-1, 2)

    # Vectors of 3D points for each CIRCLEGRID image
    objpoints = []

    # Vectors of 2D points for each CIRCLEGRID image
    imgpoints = []

    for image in targets.images:
        print(image.name)
        gray = cv2.cvtColor(image.image, cv2.COLOR_BGR2GRAY)
        # Find the chess board corners
        found, corners = cv2.findCirclesGrid(
            gray,
            CIRCLEGRID,
            cv2.CALIB_CB_SYMMETRIC_GRID
        )
        if found:
            print("found:", image.name)
            objpoints.append(objp)

            # refining pixel coordinates for given 2d points.
            corners2 = cv2.cornerSubPix(
                gray,
                corners,
                (11, 11),
                (-1, -1),
                CRITERIA
            )
            imgpoints.append(corners2)

            # Draw and display the corners
            corners3 = cv2.drawChessboardCorners(
                image.image.copy(),
                CIRCLEGRID,
                corners2,
                True
            )
            cv2.imwrite('./corners/' + image.name, corners3)
    return (objpoints, imgpoints)


if __name__ == "__main__":
    clean('./undistorted/')
    clean('./corners/')

    targets = Targets('./image/*.jpg')
    h, w = targets.size()

    objectPoints, imagePoints = extractPaths(targets)
    status, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(
        objectPoints,
        imagePoints,
        (w, h),
        None,
        None
    )

    print("Camera matrix :")
    print(mtx)
    print("dist :")
    print(dist)
    print("rvecs :")
    print(rvecs)
    print("tvecs :")
    print(tvecs)

    for image in targets.images:
        # Refining the camera matrix using parameters obtained by calibration
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(
            mtx,
            dist,
            (w, h),
            1,
            (w, h)
        )

        # Method 1 to undistort the image
        dst = cv2.undistort(image.image, mtx, dist, None, newcameramtx)

        # Method 2 to undistort the image
        mapx, mapy = cv2.initUndistortRectifyMap(
            mtx,
            dist,
            None,
            newcameramtx,
            (w, h),
            5
        )

        dst = cv2.remap(image.image, mapx, mapy, cv2.INTER_LINEAR)
        cv2.imwrite('./undistorted/' + image.name, dst)
