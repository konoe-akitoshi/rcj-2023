import cv2
import numpy as np
from main import clean, Targets


def main():
    clean("./perspective/")

    # targets = Targets("./undistorted/*.jpg")
    targets = Targets("./images/*.jpg")
    h, w = targets.size()

    original_points = np.array(
        [[0, 0], [0, h], [w, h], [w, 0]],
        dtype=np.float32
    )
    target_points = np.array(
        [[0, 0], [w/2, 2*h], [3*w/2, 2*h], [2*w, 0]],
        dtype=np.float32
    )
    matrix = cv2.getPerspectiveTransform(original_points, target_points)

    for image in targets.images:
        dst = cv2.warpPerspective(image.image, matrix, (2 * w, 2 * h))
        cv2.imwrite("./perspective/" + image.name, dst)


if __name__ == "__main__":
    main()
