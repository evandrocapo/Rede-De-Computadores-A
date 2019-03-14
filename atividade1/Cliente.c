/*
Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744
Pedro Andrade Caccavaro --------------------------------- RA: 16124679
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Cliente UDP */
void main(int argc, char *argv[])
{
	int s, server_address_size, len_rec;
	unsigned short port;
	struct sockaddr_in server;
	char buf_send[200], buf_rec[2001], msg[201];

	/*
	* O primeiro argumento (argv[1]) é o endere�o IP do servidor
	* O segundo argumento (argv[2]) é a porta do servidor
	*/
	if(argc != 3)
	{
		printf("Use: %s enderecoIP porta\n",argv[0]);
		exit(1);
	}
	port = htons(atoi(argv[2]));

	/* Cria um socket UDP (dgram) */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket()");
		exit(1);
	}

	/* Define o endereço IP e a porta do servidor */
	server.sin_family      = AF_INET;            /* Tipo do endereço         */
	server.sin_port        = port;               /* Porta do servidor        */
	server.sin_addr.s_addr = inet_addr(argv[1]); /* Endereço IP do servidor  */


	do
	{
		printf("> ");
		fgets(msg, sizeof(msg), stdin);
		strtok(msg, "\n");
		memset(buf_send,0,strlen(buf_send));



		if(strcmp(msg, "exit") == 0){
			printf("Fechando\n");
			exit(1);
		}

		if(strcmp(msg, "exit") != 0){
			memcpy(buf_send, msg, strlen(msg)+1);

			/* Envia a mensagem no buffer para o servidor */
			if (sendto(s, buf_send, strlen(buf_send)+1, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
			{
				perror("sendto()");
				exit(2);
			}

			fflush(stdin);

			memset(buf_send,0,strlen(buf_send));

			server_address_size = sizeof(server);


			int len_rec = sizeof(buf_rec);

			if(recvfrom(s, buf_rec, len_rec, 0, (struct sockaddr *) &server,&server_address_size) < 0)
			{
				perror("recvfrom()");
				exit(1);
			}


			printf("%s",buf_rec);
		}
	}while(strcmp(msg, "exit") != 0);

	/* Fecha o socket */
	close(s);
}
