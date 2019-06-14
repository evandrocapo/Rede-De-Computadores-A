/*
EQUIPE:
Agostinho Sanches de Araújo ----------------------------- RA: 16507915	
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744	
Pedro Andrade Caccavaro --------------------------------- RA: 16124679	
*/
#include <pthread.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

pthread_t servidor_i;
pthread_mutex_t semaforo;
int ns;
int maiorTemperatura = 0; 

void INThandler(int sig){

	int x;
	if ((pthread_mutex_lock(&semaforo)) != 0){
		fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
		exit(1);
	}
	if ((x = pthread_mutex_destroy(&semaforo)) != 0){
		fprintf(stderr, "Impossivel remover semaforo\n");
		printf("%d\n", x);
		exit(1);
	}

	exit(0);
}

void *servidor()
{
	int socket_servidor = ns;
	int textoTemperatura;
	int temperaturaLocal = 0;
	int signal;

	//do{
	//close(ns);
	while(1){
		// Pegar temperatura do cliente arduino!
		if(recv(socket_servidor, &textoTemperatura, sizeof(textoTemperatura), 0) == -1){
			perror("recv() textoTemperatura");
			exit(7);
		}

		printf("Mensagem recebida: %d\n", textoTemperatura);
		
		if ((pthread_mutex_lock(&semaforo)) != 0)
		{
			fprintf(stderr, "pthread_mutex_lock() falhou");
			exit(8);
		}
		
		// Transformar textoTemperatura para float e comparar com maiorTemperatura!
		//temp = atof(textoTemperatura);
		//printf("Resultado transformado: %.2f\n", atof(textoTemperatura));

		if(textoTemperatura >= temperaturaLocal){
			// Send sinal para HIGH (mandar número 1) ----- acender led
			temperaturaLocal = textoTemperatura;
			if(temperaturaLocal >= maiorTemperatura){
				maiorTemperatura = temperaturaLocal;
				signal = 1;
				if (send(socket_servidor, &signal, sizeof(signal), 0) < 0)
				{
					perror("Send()");
					exit(7);
				} 
			}else{
				// Send sinal para LOW (mandar número 0) ------ apagar led
				signal = 0;
				if (send(socket_servidor, &signal, sizeof(signal), 0) < 0)
				{
					perror("Send()");
					exit(7);
				} 	
			}	
		}else{
			// Send sinal para LOW (mandar número 0) ------ apagar led
			signal = 0;
			if (send(socket_servidor, &signal, sizeof(signal), 0) < 0)
			{
				perror("Send()");
				exit(7);
			} 
			
			
		}
		
		if ((pthread_mutex_unlock(&semaforo)) != 0)
			{
				fprintf(stderr, "pthread_mutex_lock() falhou");
				exit(8);
			}
			
	}

	/* Receber algum sinal do cliente arduino sobre desconectar!
	} while ();*/
	close(socket_servidor);

	printf("\nServidor Thread %ld encerrado\n", pthread_self());
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
unsigned short port;
struct sockaddr_in client;
struct sockaddr_in server;
int namelen, s;

/*
* o primeiro argumento (argv[1]) � a porta
* onde o servidor aguardar� por conex�es
*/
if (argc != 2)
{
fprintf(stderr, "use: %s porta\n", argv[0]);
exit(1);
}

port = (unsigned short)atoi(argv[1]);

/*
* cria um socket tcp (stream) para aguardar conex�es
*/
if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
{
perror("socket()");
exit(2);
}

/*
* define a qual endere�o ip e porta o servidor estar� ligado.
* ip = inadddr_any -> faz com que o servidor se ligue em todos
* os endere�os ip
*/

server.sin_family = AF_INET;
server.sin_port = htons(port);
server.sin_addr.s_addr = INADDR_ANY;

/*
* liga o servidor � porta definida anteriormente.
*/
if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
{
perror("bind()");
exit(3);
}

/*
* prepara o socket para aguardar por conex�es e
* cria uma fila de conex�es pendentes.
*/
if (listen(s, 1) != 0)
{
perror("listen()");
exit(4);
}

//criacao do semaforo da regia critica
if((pthread_mutex_init(&semaforo, NULL)) != 0)
{
printf("erro: impossivel criar um mutex\n");
exit(5);
}

if ((pthread_mutex_unlock(&semaforo)) != 0)
{
fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
exit(6);
}

/*
* aceita uma conex�o e cria um novo socket atrav�s do qual
* ocorrer� a comunica��o com o cliente.
*/

namelen = sizeof(client);

signal(SIGINT, INThandler);

while (1)
{
namelen = sizeof(client);
if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1){
perror("accept()");
exit(5);
}

printf("Conexao estabelecida com o enderecoIP: %s na porta: %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

//criar thread
if(pthread_create(&servidor_i, NULL, servidor, NULL) != 0){	
perror("pthread_create()");
exit(6);
}
}

}
