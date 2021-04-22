// Access from ARM Running Linux - updated for RPi4
#include "gpio_dev_mem.h"
#include <sys/poll.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
//#include <sys/time.h>
#include <MQTTClient.h>
#include <time.h>

/*
* Defines
*/
/* Caso desejar utilizar outro broker MQTT, substitua o endereco abaixo */
#define MQTT_ADDRESS   "tcp://127.0.0.1:1883"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "control"  

/* Substitua aqui os topicos de publish e subscribe por topicos exclusivos de sua aplicacao */
#define MQTT_PUBLISH_TOPIC     "MQTTCClientPubTopic"
#define MQTT_SUBSCRIBE_TOPIC   "prediction"

/*
*  Variaveis globais
*/
MQTTClient client;
int t=0;
int ant=0;
/*
* Prototipos de funcao
*/
void publish(MQTTClient client, char* topic, char* payload);
int predictor_callback(void *context, char *topicName, int topicLen, MQTTClient_message *message);


// Servo pin - GPIO17
#define servo_pin 17
#define minAngle -30 // 60
#define maxAngle 60 //150
#define Nlim 30

#define deg_0_pulse 0.5
#define deg_180_pulse 2.5
#define f 50

#define period 1000/f
#define k 100/f
#define deg_0_duty deg_0_pulse*k
#define pulse_range deg_180_pulse - deg_0_pulse
#define duty_range pulse_range * k

// dutyCyclechange function - using device memory

void dutyCyclechange(int pin, int degree, int N)
{
    int t1 = (50*degree+4)/9+1500;
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

void control(int data, int N)
{
    if (data == 1)
        dutyCyclechange(servo_pin, minAngle, N);
    else if (data == -1)
        dutyCyclechange(servo_pin, maxAngle, N);
    else if (data == 3)
        dutyCyclechange(servo_pin, 0, N);
}

// Callback function
int n=0;
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    /* Mostra a mensagem recebida */
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    if (atoi(payload) == 1)
        dutyCyclechange(servo_pin, minAngle, N);
    else if (atoi(payload) == -1)
        dutyCyclechange(servo_pin, maxAngle, N);
    else if (atoi(payload) == 3)
        dutyCyclechange(servo_pin, 0, N);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char *argv[])
{

    setup_io();
    OUT_GPIO(servo_pin);
    int data = atoi(argv[1]);
    int N = Nlim;
    int rc;
    /*
    int angle = atoi(argv[1]);
    if(angle<-90)
            angle = 0;
        else if(angle>90)
            angle = 90;
    */
    // int N = atoi(argv[2]);

    // control(data, N);

    // MQTT
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

     /* Inicializacao do MQTT (conexao & subscribe) */
    MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
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