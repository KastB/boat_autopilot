import numpy as np
import cv2
from matplotlib import pyplot as plt
from os.path import expanduser
home = expanduser("~")


imag1 = cv2.imread(home + '/src/boat_autopilot/data/test_images/segelschiff.png')          # queryImage
imag2 = cv2.imread(home + '/src/boat_autopilot/data/test_images/2.jpg') # trainImage

img1 = cv2.cvtColor(imag1, cv2.COLOR_BGR2GRAY)
img2 = cv2.cvtColor(imag2, cv2.COLOR_BGR2GRAY)


# Initiate SIFT detector
sift = cv2.xfeatures2d.SIFT_create()

# find the keypoints and descriptors with SIFT
kp1, des1 = sift.detectAndCompute(imag1,None)
kp2, des2 = sift.detectAndCompute(imag2,None)

# create BFMatcher object
bf = cv2.BFMatcher(cv2.NORM_L2, crossCheck=True)

# Match descriptors.
matches = bf.match(des1, des2)

# Sort them in the order of their distance.
matches = sorted(matches, key = lambda x:x.distance)

# Draw first 10 matches.
img3 = cv2.drawMatches(img1,kp1,img2,kp2,matches[:10], img2, flags=2)

plt.imshow(img3),plt.show()