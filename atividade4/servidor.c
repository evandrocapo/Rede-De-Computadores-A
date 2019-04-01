/*
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

#define NUM_THREADS 5

pthread_t servidor_i[NUM_THREADS];
pthread_mutex_t semaforo;

struct mensagem
{
    char nome[20];
    char texto[80];
    int ativo;
};

struct mensagem msg[10]; // variavel global

void INThandler(int sig)
{
	if (pthread_mutex_destroy(&semaforo)) != 0)
	{
		fprintf(stderr, "impossivel remover semaforo");
		exit(1);
	}

	exit(0);
}

void *servidor(void *thread_id)
{
    int ns, opcao;
    char nome[20], sendbuf[100];
    struct mensagem msg;

    do
    {

        if (recv(ns, &opcao, sizeof(opcao), 0) == -1)
        {
            perror("Recv()");
            exit(6);
        }

        if (opcao == 1)
        {

             if (pthread_mutex_lock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	exit(1);
             }

            if (recv(ns, &msg, sizeof(msg), 0) == -1)
            {
                perror("Recv()");
                exit(7);
            }
            printf("\nMensagem do Cliente: %s\n", msg.nome);
            printf("Mensagem recebida do cliente: %s\n", msg.texto);

            for (int i = 0; i < 10; i++)
            {
                if (shrd[i].ativo == 0) // alterar para variavel globalizada da globo
                {
                    shrd[i] = msg; // alterar para variavel globalizada da globo
                    strcpy(sendbuf, "Incluso com sucesso!\n");
                    break;
                }
                else
                {
                    if (i == 9 && shrd[i].ativo == 1)
                    {
                        strcpy(sendbuf, "A mensagem não foi inclusa!\n");
                        break;
                    }
                }
            }
            //semaforo END

            if (send(ns, sendbuf, sizeof(sendbuf), 0) < 0)
            {
                perror("Send()");
                exit(5);
            }
             if (pthread_mutex_unlock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
             	exit(1);
             }
            printf("%s\n", sendbuf);
            fflush(stdout);
        }
        else
        {
            if (opcao == 2)
            {
                printf("Enviando as mensagens para o cliente");

                if (pthread_mutex_lock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	exit(1);
             }

                if (send(ns, shrd, sizeof(struct mensagem) * 10, 0) < 0) // alterar para variavel globalizada da globo
                {
                    perror("Send()");
                    exit(5);
                }

                if (pthread_mutex_unlock(&semaforo) != 0)
                {
                    fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
                    exit(1);
                }
            }
            else
            {
                if (opcao = 3)
                {
                    if (pthread_mutex_lock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	exit(1);
             }

                    if (recv(ns, nome, sizeof(nome), 0) == -1)
                    {
                        perror("Recv()");
                        exit(7);
                    }

                    for (i = 0; i < 10; i++)
                    {
                        if (strcmp(shrd[i].nome, nome) == 0) // alterar para variavel globalizada da globo
                        {
                            shrd[i].ativo = 0; // alterar para variavel globalizada da globo
                        }
                    }

                    if (pthread_mutex_unlock(&semaforo) != 0)
                    {
                        fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
                        exit(1);
                    }

                    printf("As mensagens foram apagadas\n");

                    strcpy(sendbuf, "As mensagens foram apagadas\n");

                    if (send(ns, sendbuf, sizeof(sendbuf), 0) < 0)
                    {

                        perror("Send()");
                        exit(5);
                    }
                }
            }
        }
    } while (opcao != 4);
    close(ns);

    printf("Servidor %ld encerrado", thread_id);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    unsigned short port;
    struct sockaddr_in client;
    struct sockaddr_in server;
    struct mensagem msg_only;
    long int pos = 0, i, opcao, s, namelen, ns; // passar como parametro pro thread ( precisar ser long int)

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

    //Criacao do semaforo da Regia Critica
    if(pthread_mutex_init(&semaforo, NULL) != 0) 
	{
		printf("ERRO: Impossivel criar um mutex\n");
    	exit(5);
	}

    if (pthread_mutex_unlock(&semaforo) != 0)
    {
        fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
        exit(6);
    }

    /*
     * Aceita uma conex�o e cria um novo socket atrav�s do qual
     * ocorrer� a comunica��o com o cliente.
     */
    namelen = sizeof(client);

    signal(SIGINT, INThandler);

    while (1)
    {
        namelen = sizeof(client);
        if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
        {
            perror("Accept()");
            exit(5);
        }
    }

    //criar
    for (i = 0; i < 10; i++)
    {
        pthread_create(&servidor_i[pos], NULL, servidor, (void *)pos); //enviar o POS, NS e o S para o fechamento 
        pos++;
    }

    sleep(5);
}