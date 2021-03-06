/*
* Includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <MQTTClient.h>
#include <time.h>

/*
* Defines
*/
/* Caso desejar utilizar outro broker MQTT, substitua o endereco abaixo */
#define MQTT_ADDRESS   "tcp://127.0.0.1:1883"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "test"  

/* Substitua aqui os topicos de publish e subscribe por topicos exclusivos de sua aplicacao */
#define MQTT_PUBLISH_TOPIC     "prediction"
#define MQTT_SUBSCRIBE_TOPIC   "MQTTCClientSubTopic"

#define TEMPO 10000
/*
*  Variaveis globais
*/
MQTTClient client;
int i=0;
int k=0;
int t=0;
int ant=0;
/*
* Prototipos de funcao
*/
void publish(MQTTClient client, char* topic, char* payload);
//int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/*
* Implementacoes
*/

/* Funcao: publicacao de mensagens MQTT
 * Parametros: cleinte MQTT, topico MQTT and payload
 * Retorno: nenhum
*/
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

/* Funcao: callback de mensagens MQTT recebidas e echo para o broker
 * Parametros: contexto, ponteiro para nome do topico da mensagem recebida, tamanho do nome do topico e mensagem recebida
 * Retorno : 1: sucesso (fixo / nao ha checagem de erro neste exemplo)
*/

void capturaDados(){
    char n[10];
    i=1;
    sprintf(n,"%d",i);
    publish(client, MQTT_PUBLISH_TOPIC,n);
    k++;
    t=time(NULL);
/*
    if(t-ant == 1){
        printf("Freq: %dHz\n",k);
        k=0;
    }
    ant=t;*/
if(k == 100){
k=0;


   // for (i = 0; i < 100; i++);
	if(i==1)
		i=-1;
	else
		i=1;
}    
    sprintf(n,"%d",i);
    publish(client, MQTT_PUBLISH_TOPIC,n);
    //ualarm(TEMPO,0);
}

int main(int argc, char *argv[])
{
   int rc;
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

    signal(SIGALRM,capturaDados);
    ualarm(TEMPO,TEMPO);
   while(1)
   {
       
       /*
        * o exemplo opera por "interrupcao" no callback de recepcao de 
        * mensagens MQTT. Portanto, neste laco principal eh preciso fazer
        * nada.
        */
   }
}
