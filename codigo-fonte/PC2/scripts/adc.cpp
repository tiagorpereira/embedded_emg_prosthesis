#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "/home/ubuntu/catkin_ws/src/embedded_emg_prosthesis/scripts/libs/ads1115_rpi.h"

int i2cFile;
//int deviceAddr = 0x48;
unsigned char writeBuf[3] = {0};

int openI2CBus(char *bus)
{
	if ((i2cFile = open(bus, O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		return -1;
	} else {
		printf("Bus open \n");
		return 1;
	}
}

int setI2CSlave(unsigned char deviceAddr)
{
	if(ioctl(i2cFile, I2C_SLAVE, deviceAddr) < 0)
	{
		printf("Failed to set I2C_SLAVE at address: 0x%x. \n", deviceAddr);
		return -1;
	} else {
		printf("I2C_SLAVE set at address: 0x%x \n", deviceAddr);
		return 1;
	}

}

void configDevice(unsigned int config)
	{
		writeBuf[0] = 0x01;
		writeBuf[1] = config >> 8;
		writeBuf[2] = config && 0xFF;
		write(i2cFile, writeBuf, 3);
		usleep(25);
	}

unsigned int readVoltage(int channel)
{
	unsigned char readBuf[2] = {0};
	unsigned int analogVal;
	//float voltage;
	unsigned int config = 0;

	config = 	CONFIG_REG_OS_SINGLE		|
				CONFIG_REG_PGA_6_144V 		|
				CONFIG_REG_MODE_CONTIN 		|
				CONFIG_REG_DR_3300SPS 			|
				CONFIG_REG_CMODE_TRAD 		|
				CONFIG_REG_CPOL_ACTIV_LOW 	|
				CONFIG_REG_CLATCH_NONLATCH 	|
				CONFIG_REG_CQUE_NONE;

	

	switch (channel) {
		case 0:
			config |= CONFIG_REG_MUX_CHAN_0;
			break;
		case 1:
			config |= CONFIG_REG_MUX_CHAN_1;
			break;
		case 2:
			config |= CONFIG_REG_MUX_CHAN_2;
			break;
		case 3:
			config |= CONFIG_REG_MUX_CHAN_3;
			break;
		default:
			printf("Give a channel between 0-3\n");
	}
	configDevice(config);
	//usleep(135000);

	writeBuf[0] = 0x00;
	write(i2cFile, writeBuf, 1);

	if(read(i2cFile, readBuf, 2) != 2) // read data and check error
	{
		printf("Error : Input/Output Error \n");
	}
	else
	{
		analogVal = readBuf[0] << 8 | readBuf[1];
		analogVal = analogVal >> 4;
		//voltage = (float)analogVal*4.096/32767.0;
	}

	return analogVal * 3;
}
/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "adc");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = n.advertise<std_msgs::Int32>("dados",1);

  ros::Rate loop_rate(1600);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
  unsigned int dados;
  while (ros::ok())
  {
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    std_msgs::Int32 msg;

    //std::stringstream ss;
    //ss << "hello world " << count;
    dados =readVoltage(0);
    msg.data = dados;

    //ROS_INFO("%s", msg.data.c_str());

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}