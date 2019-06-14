from __future__ import print_function
import os,sys
from math import exp, sqrt, log
import numpy as np
#from tqdm import tqdm

from osgeo import gdal

import time

################################### Load image ##############################
cube = gdal.Open('op.tif')
Xdim = cube.RasterXSize
Ydim = cube.RasterYSize
n_bands = cube.RasterCount

print('Reading image with ', n_bands, ' bands')


noisy_img = np.zeros(shape = (Ydim, Xdim, n_bands), dtype = 'float64')

for i in range(n_bands):
	noisy_img[:,:,i] = cube.GetRasterBand(i+1).ReadAsArray(0, 0, Xdim, Ydim)

print(noisy_img[:,:,0])

#op = edgedetect.hypedge(noisy_img, 5)


#np.save('op', op)
