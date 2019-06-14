from __future__ import print_function
import os,sys
from math import exp, sqrt, log
import numpy as np
import matplotlib.pyplot as plt
#from tqdm import tqdm
import edgedetect
from osgeo import gdal

import time

################################### Load image ##############################
cube = gdal.Open('abcd1.tif')
Xdim = cube.RasterXSize
Ydim = cube.RasterYSize
n_bands = cube.RasterCount

print('Reading image with ', n_bands, ' bands')


noisy_img = np.zeros(shape = (Ydim, Xdim, n_bands), dtype = 'float64')

for i in range(n_bands):
	noisy_img[:,:,i] = cube.GetRasterBand(i+1).ReadAsArray(0, 0, Xdim, Ydim)

plt.imshow(noisy_img[:,:,0])

#op = edgedetect.hypedge(noisy_img, 5)

plt.show()
#np.save('op', op)
