#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/sem.h>

#define SEM_KEY 0x2034

struct sigaction sigchld_action = {
		.sa_handler = SIG_DFL,
		.sa_flags = SA_NOCLDWAIT};

struct mensagem
{
	char nome[20];
	char texto[80];
	int ativo;
};

int sem_post(sem_t *semaphore);
int sem_wait(sem_t *semaphore);

int g_sem_id;

struct sembuf g_sem_op1[1];
struct sembuf g_sem_op2[1];

void INThandler(int sig)
{
	if (semctl(g_sem_id, 0, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "impossivel remover semaforo");
		exit(1);
	}

	exit(0);
}
/*
 * Servidor TCP
 */
void main(int argc, char **argv)
{
	unsigned short port;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int s, i; /* Socket para aceitar conex�es       */
	int ns;		/* Socket conectado ao cliente        */
	int namelen;
	pid_t pid, fid;
	char nome[20], sendbuf[100];
	int shm_id, semval;
	sem_t *sem;

	struct mensagem msg;
	struct mensagem msg_total[10];
	int quant_msg, opcao;

	g_sem_op1[0].sem_num = 0;
	g_sem_op1[0].sem_op = -1;
	g_sem_op1[0].sem_flg = 0;

	g_sem_op2[0].sem_num = 0;
	g_sem_op2[0].sem_op = 1;
	g_sem_op2[0].sem_flg = 0;

	//criando semaforo
	if ((g_sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666)) == -1)
	{
		fprintf(stderr, "semget() falhou, impossivel criar o semaforo");
		exit(1);
	}

	if (semop(g_sem_id, g_sem_op2, 1) == -1)
	{
		fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
		exit(2);
	}

	if (sigaction(SIGCHLD, &sigchld_action, NULL) == -1)
	{
		perror("SetSigChild()");
		exit(200);
	}

	shm_id = shmget(IPC_PRIVATE, sizeof(struct mensagem) * 10, IPC_CREAT | 0666);
	if (shm_id < 0)
	{
		printf("shmget error\n");
	}

	struct mensagem *shrd = (struct mensagem *)shmat(shm_id, NULL, 0);

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

	quant_msg = 0;

	//sem = sem_open("sem", O_CREAT | O_EXCL, 0644, 1 );

	signal(SIGINT, INThandler);

	while (1)
	{
		/*
	  * Aceita uma conex�o e cria um novo socket atrav�s do qual
	  * ocorrer� a comunica��o com o cliente.
	  */

		namelen = sizeof(client);
		if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
		{
			perror("Accept()");
			exit(5);
		}

		if ((pid = fork()) == 0)
		{
			/*
		 * Processo filho 
		 */

			/* Fecha o socket aguardando por conex�es */
			close(s);

			/* Processo filho obtem seu pr�prio pid */
			fid = getpid();

			do
			{

				if (recv(ns, &opcao, sizeof(opcao), 0) == -1)
				{
					perror("Recv()");
					exit(6);
				}

				if (opcao == 1)
				{

					if (semop(g_sem_id, g_sem_op1, 1) == -1)
					{
						fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
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
						if (shrd[i].ativo == 0)
						{
							shrd[i] = msg;
							break;
						}
					}

					sleep(10);
					//semaforo END

					strcpy(sendbuf, "Incluso com sucesso!\n");

					if (send(ns, sendbuf, sizeof(sendbuf), 0) < 0)
					{
						perror("Send()");
						exit(5);
					}
					if (semop(g_sem_id, g_sem_op2, 1) == -1)
					{
						fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
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

						if (semop(g_sem_id, g_sem_op1, 1) == -1)
						{
							fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
							exit(1);
						}

						if (send(ns, shrd, sizeof(struct mensagem) * 10, 0) < 0)
						{
							perror("Send()");
							exit(5);
						}

						if (semop(g_sem_id, g_sem_op2, 1) == -1)
						{
							fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
							exit(1);
						}
					}
					else
					{
						if (opcao = 3)
						{
							if (semop(g_sem_id, g_sem_op1, 1) == -1)
							{
								fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
								exit(1);
							}

							if (recv(ns, nome, sizeof(nome), 0) == -1)
							{
								perror("Recv()");
								exit(7);
							}

							for (i = 0; i < 10; i++)
							{
								if (strcmp(shrd[i].nome, nome) == 0)
								{
									shrd[i].ativo = 0;
								}
							}

							if (semop(g_sem_id, g_sem_op2, 1) == -1)
							{
								fprintf(stderr, "semop() falhou, impossivel iniciar o semaforo");
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

				// 	break;
				// case 4:
				// 	printf("\nA conexão está sendo encerrada!\n");

				// 	break;
				// }
			} while (opcao != 4);
			close(ns);

			/* Processo filho termina sua execu��o */

			printf("[%d] Processo filho terminado com sucesso.\n", fid);

			exit(0);
		}
		else
		{
			/*
		* Processo pai 
		*/

			if (pid > 0)
			{
				printf("Processo filho criado: %d\n", pid);

				/* Fecha o socket conectado ao cliente */
				close(ns);
			}
			else
			{
				perror("Fork()");
				exit(7);
			}
		}
	}
}
