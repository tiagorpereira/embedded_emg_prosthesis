#!/usr/bin/env python3

import rospy
import numpy as np
from statsmodels.tsa.ar_model import AR
from scipy.signal import *
from std_msgs.msg import Float64MultiArray
from std_msgs.msg import Int32MultiArray


global pub
global parameters

parameters=Float64MultiArray()
parameters.data = 4 * [0]
counter = 0
pub = rospy.Publisher('extracted', Float64MultiArray, queue_size=10)
def callback(data):
    global pub  
    global parameters
    parameters.data = 8 * [0]
    x=data.data - np.mean(data.data)
    N = len(x)
    # Valor medio absoluto
    parameters.data[0] = np.mean(np.abs(x))
    # Valor RMS
    parameters.data[1] = np.sqrt(np.sum(x**2)/len(x))
    # Waveform length
    parameters.data[2] = np.sum(np.abs(np.diff(x)))
    # Variancia
    parameters.data[3] = np.var(x)
    # Average Amplitude Change
    Y=0;
    X=np.array(x);
    for i in range(N-1):
        Y=Y+np.abs(X[i+1]-X[i])
    parameters.data[4]=Y/N;
    #Energia em bandas
    tot = np.linalg.norm(X)**2
    #print tot
    num=[2.44897128e-09, 3.67345692e-08, 2.57141984e-07, 1.11428193e-06,
        3.34284579e-06, 7.35426075e-06, 1.22571012e-05, 1.57591302e-05,
        1.57591302e-05, 1.22571012e-05, 7.35426075e-06, 3.34284579e-06,
        1.11428193e-06, 2.57141984e-07, 3.67345692e-08, 2.44897128e-09]
    den=[ 1.00000000e+00, -8.94097666e+00,  3.81274598e+01, -1.02595239e+02,
         1.94421048e+02, -2.74396921e+02,  2.97560724e+02, -2.52180163e+02,
         1.68238453e+02, -8.82785479e+01,  3.61095963e+01, -1.12999558e+01,
         2.61723273e+00, -4.23343031e-01,  4.27414269e-02, -2.02960157e-03]
    zi = lfilter_zi(num, den)
    y, _  = lfilter(num,den,X,zi=zi*X[0])
    y1 =np.linalg.norm(y)**2
    parameters.data[5]=y1/tot*100

    num = [ 1.31247236e-05,  0.00000000e+00, -1.18122513e-04,  0.00000000e+00,
         4.72490051e-04,  0.00000000e+00, -1.10247679e-03,  0.00000000e+00,
         1.65371518e-03,  0.00000000e+00, -1.65371518e-03,  0.00000000e+00,
         1.10247679e-03,  0.00000000e+00, -4.72490051e-04,  0.00000000e+00,
         1.18122513e-04,  0.00000000e+00, -1.31247236e-05]

    den = [ 1.00000000e+00, -9.68593127e+00,  4.69901672e+01, -1.50739975e+02,
         3.56909439e+02, -6.60502621e+02,  9.88073702e+02, -1.21982331e+03,
         1.25851221e+03, -1.09246913e+03,  7.99555480e+02, -4.92238937e+02,
         2.53142229e+02, -1.07374086e+02,  3.67952917e+01, -9.85240606e+00,
         1.94788749e+00, -2.55100758e-01,  1.68137718e-02]

    zi = lfilter_zi(num, den)
    y, _  = lfilter(num,den,X,zi=zi*X[0])
    y1 =np.linalg.norm(y)**2
    parameters.data[6]=y1/tot*100

    num = [ 9.17785774e-09,  0.00000000e+00, -1.28490008e-07,  0.00000000e+00,
         8.35185054e-07,  0.00000000e+00, -3.34074022e-06,  0.00000000e+00,
         9.18703559e-06,  0.00000000e+00, -1.83740712e-05,  0.00000000e+00,
         2.75611068e-05,  0.00000000e+00, -3.14984077e-05,  0.00000000e+00,
         2.75611068e-05,  0.00000000e+00, -1.83740712e-05,  0.00000000e+00,
         9.18703559e-06,  0.00000000e+00, -3.34074022e-06,  0.00000000e+00,
         8.35185054e-07,  0.00000000e+00, -1.28490008e-07,  0.00000000e+00,
         9.17785774e-09]

    den = [ 1.00000000e+00, -3.75618537e+00,  1.49160032e+01, -3.64590663e+01,
         8.62006102e+01, -1.58929766e+02,  2.79121488e+02, -4.14409137e+02,
         5.84695352e+02, -7.23468409e+02,  8.51082988e+02, -8.94437527e+02,
         8.94426960e+02, -8.06151007e+02,  6.91695461e+02, -5.36216565e+02,
         3.95715767e+02, -2.63000210e+02,  1.66317800e+02, -9.38008369e+01,
         5.03045394e+01, -2.35910290e+01,  1.05214957e+01, -3.94854217e+00,
         1.41484447e+00, -3.92253746e-01,  1.05899399e-01, -1.73361510e-02,
         3.06405453e-03]

    zi = lfilter_zi(num, den)
    y, _  = lfilter(num,den,X,zi=zi*X[0])
    y1 =np.linalg.norm(y)**2
    parameters.data[7]=y1/tot*100

    # Autorregressive Model
    # model = AR(np.array(x)/1.)
    # model_fit = model.fit()
    # parameters.data = parameters.data + model_fit.params.tolist()
    #print len(parameters.data)
    pub.publish(parameters)




def extraction():
    rospy.init_node('extraction', anonymous=True)
    rospy.Subscriber('data_window', Int32MultiArray, callback)
    rospy.spin()

if __name__ == '__main__':
    try:
        extraction()
    except rospy.ROSInterruptException:
        pass