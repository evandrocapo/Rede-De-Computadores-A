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

/*
EQUIPE:
Agostinho Sanches de Araújo ----------------------------- RA: 16507915    
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744    
Pedro Andrade Caccavaro --------------------------------- RA: 16124679    
*/

pthread_t servidor_i;
int ns; 


void INThandler(int sig){

	int x;

	exit(0);
}

void *servidor()
{
	int socket_servidor = ns;
	int valorUmidade, valorTemperatura, valorLuminosidade;
	int sinalUmidade, sinalTemperatura, sinalLuminosidade;
	int count = 1;
	
	while(1){
		printf("Leitura %d\n", count);
		valorLuminosidade = 0;
		valorTemperatura = 0;
		valorUmidade = 0;
		// Pegar dados sobre temperatura do cliente arduino!
		if(recv(socket_servidor, &valorTemperatura, sizeof(valorTemperatura), 0) == -1){
			perror("Recv() valorTemperatura");
			exit(8);
		}

		printf("|->Temperatura Captada: %d\n", valorTemperatura);

		// Comparações temperatura (DEFINIR VALORES: CRÍTICOS, SUPORTÁVEIS, IDEIAIS)
		if(valorTemperatura >= 21 && valorTemperatura <= 28){
			
			// Situação ideal de temperatura
			sinalTemperatura = 0;
			
		} else if((valorTemperatura >= 10 && valorTemperatura <= 20) || (valorTemperatura >= 29 && valorTemperatura <= 34)){
			
			// Situação suportável de temperatura
			sinalTemperatura = 1;
			
		} else {
			
			// Situação crítica de temperatura
			sinalTemperatura = 2;
		}
		
		if (send(socket_servidor, &sinalTemperatura, sizeof(sinalTemperatura), 0) < 0)
		{
			perror("Send() Temperatura");
			exit(11);
		}
		
		// Pegar dados sobre luminosidade do cliente arduino!
		if(recv(socket_servidor, &valorLuminosidade, sizeof(valorLuminosidade), 0) == -1){
			perror("Recv() valorLuminosidade");
			exit(9);
		}
		
		valorLuminosidade = valorLuminosidade * 10; 
		printf("|->Luminosidade Captada: %d\n", valorLuminosidade);

		// Comparações luminosidade (DEFINIR VALORES: CRÍTICOS, SUPORTÁVEIS, IDEIAIS) 	
		if(valorLuminosidade >= 0 && valorLuminosidade <= 300){
			
			// Situação ideal de luminosidade
			sinalLuminosidade = 0;
			
		} else if(valorLuminosidade > 300  && valorLuminosidade <= 500){
			
			// Situação suportável de luminosidade
			sinalLuminosidade = 1;
			
		} else {
			
			// Situação crítica de luminosidade
			sinalLuminosidade = 2;
		}
		
		if (send(socket_servidor, &sinalLuminosidade, sizeof(sinalLuminosidade), 0) < 0)
		{
			perror("Send() Luminosidade");
			exit(12);
		}

		// Pegar dados sobre umidade do cliente arduino!
		if(recv(socket_servidor, &valorUmidade, sizeof(valorUmidade), 0) == -1){
			perror("Recv() valorUmidade");
			exit(7);
		}

		valorUmidade = valorUmidade * 100;
		printf("|->Umidade Captada: %d\n", valorUmidade);
		
		// Comparações (DEFINIR VALORES: CRÍTICOS, SUPORTÁVEIS, IDEIAIS)
		if(valorUmidade > 0 && valorUmidade < 500 ){
			
			// Situação ideal de umidade
			sinalUmidade = 0;
			
		} else {
			
			// Situação crítica de umidade!!
			sinalUmidade = 1;
		}

		printf("Sinal umidade: %d\n", sinalUmidade);
		
		if (send(socket_servidor, &sinalUmidade, sizeof(sinalUmidade), 0) < 0)
		{
			perror("Send() Sinal Umidade");
			exit(10);
		}
		 count++;
		printf("\n\n");
	}
	
	// Analisar fechamento da comunicação
	close(socket_servidor);

	printf("\nServidor Thread %ld encerrado\n", pthread_self());
	pthread_exit(NULL);
}

int main(int argc, char **argv){
	
	unsigned short port;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int namelen, s;

	/*
	* o primeiro argumento (argv[1]) eh a porta
	* onde o servidor aguardara por conexoes
	*/
	
	if (argc != 2){
		fprintf(stderr, "use: %s porta\n", argv[0]);
		exit(1);
	}

	port = (unsigned short)atoi(argv[1]);

	/*
	* cria um socket tcp (stream) para aguardar conexoes
	*/
	
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket()");
		exit(2);
	}

	/*
	* define a qual endereco ip e porta o servidor estara ligado.
	* ip = inadddr_any -> faz com que o servidor se ligue em todos
	* os enderecos ip
	*/

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	/*
	* liga o servidor a porta definida anteriormente.
	*/
	
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind()");
		exit(3);
	}

	/*
	* prepara o socket para aguardar por conexoes e
	* cria uma fila de conexoes pendentes.
	*/
	
	if (listen(s, 1) != 0){
		perror("listen()");
		exit(4);
	}

	/*
	* aceita uma conexao e cria um novo socket atrav�s do qual
	* ocorrera a comunicacao com o cliente.
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
