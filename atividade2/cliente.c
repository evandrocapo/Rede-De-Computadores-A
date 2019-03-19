/*
Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744
Pedro Andrade Caccavaro --------------------------------- RA: 16124679
*/

#include <stdio.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Cliente TCP
 */

struct mensagem
{
    char nome[20];
    char texto[80];
};

int main(int argc, char **argv)
{
    unsigned short port;
    char sendbuf[12];
    char recvbuf[100];
    struct hostent *hostnm;
    struct sockaddr_in server;
    int s, i;

    struct mensagem msg;
    struct mensagem msg_total[10];
    int quant_msg, opcao;

    // strcpy(msg.nome, "Evandro");
    // strcpy(msg.texto, "Me2349y34y23hiuhdasidhuas");

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
            if(strlen(msg.nome) > 19){
                msg.nome[19] = '\0';
            }

            printf("Digite seu texto: ");
            __fpurge(stdin);
            scanf("%[^\n]", msg.texto);
            if(strlen(msg.texto) > 79){
                msg.texto[79] = '\0';
            }

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

            if (recv(s, msg_total, sizeof(msg_total), 0) < 0)
            {
                perror("Recv()");
                exit(6);
            }

            for (i = 0; i < quant_msg; i++)
            {
                printf("Mensagem %d\n", i + 1);
                printf("Nome: %s\n", msg_total[i].nome);
                printf("Mensagem: %s\n\n", msg_total[i].texto);
            }
            break;

        case 3:
            printf("Removendo as mensagens do servidor!\n");

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
