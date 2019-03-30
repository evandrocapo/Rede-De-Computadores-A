/*Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Brunno Mendes Vega ---------------------------------------- RA: 13144175
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins --------------------------------- RA: 16105744 
Pedro Andrade Caccavaro --------------------------------- RA: 16124679 


 * Includes Necessarios 
 */

#include <pthread.h>			/* para poder manipular threads */
#include <stdio.h>				/* para printf() */
#include <stdlib.h>

/*
 * Constantes Necessarias 
 */

#define NUM_THREADS     10
#define SIZEOFBUFFER    50
#define NO_OF_ITERATIONS 100

/*
 * O tipo pthread_t permite a declaração de uma variável que recebe
 * um id quando o thread é criado. Posteriormente, esse id pode ser
 * usado em comandos de controle para threads.
 * Seguem dois vetores para ids, para um numero de threads igual a
 * constante NUM_THREADS
 */

pthread_t consumers[NUM_THREADS];
pthread_t producers[NUM_THREADS];

/*
 * Variaveis Necessarias 
 */

int buffer[SIZEOFBUFFER];		/* Este e um buffer circular	*/
int *start;						/* apontara para a primeira posicao do buffer */
int *rp;						/* eh o apontador para o proximo item do buffer a ser consumido */
int *wp;						/* eh o apontador para o proximo item do buffer a ser produzido */
int cont_p = 0;             	/* eh um contador para controlar o numero de itens produzidos */
int cont_c = 0;         		/* eh um contador para controlar o numero de itens consumidos */


/*
 * Rotina para produzir um item toAdd no buffer 
 */

int myadd(int toAdd)
{
  if ((rp != (wp+1)) && (rp + SIZEOFBUFFER - 1 != wp)) 
  {
    *wp = toAdd;
    wp++;
      if (wp == (start + SIZEOFBUFFER)) 	//verificacao se wp chegou a ultima posicao do buffer
      {
        wp = start;						/* realiza a circularidade no buffer */
      }
      return 1;
    } 
    else return 0;
  }

/*
 * Rotina para consumir um item do buffer e coloca-lo em retValue 
 */

  int myremove() 
  {
    int retValue;
  if (wp != rp) 	//verificacao se o buffer nao esta vazio
  {
    retValue = *rp;
    rp++;
	if (rp == (start + SIZEOFBUFFER)) 	//verificacao se rp chegou a ultima posicao do buffer
	{
       rp = start;				/* realiza a circularidade no buffer */
  }
  return retValue;
} 
else return 0;
}

/*
 * A rotina produce e responsavel por chamar myadd para que seja 
 * colocado o valor 10 em uma posicao do buffer NO_OF_ITERATIONS vezes
 */

void *produce(void *threadid)
{
  int sum = 0;
  int ret = 0;
  int erros = 0;
  printf("Produtor #%ld iniciou...\n", (long int)threadid);
  while (cont_p < NO_OF_ITERATIONS) 
  {
    ret = myadd(10);
    if (ret) 
    {
      cont_p++;
      sum += 10;
      //printf("Foi inserido o numero %d pelo Produtor %ld\n",cont_p,threadid);
    }
    else{
      erros++;
    }
  }
  printf("Soma do que foi produzido pelo Produtor #%ld : %d\n", (long int)threadid, sum);
  printf("----> Erros para inserir com o Produtor #%ld : %d\n", (long int)threadid, erros);
  pthread_exit(NULL);
}

/*
 * A rotina consume e responsavel por chamar myremove para que seja
 * retorando um dos valores existentes no buffer NO_OF_ITERATIONS vezes 
 */

void *consume(void *threadid)
{
  int sum = 0;
  int ret;
  int erros =0;
  printf("Consumidor #%ld iniciou...\n", (long int)threadid);
  while (cont_c < NO_OF_ITERATIONS) 
  {
    ret = myremove();
    if (ret != 0) 
    {
      cont_c++;
      sum += ret;
      //printf("Foi retirado o numero %d pelo Consumidor %ld\n",cont_c,threadid);
    }
        else{
      erros++;
    }
  }
  printf("Soma do que foi consumido pelo Consumidor #%ld : %d\n", (long int)threadid, sum);
  printf("--> Erros para inserir com o Consumidor #%ld : %d\n", (long int)threadid, erros);
  pthread_exit(NULL);
}

/*
 * Rotina Principal (que tambem e a thread principal, quando executada) 
 */

int main(int argc, char *argv[])
{
  int tp, tc;
  long int i;
  start = &buffer[0];
  wp = start;
  rp = start;
  for (i=0;i<NUM_THREADS;i++) 
  {

  tc = pthread_create(&consumers[i], NULL, consume, (void *)i+1);		// tenta criar um thread consumidor
  if (tc) 
  {
   printf("ERRO: impossivel criar um thread consumidor\n");
   exit(-1);
 }

    tp = pthread_create(&producers[i], NULL, produce, (void *)i+1);   // tenta criar um thread produtor
    if (tp) 
    {
      printf("ERRO: impossivel criar um thread produtor\n");
      exit(-1);
    }
    
  }

  for(i = 0;i < NUM_THREADS;i++){
    pthread_join(consumers[i], NULL);
    pthread_join(producers[i], NULL);
  }
  printf("Terminando a thread main()\n");
  pthread_exit(NULL);
}
