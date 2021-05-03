#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/time.h>

char socket_name[100];
int  socket_id;

double t=0;
double ant=0;

void print_client_message(int client_socket)
{
	
	int length,n;
	while(1){
		read(client_socket, &n, sizeof(int));
		struct timeval timev;
		gettimeofday(&timev, NULL);
		t=(double)timev.tv_sec+((double)timev.tv_usec)*0.000001;
		//printf("Freq: %.2fHz\n",1/(t-ant));
		fprintf(stderr,"Mensagem = %d\n", n);
	}
	//fprintf(stderr,"Mensagem = %d\n", n);
	//end_server_ok = !(n < 10);
	return ;
}

void end_server(int signum)
{
	fprintf(stderr,"Fechando servidor local\n");
	unlink(socket_name);
	close(socket_id);
	exit(0);
}


int main ()
{
	struct sockaddr socket_struct;
	signal(SIGINT, end_server);
	strcpy(socket_name, "/tmp/ads");
	socket_id = socket(PF_LOCAL, SOCK_STREAM, 0);
	socket_struct.sa_family = AF_LOCAL;
	strcpy(socket_struct.sa_data, socket_name);
	bind(socket_id, &socket_struct, sizeof(socket_struct));
	listen(socket_id, 10);
	//while(1)
	//{
	struct sockaddr cliente;
	int socket_id_cliente, end_server_ok;
	socklen_t cliente_len;
	socket_id_cliente = accept(socket_id, &cliente, &cliente_len);
	print_client_message(socket_id_cliente);
	close(socket_id_cliente);
		
	//}
	sleep(1);
	return 0;
}