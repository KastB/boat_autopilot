import numpy as np
import cv2
from matplotlib import pyplot as plt
from pathlib import Path
from os.path import expanduser
home = expanduser("~")

n = 20

#im = cv2.imread(home + '/src/boat_autopilot/data/test_images/2.jpg')
im = cv2.imread(home + '/src/boat_autopilot/data/test_images/IMG_20180922_144912.jpg')
im = cv2.imread(home + '/src/boat_autopilot/data/test_images/IMG_20180922_145236.jpg')
#im = cv2.imread(home + '/src/boat_autopilot/data/test_images/IMG_20180922_145246.jpg')


width = im.shape[1]
height = im.shape[0]
print(height)

i = 1
delta = int(height / n)
current = 0
res = im.copy()
# old Test: image is filtered in horizontal bands, complete band is used
if False:
    while i <= n:
        print(current)
        sub_face = im[current:current + delta, 0:width]

        # apply a gaussian blur on this new recangle image
        sub_face = cv2.GaussianBlur(sub_face, (int(i/2) * 2 + 1, int(i/2) * 2 + 1), 30)
        # merge this blurry rectangle to our final image
        res[current:current + delta, 0:width] = sub_face
        current = current + delta
        i = i + 1
else:   # new Test: image is filtered in horizontal bands, only first line of band is used:
    while current < height:
        sub_face = im[current:current + delta, 0:width]

        # apply a gaussian blur on this new recangle image
        sub_face = cv2.GaussianBlur(sub_face, (int(i / 2) * 2 + 1, int(i / 2) * 2 + 1), 30)
        # merge this blurry rectangle to our final image
        res[current:current + 1, 0:width] = sub_face[0:1, 0:width]
        current = current + 1
        if current % delta == 0:
            i = i + 1
        print(current)
        print(i)

'''for (x, y, w, h) in faces:
    sub_face = image[y:y+h, x:x+w]
    # apply a gaussian blur on this new recangle image
    sub_face = cv2.GaussianBlur(sub_face,(23, 23), 30)
    # merge this blurry rectangle to our final image
    result_image[y:y+sub_face.shape[0], x:x+sub_face.shape[1]] = sub_face
'''

im = res
edgs = cv2.Canny(im, 100, 200)
edges = cv2.cvtColor(edgs, cv2.COLOR_GRAY2BGR)
cv2.namedWindow("edges", cv2.WINDOW_NORMAL)
res = cv2.addWeighted(im, 0.5, edges, 0.5, 0)
cv2.imshow('edges',  res)

'''

edges = cv2.Canny(im,100,200)

plt.subplot(121),plt.imshow(im,cmap = 'gray')
plt.title('Original Image'), plt.xticks([]), plt.yticks([])
plt.subplot(122),plt.imshow(edges,cmap = 'gray')
plt.title('Edge Image'), plt.xticks([]), plt.yticks([])

plt.show()
'''




'''
kernel = np.ones((n,n),np.float32)/n/n
dst = cv2.filter2D(im,-1,kernel)

imgray = cv2.cvtColor(dst,cv2.COLOR_BGR2GRAY)
ret,thresh = cv2.threshold(imgray,127,255,0)
im2, contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

cv2.drawContours(im, contours, -1, (0,255,0), 3)
'''

sift = cv2.xfeatures2d.SIFT_create()
gray= cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)
kp = sift.detect(gray, None)
img=cv2.drawKeypoints(gray,kp, im, flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

cv2.namedWindow("image", cv2.WINDOW_NORMAL)
cv2.imshow('image',im)
cv2.waitKey(0)
cv2.destroyAllWindows()



