#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio_ext.h>

struct mensagem
{
    char nome[20];
    char texto[80];
    int ativo;
};

/*
 * Cliente TCP
 */
void main(int argc, char **argv)
{
    unsigned short port;
    struct hostent *hostnm;
    struct sockaddr_in server;
    int s, i;
    char nome[20],recvbuf[100];

    struct mensagem msg;
    struct mensagem msg_total[10];
    int quant_msg, opcao;

    /*
     * O primeiro argumento (argv[1]) � o hostname do servidor.
     * O segundo argumento (argv[2]) � a porta do servidor.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
        exit(1);
    }

    /*
     * Obtendo o endere�o IP do servidor
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *)0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }
    port = (unsigned short)atoi(argv[2]);

    /*
     * Define o endere�o IP e a porta do servidor
     */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /*
     * Cria um socket TCP (stream)
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket()");
        exit(3);
    }

    /* Estabelece conex�o com o servidor */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect()");
        exit(4);
    }

    do
    {
        printf("Digite o seu opção: ");
        __fpurge(stdin);
        scanf("%d", &opcao);

        if (send(s, &opcao, sizeof(opcao), 0) < 0)
        {
            perror("Send()");
            exit(5);
        }

        switch (opcao)
        {
        case 1:

            printf("\nDigite seu nome: ");
            __fpurge(stdin);
            scanf("%s", msg.nome);
            if (strlen(msg.nome) > 19)
            {
                msg.nome[19] = '\0';
            }

            printf("Digite seu texto: ");
            __fpurge(stdin);
            scanf("%[^\n]", msg.texto);
            if (strlen(msg.texto) > 79)
            {
                msg.texto[79] = '\0';
            }

            msg.ativo = 1;

            if (send(s, &msg, sizeof(msg), 0) < 0)
            {
                perror("Send()");
                exit(5);
            }
            printf("\nMensagem enviada ao servidor\n");

            if (recv(s, recvbuf, sizeof(recvbuf), 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }
            printf("Mensagem recebida do servidor: %s\n", recvbuf);
            break;

        case 2:

            if (recv(s, &quant_msg, sizeof(quant_msg), 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }

            

            if (recv(s, msg_total, sizeof(struct mensagem) * 10, 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }

            // for (i = 0; i < 10; i++)
            // {
            //     if (msg_total[i].ativo == 1)
            //     {
            //         printf("Mensagem %d\n", i + 1);
            //         printf("Nome: %s\n", msg_total[i].nome);
            //         printf("Mensagem: %s\n\n", msg_total[i].texto);
            //     }
            // }

            printf("%s\n", msg_total[0].nome);
            printf("%s\n", msg_total[0].texto);

            break;

        case 3:
            printf("Removendo as mensagens do servidor!\n");

            printf("Digite o nome: ");
            __fpurge(stdin);
            scanf("%s", nome);
            if (strlen(nome) > 19)
            {
                nome[19] = '\0';
            }

            if (send(s, nome, sizeof(nome), 0) < 0)
            {
                perror("Send()");
                exit(5);
            }

            if (recv(s, &quant_msg, sizeof(quant_msg), 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }

            if (recv(s, recvbuf, sizeof(recvbuf), 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }
            printf("Mensagem recebida do servidor: %s\n", recvbuf);
            break;

        case 4:
            printf("O cliente foi encerrado!\n");
            break;

        default:
            printf("Opcao invalida !\n\n");
            break;
        }
    } while (opcao != 4);
    /* Fecha o socket */
    close(s);

    printf("Cliente terminou com sucesso.\n");
    exit(0);
}
