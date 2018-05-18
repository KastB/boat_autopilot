import numpy as np
import cv_bridge
import cv2
from matplotlib import pyplot as plt

n = 5

im = cv2.imread('/export/home/bernd/src/boat_autopilot/data/test_images/2.jpg')


width = im.shape[1]
height = im.shape[0]
print (height)

i = 1
delta = int(height / n)
current = 0
res = im.copy()
while i <= n:
    print (current)
    sub_face = im[current:current + delta, 0:width]

    # apply a gaussian blur on this new recangle image
    sub_face = cv2.GaussianBlur(sub_face, (int(i/2) * 2 + 1, int(i/2) * 2 + 1), 30)
    # merge this blurry rectangle to our final image
    res[current:current + delta, 0:width] = sub_face
    current = current + delta
    i = i + 1
'''for (x, y, w, h) in faces:
    sub_face = image[y:y+h, x:x+w]
    # apply a gaussian blur on this new recangle image
    sub_face = cv2.GaussianBlur(sub_face,(23, 23), 30)
    # merge this blurry rectangle to our final image
    result_image[y:y+sub_face.shape[0], x:x+sub_face.shape[1]] = sub_face
'''

im = res



edges = cv2.Canny(im,100,200)

plt.subplot(121),plt.imshow(im,cmap = 'gray')
plt.title('Original Image'), plt.xticks([]), plt.yticks([])
plt.subplot(122),plt.imshow(edges,cmap = 'gray')
plt.title('Edge Image'), plt.xticks([]), plt.yticks([])

plt.show()





'''
kernel = np.ones((n,n),np.float32)/n/n
dst = cv2.filter2D(im,-1,kernel)

imgray = cv2.cvtColor(dst,cv2.COLOR_BGR2GRAY)
ret,thresh = cv2.threshold(imgray,127,255,0)
im2, contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

cv2.drawContours(im, contours, -1, (0,255,0), 3)
'''

edges = cv2.Canny(im,100,200)




cv2.imshow('image',im)
cv2.waitKey(0)
cv2.destroyAllWindows()



