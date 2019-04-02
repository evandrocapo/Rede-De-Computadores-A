#include <pthread.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

pthread_t servidor_i;
//pthread_mutex_t semaforo;
int opcao;

struct mensagem
{
    char nome[20];
    char texto[80];
    int ativo;
};

struct mensagem msg_gravada[10]; // variavel global

/*void intHandHer(int sig)
{
	if ((pthread_mutex_destroy(&semaforo)) != 0)
	{
		fprintf(stderr, "Impossivel remover semaforo");
		exit(1);
	}

	exit(0);
}*/

void *servidor(void *socket_servidor)
{
    char nome[20], sendbuf[100];
    struct mensagem msg;

    //close(s);
    do
    {

        if (recv(socket_servidor, &opcao, sizeof(opcao), 0) == -1)
        {
            perror("recv()");
            exit(6);
        }

        if (opcao == 1)
        {

             //if (pthread_mutex_lock(&semaforo) != 0)
             //{
             	//fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	//exit(1);
             //}

            if (recv(socket_servidor, &msg, sizeof(msg), 0) == -1)
            {
                perror("recv()");
                exit(7);
            }
            printf("\nMensagem do cliente: %s\n", msg.nome);
            printf("Mensagem recebida do cliente: %s\n", msg.texto);
            

            for (int i = 0; i < 10; i++)
            {
                if (msg_gravada[i].ativo == 0) // alterar para variavel globalizada da globo
                {
                    msg_gravada[i] = msg; // alterar para variavel globalizada da globo
                    strcpy(sendbuf, "Incluso com sucesso!\n");
                    break;
                }
                else
                {
                    if (i == 9 && msg_gravada[i].ativo == 1)
                    {
                        strcpy(sendbuf, "A mensagem não foi inclusa!\n");
                        break;
                    }
                }
            }
            //semaforo end

            if (send(socket_servidor, sendbuf, sizeof(sendbuf), 0) < 0)
            {
                perror("send()");
                exit(5);
            }
            // if (pthread_mutex_unlock(&semaforo) != 0)
             //{
             	//fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
             	//exit(1);
             //}
            printf("%s\n", sendbuf);
            fflush(stdout);
        }
        else
        {
            if (opcao == 2)
            {
                printf("Enviando as mensagens para o cliente");

                /*if (pthread_mutex_lock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	exit(1);
             }*/

                if (send(socket_servidor, msg_gravada, sizeof(struct mensagem) * 10, 0) < 0) // alterar para variavel globalizada da globo
                {
                    perror("send()");
                    exit(5);
                }

                /*if (pthread_mutex_unlock(&semaforo) != 0)
                {
                    fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
                    exit(1);
                }*/
            }
            else
            {
                if (opcao == 3)
                {
                /*    if (pthread_mutex_lock(&semaforo) != 0)
             {
             	fprintf(stderr, "pthread_mutex_lock() falhou, impossivel fechar o semaforo");
             	exit(1);
             }*/

                    if (recv(socket_servidor, nome, sizeof(nome), 0) == -1)
                    {
                        perror("recv()");
                        exit(7);
                    }

                    for (int i = 0; i < 10; i++)
                    {
                        if (strcmp(msg_gravada[i].nome, nome) == 0) // alterar para variavel globalizada da globo
                        {
                            msg_gravada[i].ativo = 0; // alterar para variavel globalizada da globo
                        }
                    }

                    /*if (pthread_mutex_unlock(&semaforo) != 0)
                    {
                        fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
                        exit(1);
                    }*/

                    printf("As mensagens foram apagadas\n");

                    strcpy(sendbuf, "As mensagens foram apagadas\n");

                    if (send(socket_servidor, sendbuf, sizeof(sendbuf), 0) < 0)
                    {

                        perror("send()");
                        exit(5);
                    }
                }
            }
        }
    } while (opcao != 4);
    close(socket_servidor);

    printf("Servidor Thread %ld encerrado\n", pthread_self());
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    unsigned short port;
    struct sockaddr_in client;
    struct sockaddr_in server;
    struct mensagem msg_only;
    int namelen, s;
    long int ns; 

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
    /*if(pthread_mutex_init(&semaforo, null) != 0) 
	{
		printf("erro: impossivel criar um mutex\n");
    	exit(5);
	}

    if (pthread_mutex_unlock(&semaforo) != 0)
    {
        fprintf(stderr, "pthread_mutex_unlock() falhou, impossivel iniciar o semaforo");
        exit(6);
    }*/

    /*
     * aceita uma conex�o e cria um novo socket atrav�s do qual
     * ocorrer� a comunica��o com o cliente.
     */

    namelen = sizeof(client);

    //signal(sigint, inthandler);

    while (1)
    {
        namelen = sizeof(client);
        if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
        {
            perror("accept()");
            exit(5);
        }
	
	//criar thread
	if(pthread_create(&servidor_i, NULL, servidor, ns) != 0){	//enviar o pos, ns e o s para o fechamento
		perror("pthread_create()");
		exit(6);
	} 
    }
 
}
