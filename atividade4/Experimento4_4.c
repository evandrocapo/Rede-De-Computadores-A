/*Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Brunno Mendes Vega ---------------------------------------- RA: 13144175
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins --------------------------------- RA: 16105744 
Pedro Andrade Caccavaro --------------------------------- RA: 16124679 
*/

//Includes Necessarios 
#include <pthread.h>			/* para poder manipular threads */
#include <stdio.h>				/* para printf() */
#include <stdlib.h>
#include <unistd.h>

//Constantes Necessarias
#define 	NUM_THREADS     5
#define 	PENSANDO		0
#define 	FAMINTO			1
#define 	COMENDO			2
#define		ESQ				(threadid + NUM_THREADS - 1) % NUM_THREADS
#define		DIR				(threadid + 1) % NUM_THREADS

//Variaveis
pthread_t filosofos[NUM_THREADS];
pthread_mutex_t garfos[NUM_THREADS];
pthread_mutex_t regiao;

int estado[ NUM_THREADS ];
int ciclo[NUM_THREADS]; //Ciclo é o número de vezes que cada filósofo comeu

//Funcoes

//Funçao para checar se o filosofo é liberado para comer e posteriormente liberar o garfo
void intencao(long int threadid){
	
	if(estado[ threadid ] == FAMINTO && estado[ ESQ ] != COMENDO && estado[ DIR ] != COMENDO){
	//O filosofo só pode comer se estiver faminto e os dois ao lado nao estiverem comendo
	
		printf("A filosofa %ld pode comer agora...\n", (long int)threadid+1);
		estado[ threadid ] = COMENDO;
		
		pthread_mutex_unlock( &(garfos[threadid]) );
	}
	
}




//Função para definir o estado do Filósofo e gastar tempo para isso
void pensar(long int threadid){
	
	estado[ threadid ] = PENSANDO;
	printf("A filosofa %ld esta pensando...\n", (long int)threadid+1);
	usleep(25/1000);
	
}

//Função com região crítica onde o filosofo confere se pode pegar o garfo
void pegar_garfo (long int threadid){
	pthread_mutex_lock( &regiao ); //Protecao da regiao critica
	
	printf("A filosofa %ld tenta pegar o garfo...\n", (long int)threadid+1);
	estado[ threadid ] = FAMINTO;
	intencao( threadid);
	
	pthread_mutex_unlock( &regiao );
	pthread_mutex_lock( &garfos[threadid ] ); //Travamento do Garfo do filosofo
}

//Apos comer o filosofo volta a pensar e libera os garfos caso o vizinho desejar comer
void devolver_garfo(long int threadid){
	pthread_mutex_lock ( &regiao );//regiao critica
	
	printf("A filosofa %ld acabou de comer...\n", (long int)threadid+1);
	estado[ threadid] = PENSANDO;
	intencao( ESQ );
	intencao( DIR );
	
	pthread_mutex_unlock( &regiao );
}



//Tempo para comer
void comer(long int threadid){

	
	printf("A filosofa %ld esta comendo...\n", (long int)threadid+1);
	usleep(5/1000);
	
	//return 1;
}

//Função principal da Thread, representa o filósofo
void *filosofo(void *threadid)
{
	
	long int id = (long int) threadid;

	ciclo[id] = 0;

	while(ciclo[id] != 2)
	{
		pensar( id );
		pegar_garfo( id );
		comer( id );
		ciclo[id]++;
		devolver_garfo( id );
	}	
}

//Main
int main(int argc, char *argv[])
{
  	int tf;
  	long int i;
  	
	if(pthread_mutex_init(&regiao, NULL) != 0) //Criacao do semaforo da Regia Critica
	{
		printf("ERRO: Impossivel criar um mutex\n");
    	return 1;
	}
	
	for(i=0; i<NUM_THREADS;i++) // Criacao do Semaforo de cada garfo
	{
		if(pthread_mutex_init(&garfos[i], NULL) != 0)
		{
			printf("ERRO: Impossivel criar um mutex\n");
        		return 1;
		}
	}
	
	
  	for (i=0;i<NUM_THREADS;i++)  //Inicializacao das Threads
  	{
  		tf = pthread_create(&filosofos[i], NULL, filosofo, (void *)i);	
  		if (tf) 
  		{
   			printf("ERRO: Impossivel criar um thread filosofo\n");
   			exit(-1);
 		}    
  	}

	//Finalizacao das Threads
	
  	for(i = 0;i < NUM_THREADS;i++)
  	{
    		pthread_join(filosofos[i], NULL);
    		
  	}

	for(i=0;i<NUM_THREADS;i++){
		pthread_mutex_destroy(&garfos[i]);
	}


	pthread_mutex_destroy(&regiao); 
	
  	printf("Terminando a thread main()\n");
  	pthread_exit(NULL);
}
