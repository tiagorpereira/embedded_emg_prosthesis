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

double energia(vec b,vec a,colvec x){
    IIR_filt<double, double, double> iir_filt;
    iir_filt.set_coeffs(b, a);
    colvec Y1 = iir_filt.filter(x);
    double y1 = norm(Y1,2);
    y1 = y1*y1;
    return y1;
}


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
    double y1;
    double tot = norm(x,2);
    tot = tot*tot;

    

    vec b = {2.44897128e-09, 3.67345692e-08, 2.57141984e-07, 1.11428193e-06,
        3.34284579e-06, 7.35426075e-06, 1.22571012e-05, 1.57591302e-05,
        1.57591302e-05, 1.22571012e-05, 7.35426075e-06, 3.34284579e-06,
        1.11428193e-06, 2.57141984e-07, 3.67345692e-08, 2.44897128e-09};

    vec a = { 1.00000000e+00, -8.94097666e+00,  3.81274598e+01, -1.02595239e+02,
         1.94421048e+02, -2.74396921e+02,  2.97560724e+02, -2.52180163e+02,
         1.68238453e+02, -8.82785479e+01,  3.61095963e+01, -1.12999558e+01,
         2.61723273e+00, -4.23343031e-01,  4.27414269e-02, -2.02960157e-03};
    y1=energia(b,a,x);
    parameters(5)=y1/tot*100;
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
        printf("%.1f\n",parametros(5));
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