/*
* Includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <sys/time.h>
#include <MQTTClient.h>
#include <time.h>
#include <iostream>
#include <armadillo>

#include "lib/sigpack/sigpack.h"

using namespace std;
using namespace arma;
using namespace sp;

/*
* Defines
*/
#define WINDOW 800
/* Caso desejar utilizar outro broker MQTT, substitua o endereco abaixo */
#define MQTT_ADDRESS   "tcp://127.0.0.1:1883"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "test"  

/* Substitua aqui os topicos de publish e subscribe por topicos exclusivos de sua aplicacao */
#define MQTT_PUBLISH_TOPIC     "MQTTCClientPubTopic"
#define MQTT_SUBSCRIBE_TOPIC   "dados"

/*
*  Variaveis globais
*/
MQTTClient client;

//int window[WINDOW]={0};
colvec window(WINDOW,fill::zeros);
int counter=0;
/*
* Prototipos de funcao
*/



/*
* Implementacoes
*/


/* Funcao: callback de mensagens MQTT recebidas e echo para o broker
 * Parametros: contexto, ponteiro para nome do topico da mensagem recebida, tamanho do nome do topico e mensagem recebida
 * Retorno : 1: sucesso (fixo / nao ha checagem de erro neste exemplo)
*/
colvec extraction(colvec x){
    colvec parameters(8,fill::zeros);

    x=x-mean(x);
    //Valor medio absoluto
    parameters(0)=mean(abs(x));
    //Valor RMS
    parameters(1)=sqrt(sum(square(x))/WINDOW);
    //Waveform length
    parameters(2)=sum(abs(diff(x)));
    //Variancia
    parameters(3)=var(x);
    //Average Amplitude Change
    double Y = 0;
    for (int i = 0; i < WINDOW - 2; i++)
    {
        Y = Y + abs(x(i+1)-x(i));
    }
    parameters(4)=Y;
    //Energia em bandas
    double tot = norm(x,2);
    tot = tot*tot;
    vec b;           // Filter coeffs.
    b(1) = 4.72490051e-04;
    std::cout << "Filter coeffs: \n" << b.t() << std::endl;

    return parameters;
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    for (int i = 0; i < WINDOW-2; i++)
    {
        window(i)=window(i+1);
    }
    char* payload = (char*)message->payload;
    window(WINDOW-1)= atoi(payload);
    //printf("%.1f\n",window(WINDOW-1));
    if(counter == WINDOW/2){
        counter = 0;
        colvec parametros = extraction(window);
        printf("%.1f\n",parametros(3));
    }else
        counter++;
    
    

    /* Mostra a mensagem recebida */
    //printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    /* Faz echo da mensagem recebida */
    //publish(client, MQTT_PUBLISH_TOPIC, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

int main(int argc, char *argv[])
{
   int rc;
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
       /*
        * o exemplo opera por "interrupcao" no callback de recepcao de 
        * mensagens MQTT. Portanto, neste laco principal eh preciso fazer
        * nada.
        */
   }
}