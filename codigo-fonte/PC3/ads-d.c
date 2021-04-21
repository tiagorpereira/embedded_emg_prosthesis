#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#define TEMPO 625

char *socket_name;
int socket_id;
struct sockaddr name;

long int i;

void enviar(long int n){
    socket_id = socket(PF_LOCAL, SOCK_STREAM,0);
    connect(socket_id, &name, sizeof(name));
    write(socket_id, &n, sizeof(int));
    close(socket_id);
}
double t=0;
double ant=0;
void capturaDados(){
    //char n[10];
    i=(i+1)%200;
    //sprintf(n,"%d",i);
    //write(socket_id, &i, sizeof(int));
    struct timeval timev;
    gettimeofday(&timev, NULL);
    t=(double)timev.tv_sec+((double)timev.tv_usec)*0.000001;
    printf("Freq: %.2fHz\n",1/(t-ant));
    ant=t;
    //publish(client, MQTT_PUBLISH_TOPIC, n);
    ualarm(TEMPO,0);
}

int main ()
{
	
	int length;

	socket_name = "/tmp/ads";
	
	name.sa_family = AF_LOCAL;
	strcpy(name.sa_data, socket_name);
    socket_id = socket(PF_LOCAL, SOCK_STREAM,0);
    connect(socket_id, &name, sizeof(name));
    signal(SIGALRM,capturaDados);
    ualarm(TEMPO,0);
    while(1){}
	close(socket_id);
	return 0;
}