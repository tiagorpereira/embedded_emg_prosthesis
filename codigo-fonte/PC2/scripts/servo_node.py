#!/usr/bin/env python3

import rospy

# Import ROS msgs
from std_msgs.msg import Float64
from std_msgs.msg import UInt8
from std_msgs.msg import String

# Import utilities
import RPi.GPIO as GPIO
import time
import rosnode

# Parameters
global servoPIN  # servomotor pin in raspberry GPIO
global minAngle  # minimun angle
global maxAngle  # maximum angle
global deg_0_pulse # pulse period for 0º
global deg_180_pulse # pulse period for 180º
global f # frequency

# Calculation parameters
global period # period
global k # constant
global deg_0_duty # 0 degree position
global pulse_range # pulse total range

# Servopin
servoPIN = 17

# Max/min Angle for open/close prothesys action
minAngle = 45
maxAngle = 150

# Ajuste estes valores para obter o intervalo completo do movimento do servo
deg_0_pulse   = 0.5 
deg_180_pulse = 2.5
f = 50.0

# Servo parameters calculation
period = 1000/f
k      = 100/period
deg_0_duty = deg_0_pulse*k
pulse_range = deg_180_pulse - deg_0_pulse
duty_range = pulse_range * k


def SetAngle(pwm, angle):
    duty = deg_0_duty + (angle/180.0) *  duty_range
    GPIO.output(servoPIN, True)
    pwm.ChangeDutyCycle(duty)
    time.sleep(1)
    GPIO.output(servoPIN, False)
    pwm.ChangeDutyCycle(0)

def control_callback(data):

    # Prothesis hand open
    if data == UInt8(1):
        SetAngle(p, float(minAngle))

    # Prothesis hand close
    elif data == UInt8(2):
        SetAngle(p, float(maxAngle))

    # Prothesis hand half open
    elif data == UInt8(3):
        SetAngle(p, float(90))



def main():
    global p

    # Init servo node
    rospy.loginfo('Inicializing node')
    rospy.init_node('servo', anonymous=False)

    # Initializing GPIO pin
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(servoPIN, GPIO.OUT)
    p = GPIO.PWM(servoPIN, 50) # GPIO 17 for PWM with 50Hz
    p.start(0) # Initialization

    # list subscribed topics
    rospy.loginfo('Setting up topics')
    sub = rospy.Subscriber('servo/action', UInt8, callback=control_callback)
    
    rospy.spin()
    p.stop()
    GPIO.cleanup()

    # # loop rate (in hz)
    # rate = rospy.Rate(5)

    # # Node Loop
    # while not rospy.is_shutdown():
    #     angle = input("Enter angle (0 a 180): ")
    #     SetAngle(p, float(angle))

    #     # Wait for the next cycle
    #     rate.sleep()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass