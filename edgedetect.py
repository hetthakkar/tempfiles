# -*- coding: utf-8 -*-
"""
Created on Tue Jun 04 15:04:19 2019

@author: admin1
"""

import scipy.io
from math import exp, sqrt, log
import numpy as np
from osgeo import gdal
import scipy as sci
import matplotlib.pyplot as plt
import time
import math

fr=2.0
def dist_fract(p_vec, q_vec, fraction=fr):
  diff = p_vec-q_vec
  # print('pvec shape', p_vec.shape)
  # print('qvec shape', q_vec.shape)
  # print('diff shape', diff.shape)
  diff_fraction = diff**fraction
  return math.pow(np.sum(diff_fraction), 1/fraction)


def hypedge(input_img,window_size):
  indx=input_img.shape
  eu=np.zeros((indx[0],indx[1]))
#  cos=np.zeros((indx[0],indx[1],6))
#  cor=np.zeros((indx[0],indx[1],6))
#  che=np.zeros((indx[0],indx[1],6))
#  sid=np.zeros((indx[0],indx[1],6))

  for i in range(0,indx[0]):
    for j in range(0,indx[1]):
      imin=max([i-window_size,0])
      imax=min([i+window_size,indx[0]])
      jmin=max([j-window_size,0])
      jmax=min([j+window_size,indx[1]])

      Irim=input_img[imin:(imax+1),jmin:(jmax+1),0:indx[2]+1]
      ind=Irim.shape
      Irimre=Irim.reshape(ind[0]*ind[1],ind[2],order='F')
      a=input_img[i,j,:].reshape(ind[2],order='F')
      distfract=np.zeros((ind[0]*ind[1],1))
      
      for k in range(0,ind[0]*ind[1]):
        distfract[k]=dist_fract(Irimre[k,:],a)
  ##-------------------- distance feature---------------  
  #    
      eu[i,j]=np.median(distfract[:])
      print(eu[i,j])
  return eu

# eun=hypedge(img,3)  #img should be m x n x k matrix
