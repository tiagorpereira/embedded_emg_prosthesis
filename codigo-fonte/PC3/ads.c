/*
 ============================================================================
 Name        : ads1115_example.c
 Author      : Giovanni Bauermeister
 Description : Read analog values from potentiometer using ADS1115 and prints to terminal
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <MQTTClient.h>
#include "ads1115_rpi.h"

#define TEMPO 625

#define MQTT_ADDRESS   "tcp://127.0.0.1:1883"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "ads"  

/* Substitua aqui os topicos de publish e subscribe por topicos exclusivos de sua aplicacao */
#define MQTT_PUBLISH_TOPIC     "dados"
#define MQTT_SUBSCRIBE_TOPIC   "MQTTCClientSubTopic"

MQTTClient client;
int i=0;
int k=0;
int t=0;
int ant=0;

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

void capturaDados(){
    char n[10];
    unsigned int dados;
	//printf("CH_0 = %d V \n ", readVoltage(0));
    dados = readVoltage(0);
    sprintf(n,"%d",dados);
    publish(client, MQTT_PUBLISH_TOPIC, n);
    k++;
    t=time(NULL);
    if(t-ant == 1){
        printf("Freq: %dHz\n",k);
        k=0;
    }
    ant=t;
     //ualarm(TEMPO,0);
}

int main(void) {

	int rc;
	int lei;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

	/* Inicializacao do MQTT (conexao & subscribe) */
	MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	//MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

	rc = MQTTClient_connect(client, &conn_opts);

	if (rc != MQTTCLIENT_SUCCESS)
	{
		printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
		exit(-1);
	}

	if(openI2CBus("/dev/i2c-1") == -1)
	{
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	signal(SIGALRM,capturaDados);
    ualarm(TEMPO,TEMPO);
	while(1)
	{
		//printf("CH_0 = %d V \n ", readVoltage(0));
		//printf("CH_1 = %.2f V | ", readVoltage(1));
		//printf("CH_2 = %.2f V \n", readVoltage(2));
	}

	return EXIT_SUCCESS;
}
