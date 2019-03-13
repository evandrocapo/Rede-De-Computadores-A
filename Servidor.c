/*
Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744
Pedro Andrade Caccavaro --------------------------------- RA: 16124679
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/* Servidor UDP */
void main(int argc, char *argv[])
{
	int sockint, s, namelen, client_address_size, teste, guardar;
	long lSize;
	struct sockaddr_in client, server;
	char buf_rec[200], ret[2001];
	FILE *p;
	unsigned short port;

	/* Cria um socket UDP (dgram) */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket()");
		exit(1);
	}

	port = htons(atoi(argv[1]));

	/*
	* Define a qual endere�o IP e porta o servidor estar� ligado.
	* Porta = 0 -> faz com que seja utilizada uma porta qualquer livre.
	* IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
	* os endere�os IP
	*/
	server.sin_family = PF_INET;		 /* Tipo do endere�o             */
	server.sin_port = port;				 /* Escolhe uma porta dispon�vel */
	server.sin_addr.s_addr = INADDR_ANY; /* Endere�o IP do servidor      */

	/* Liga o servidor à porta definida anteriormente */
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	/* Imprime qual porta foi utilizada. */
	printf("> Porta utilizada: %d\n", ntohs(server.sin_port));
	do
	{

		/* Recebe uma mensagem do cliente. O endereço do cliente ser� armazenado em "client" */
		client_address_size = sizeof(client);

		memset(buf_rec, 0, sizeof(buf_rec));

		if (recvfrom(s, buf_rec, sizeof(buf_rec), 0, (struct sockaddr *)&client, &client_address_size) < 0)
		{
			perror("recvfrom()");
			exit(1);
		}

		memset(ret, 0, sizeof(ret));

		p = popen(buf_rec, "r");

			guardar = fread(ret, 1, 1999, p);
			ret[guardar - 1] = '\n';
			ret[guardar] = '\0';

		if (sendto(s, ret, guardar + 1, 0, (struct sockaddr *)&client, client_address_size) < 0)
		{
			perror("sendto()");
			exit(2);
		}

		/* Imprime a mensagem recebida, o endereço IP do cliente e a porta do cliente */
		printf("> Recebido o comando %s...\n", buf_rec);

		/* Fecha o socket */
		fclose(p);

	} while (1);

	close(s);
}
