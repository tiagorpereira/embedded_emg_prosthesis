#!/usr/bin/env python3
import rospy
import board
import busio
import random
from std_msgs.msg import Int32
from paho.mqtt import client as mqtt_client

broker = '127.0.0.1'
port = 1883
topic = "dados"
client_id = f'python-mqtt-{random.randint(0, 100)}'
global pub
pub = rospy.Publisher('dados', Int32, queue_size=10)
def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    #client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        dados=int(msg.payload.decode())
        pub.publish(Int32(dados))
        #print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(topic)
    client.on_message = on_message

def main():
    


    
    rospy.init_node('adc', anonymous=False)
    rate = rospy.Rate(1600)
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()
    
    while not rospy.is_shutdown():
        # dados = channel.voltage *1000
        # dados= int(dados)
        # pub.publish(Int32(dados))
        # rate.sleep()
        pass
if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
