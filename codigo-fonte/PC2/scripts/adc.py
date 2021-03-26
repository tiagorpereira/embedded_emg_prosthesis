#!/usr/bin/env python3
import rospy
import serial
import struct
from std_msgs.msg import Int32

# def receive(ser):
#     k=ser.read(2)
#     v=struct.unpack('H', k)
#     h=v[0] * 3
#     return h
global ser
ser = serial.Serial('/dev/serial0', 115200) #rfcomm0 #ttyUSB0
def adc():
    
    # ser = serial.Serial('/dev/ttyUSB0', 115200) #rfcomm0 #ttyUSB0
    global ser
    pub = rospy.Publisher('dados', Int32, queue_size=10)
    rospy.init_node('adc', anonymous=True)
    rate = rospy.Rate(1600)

    while not rospy.is_shutdown():
        try:
            #k=ser.read(2)
            k=ser.readline()
            #v=struct.unpack('H', k)
            #print(k)
            v=int(k)
            dados=v#v[0] * 3
            #print(dados)
            if dados > 13000:
                print("Dados Incorretos")
                rospy.signal_shutdown('Quit') 
            pub.publish(Int32(dados))
        except serial.SerialException:
            print("Erro captura")
            rospy.signal_shutdown('Quit') 
        #rate.sleep()
if __name__ == '__main__':
    try:
        adc()
    except rospy.ROSInterruptException:
        ser.close()

