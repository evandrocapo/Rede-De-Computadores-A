#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * Servidor UDP
 */
main(int argc, char **argv)
{
    int sockint, s, namelen, client_address_size, menu;
    struct sockaddr_in client, server;
    char buf[201];
    unsigned short port;
    char texto[2001];

    if (argc != 2)
    {
        printf("Informe a porta\n");
        exit(1);
    }

    port = htons(atoi(argv[1]));

    /*
    * Cria um socket UDP (dgram). 
    */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket()");
        exit(1);
    }

    /*
    * Define a qual endere�o IP e porta o servidor estar� ligado.
    * Porta = 0 -> faz com que seja utilizada uma porta qualquer livre.
    * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
    * os endere�os IP
    */
    server.sin_family = AF_INET;         /* Tipo do endere�o             */
    server.sin_port = port;              /* Escolhe uma porta dispon�vel */
    server.sin_addr.s_addr = INADDR_ANY; /* Endere�o IP do servidor      */

    /*
    * Liga o servidor � porta definida anteriormente.
    */
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    /* Consulta qual porta foi utilizada. */
    namelen = sizeof(server);
    if (getsockname(s, (struct sockaddr *)&server, &namelen) < 0)
    {
        perror("getsockname()");
        exit(1);
    }

    /* Imprime qual porta foi utilizada. */
    printf("Porta utilizada � %d\n", ntohs(server.sin_port));

    do
    {
        /*
    * Recebe uma mensagem do cliente.
    * O endere�o do cliente ser� armazenado em "client".
    */
        client_address_size = sizeof(client);
        if (recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client,
                     &client_address_size) < 0)
        {
            perror("recvfrom()");
            exit(1);
        }

        /*
    * Imprime a mensagem recebida, o endere�o IP do cliente
    * e a porta do cliente 
    */
        //printf("Recebida a mensagem %s do endere�o IP %s da porta %d\n",buf,inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        menu = 0;

        if (strcmp(buf, "bom-dia") == 0)
        {
            menu = 1;
        }
        else
        {
            if (strcmp(buf, "boa-tarde") == 0)
            {
                menu = 2;
            }
        }

        switch (menu)
        {
        case 1:
            printf("Enviado a mensagem: 'Servidor: Tenha um bom dia user' ao usuario\n");
            strcpy(texto, "Servidor: Tenha um bom dia user\0");
            if (sendto(s, texto, (strlen(texto) + 1), 0, (struct sockaddr *)&client, sizeof(client)) < 0)
            {
                perror("sendto()");
                exit(2);
            }
            break;
        case 2:
            printf("Enviado a mensagem: 'Servidor: Tenha uma boa tarde user' ao usuario\n");
            strcpy(texto, "Servidor: Tenha uma boa tarde user\0");
            if (sendto(s, texto, (strlen(texto) + 1), 0, (struct sockaddr *)&client, sizeof(client)) < 0)
            {
                perror("sendto()");
                exit(2);
            }
            break;
        default:
            printf("Enviado a mensagem: 'Servidor: Opcao invalida\n");
            strcpy(texto, "Servidor: Opcao invalida\0");
            if (sendto(s, texto, (strlen(texto) + 1), 0, (struct sockaddr *)&client, sizeof(client)) < 0)
            {
                perror("sendto()");
                exit(2);
            }
            break;
        }

    } while (1);
    /*
    * Fecha o socket.
    */
    close(s);
}
