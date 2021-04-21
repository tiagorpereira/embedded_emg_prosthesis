/*
 * ads1115_rpi.c
 *	
 *  Created on: 14 de fev de 2019
 *      Author: giobauermeister
 * 	Modificado para embarcados
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ads1015.h"

//char *bus = "/dev/i2c-1";
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

void setConfig(){
	
	unsigned int config = 0;
	config = 	CONFIG_REG_MUX_CHAN_0		|
				CONFIG_REG_PGA_6_144V  		|
				CONFIG_REG_MODE_CONTIN 		|
				CONFIG_REG_DR_3300SPS 		|
				CONFIG_REG_CMODE_TRAD 		|
				CONFIG_REG_CPOL_ACTIV_LOW 	|
				CONFIG_REG_CLATCH_NONLATCH 	|
				CONFIG_REG_CQUE_1CONV;
	writeBuf[0] = 0x01;
	writeBuf[1] = config >> 8;
	writeBuf[2] = config && 0xFF;
	write(i2cFile, writeBuf, 3);
	usleep(135000);
	return;
}

unsigned int readAnalog(){
	unsigned char readBuf[2] = {0};
	unsigned int valor;
	writeBuf[0] = 0x00;
	write(i2cFile, writeBuf, 1);
	if(read(i2cFile, readBuf, 2) != 2) // read data and check error
	{
		printf("Erro I2C \n");
	}
	else
	{
		valor = readBuf[0] << 8 | readBuf[1];
		valor = valor * 3;
	}
	return valor;
}
/*
float readVoltage(int channel)
{
	unsigned char readBuf[2] = {0};
	unsigned int analogVal;
	float voltage;
	unsigned int config = 0;

	config = 	CONFIG_REG_MUX_CHAN_0		|
				CONFIG_REG_PGA_6_144V  		|
				CONFIG_REG_MODE_CONTIN 		|
				CONFIG_REG_DR_3300SPS 		|
				CONFIG_REG_CMODE_TRAD 		|
				CONFIG_REG_CPOL_ACTIV_LOW 	|
				CONFIG_REG_CLATCH_NONLATCH 	|
				CONFIG_REG_CQUE_1CONV;

	void configDevice(unsigned int config)
	{
		writeBuf[0] = 0x01;
		writeBuf[1] = config >> 8;
		writeBuf[2] = config && 0xFF;
		write(i2cFile, writeBuf, 3);
		usleep(25);
		
	}

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
	usleep(135000);

	writeBuf[0] = 0x00;
	write(i2cFile, writeBuf, 1);

	if(read(i2cFile, readBuf, 2) != 2) // read data and check error
	{
		printf("Error : Input/Output Error \n");
	}
	else
	{
		analogVal = readBuf[0] << 8 | readBuf[1];
		voltage = (float)analogVal*4.096/32767.0;
	}

	return voltage;
}*/


