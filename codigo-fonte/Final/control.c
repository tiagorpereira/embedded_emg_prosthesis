// Access from ARM Running Linux - updated for RPi4
#include "gpio_dev_mem.h"
#include <sys/poll.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <MQTTClient.h>
#include <time.h>

#define MQTT_ADDRESS   "tcp://127.0.0.1:1883"

#define CLIENTID       "control"  

#define MQTT_PUBLISH_TOPIC     "MQTTCClientPubTopic"
#define MQTT_SUBSCRIBE_TOPIC   "prediction"

MQTTClient client;
int t=0;
int ant=0;

void publish(MQTTClient client, char* topic, char* payload);
int predictor_callback(void *context, char *topicName, int topicLen, MQTTClient_message *message);


#define servo_pin 17
#define minAngle -45 // 60
#define maxAngle 60 //150

#define Nlim 30
#define f 50

/* dutyCyclechange function - using device memory */
void dutyCyclechange(int pin, int degree, int N)
{
    int t1 = (f*degree+4)/9+1500;
    int t2 = 20000-t1;
    int i;
    for(i=0; i<N; i++)
    {
        GPIO_SET = 1<<pin;
        usleep(t1);
        GPIO_CLR = 1<<pin;
        usleep(t2);
    }
    // printf("Duty cycle = %.2f %%\n", (double)(100*t1)/(double)period);
}


int n=0;
int control_callback(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    int N = Nlim;

    if (atoi(payload) == 1)
        dutyCyclechange(servo_pin, minAngle, N);
    else if (atoi(payload) == 0)
        dutyCyclechange(servo_pin, maxAngle, N);
    else if (atoi(payload) == 3)
        dutyCyclechange(servo_pin, 0, N);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char *argv[])
{
    int rc;
    
    /* GPIO setup - device memory */
    setup_io();
    OUT_GPIO(servo_pin);

    /* MQTT */
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, control_callback, NULL);
    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        exit(-1);
    }

    MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 0);
   
    while(1)
    {

    }

}
