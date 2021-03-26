#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int32
from std_msgs.msg import Int32MultiArray

global window
global counter
global pub
global len_window
len_window = 800
window=Int32MultiArray()
window.data = len_window * [0]
counter = 0
pub = rospy.Publisher('data_window', Int32MultiArray, queue_size=10)
def callback(data):
    global window
    global counter
    global pub  
    global len_window

    window.data.pop(0)
    window.data.append(data.data)
    
    if counter == len_window/2:
        counter = 0
        pub.publish(window)
    else:
        counter += 1


def accumulator():
    rospy.init_node('accumulator', anonymous=True)
    rospy.Subscriber('dados', Int32, callback)
    rospy.spin()

if __name__ == '__main__':
    try:
        accumulator()
    except rospy.ROSInterruptException:
        pass