"""
Description

@author: Bernd Kast
@copyright: Copyright (c) 2018, Siemens AG
@note:  All rights reserved.
"""

import numpy as np
import cv2
from matplotlib import pyplot as plt

from os.path import expanduser
home = expanduser("~")

img1 = cv2.imread(home + '/src/boat_autopilot/data/test_images/IMG_20180922_144912.jpg', 0)
img2 = cv2.imread(home + '/src/boat_autopilot/data/test_images/IMG_20180922_145236.jpg', 0)

# Initiate SIFT detector
orb = cv2.ORB_create()

# find the keypoints and descriptors with SIFT
kp1, des1 = orb.detectAndCompute(img1, None)
kp2, des2 = orb.detectAndCompute(img2, None)

# create BFMatcher object
bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)

# Match descriptors.
matches = bf.match(des1,des2)

# Sort them in the order of their distance.
matches = sorted(matches, key = lambda x:x.distance)
print(len(matches))
# Draw first 10 matches.
img3 = img1.copy()
cv2.drawMatches(img1,kp1,img2,kp2,matches[:10], img3, flags=2)

plt.imshow(img3),plt.show()