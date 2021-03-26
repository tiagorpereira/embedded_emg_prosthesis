#!/usr/bin/env python3
import rospy
import board
import busio
import adafruit_ads1x15.ads1015 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
from adafruit_ads1x15.ads1x15 import Mode
from std_msgs.msg import Int32



def main():
    

    i2c = busio.I2C(board.SCL, board.SDA, frequency=1000000)
    ads = ADS.ADS1015(i2c)
    ads.mode = Mode.CONTINUOUS
    ads.data_rate = 3300
    ads.gain = 2 / 3
    channel = AnalogIn(ads, ADS.P0)

    pub = rospy.Publisher('dados', Int32, queue_size=10)
    rospy.init_node('adc', anonymous=False)
    rate = rospy.Rate(2000)
    
    
    while not rospy.is_shutdown():
        dados = channel.voltage *1000
        dados= int(dados)
        pub.publish(Int32(dados))
        rate.sleep()
if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
