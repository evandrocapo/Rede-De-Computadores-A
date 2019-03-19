#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

/*
 * Servidor TCP
 */

struct mensagem
{
    char nome[20];
    char texto[80];
};

int main(int argc, char **argv)
{
    unsigned short port;
    char sendbuf[100];
    char recvbuf[12];
    struct sockaddr_in client;
    struct sockaddr_in server;
    int s;  /* Socket para aceitar conex�es       */
    int ns; /* Socket conectado ao cliente        */
    int namelen;

    int quant_msg = 0;
    struct mensagem msg[10];
    struct mensagem msg_only;
    int opcao;

    /*
     * O primeiro argumento (argv[1]) � a porta
     * onde o servidor aguardar� por conex�es
     */
    if (argc != 2)
    {
        fprintf(stderr, "Use: %s porta\n", argv[0]);
        exit(1);
    }

    port = (unsigned short)atoi(argv[1]);

    /*
     * Cria um socket TCP (stream) para aguardar conex�es
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket()");
        exit(2);
    }

    /*
    * Define a qual endere�o IP e porta o servidor estar� ligado.
    * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
    * os endere�os IP
    */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    /*
     * Liga o servidor � porta definida anteriormente.
     */
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind()");
        exit(3);
    }

    /*
     * Prepara o socket para aguardar por conex�es e
     * cria uma fila de conex�es pendentes.
     */
    if (listen(s, 1) != 0)
    {
        perror("Listen()");
        exit(4);
    }

    /*
     * Aceita uma conex�o e cria um novo socket atrav�s do qual
     * ocorrer� a comunica��o com o cliente.
     */
    namelen = sizeof(client);

    do
    {

        if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
        {
            perror("Accept()");
            exit(5);
        }

        printf("Conexão iniciada\n");

        do
        {

            if (recv(ns, &opcao, sizeof(opcao), 0) == -1)
            {
                perror("Recv()");
                exit(6);
            }

            switch (opcao)
            {
            case 1:
                if (recv(ns, &msg_only, sizeof(msg), 0) == -1)
                {
                    perror("Recv()");
                    exit(7);
                }
                printf("\nMensagem do Cliente: %s\n", msg_only.nome);
                printf("Mensagem recebida do cliente: %s\n", msg_only.texto);

                msg[quant_msg] = msg_only;
                // strcpy(msg[quant_msg].nome, msg_only.nome);
                // strcpy(msg[quant_msg].texto, msg_only.texto);
                quant_msg = quant_msg + 1;

                strcpy(sendbuf, "Incluso com sucesso!\n");

                if (send(ns, sendbuf, sizeof(sendbuf), 0) < 0)
                {
                    perror("Send()");
                    exit(5);
                }

                printf("%s\n", sendbuf);
                fflush(stdout);

                break;
            case 2:
                printf("Enviando as mensagens para o cliente");
                if (send(ns, &quant_msg, sizeof(quant_msg), 0) < 0)
                {
                    perror("Send()");
                    exit(5);
                }

                if (send(ns, msg, sizeof(msg), 0) < 0)
                {
                    perror("Send()");
                    exit(5);
                }
                break;
            case 3:
                printf("As mensagens foram apagadas\n");

                strcpy(sendbuf, "As mensagens foram apagadas\n");

                quant_msg = 0;

                if (send(ns, &quant_msg, sizeof(quant_msg), 0) < 0)
                {

                    perror("Send()");
                    exit(5);
                }

                if (send(ns, sendbuf, sizeof(sendbuf), 0) < 0)
                {

                    perror("Send()");
                    exit(5);
                }
                break;
            case 4:
                printf("\nA conexão está sendo encerrada!\n");
                break;
            }
        } while (opcao != 4);
        close(ns);

    } while (1);
    /* Fecha o socket aguardando por conex�es */
    close(s);

    printf("Servidor terminou com sucesso.\n");
    exit(0);
}
