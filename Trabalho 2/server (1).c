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
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define clear() printf("\033[H\033[J")

pthread_t server_thread[20];
int t_count;
int onlineClients;

struct message{
    char command[20];
    char number[20];
};

struct handler{
    char ip[20];
    int port;
    int socket;
    char number[20];
};

struct info{
    char num[20];
    char ip[20];
    int port;
    int active;
};

struct onlineUsers{
    char number[20];
    char ip[20];
    int port;
    int socket;
}list[100];

int user_index = 0;

void *server_exec( void *socket){
    char rec_buffer[100], send_buffer[100];
    struct message msg;
    struct info ping_answer;
    struct handler local = *(struct handler *) socket;
    struct onlineUsers receiver;
    int local_socket = local.socket;
    int i,j;

    while(1){

        //memset( rec_buffer, 0, sizeof(rec_buffer) );
        recv( local_socket, &msg, sizeof(msg), 0 );

        printf("[+] Request from %s %d\n", local.ip, local.port);

        if( strcmp(msg.command, "check") == 0 ){

            printf("[+] Pinging %s\n", msg.number);
            for(i = 0; i < user_index; i++){
                if( strcmp(msg.number, list[i].number) == 0){
                    strncpy(ping_answer.ip,list[i].ip,20);
                    strncpy(ping_answer.num,list[i].number,20);
                    ping_answer.port = list[i].port;
                    send(local_socket, &ping_answer, sizeof(ping_answer), 0);
                    break;
                }
            }
            if(i == user_index){
                strncpy(ping_answer.num,"offline",20);
                send(local_socket, &ping_answer, sizeof(ping_answer), 0);
            }

        }

        else if( strcmp(msg.command,"connect") == 0 ){

            printf("[+] Retrieving Address of %s\n", msg.number);

            for(i = 0; i < user_index; i++){
                if( strcmp(msg.number, list[i].number) == 0){
                    strncpy(receiver.ip,list[i].ip,20);
                    strncpy(receiver.number,list[i].number,20);
                    receiver.port = list[i].port;
                    send(local_socket, &receiver, sizeof(receiver), 0);
                    break;
                }
            }
            if(i == user_index){
                strncpy(receiver.number,"offline",20);
                send(local_socket, &receiver, sizeof(receiver), 0);
            }

        }

        else if( strcmp(msg.command,"offline") == 0 ){
            printf("[+] Disconnecting %s\n", msg.number);

            for( i = 0; i < user_index;i++){
                if( strcmp(msg.number, list[i].number) == 0){
                    for(j = i; j < user_index;j++){
                        strncpy(list[j].ip,list[j+1].ip,20);
                        strncpy(list[j].number,list[j+1].number,20);
                        list[j].socket = list[j+1].socket;
                        list[j].port = list[j+1].port;
                    }
                    break;
                }
            }
            user_index--;
            close( local_socket );
            return 0;
        }
    }
}


int main(int argc, char **argv)
{
    unsigned short port;
    struct sockaddr_in client, server;

    int s, ns;

    struct message msg;
    int n, onlineClients;


    // argv[1] ===> port
    if (argc != 2)
    {
        fprintf(stderr, "[-] Usage: %s port\n", argv[0]);
        exit(1);
    }

    port = (unsigned short)atoi(argv[1]);

    // socket tcp (stream)
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-] Error");
        exit(2);
    }

    // inadddr_any ===> any IP address
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // Connect server to port
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("[-] Error");
        exit(3);
    }

    // wait for connections and create a queue
    if (listen(s, 1) != 0)
    {
        perror("[-] Error");
        exit(4);
    }
    // accept connection and create new socket
    n = sizeof(client);

    printf("[+] Server online\n\n");

    struct handler handler_client;

    while(1){

        if ((ns = accept(s, (struct sockaddr *)&client, &n)) == -1)
        {
            perror("[-] Error");
            exit(5);
        }

        printf("[+] Got a new connection from %s/%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));


        if (recv(ns, &list[user_index], sizeof(list[user_index]), 0) == -1)
        {
            perror("[-] Error");
            exit(6);
        }

        printf("[+] Connected: %s\n", list[user_index].number);

        user_index++;

        handler_client.socket = ns;
        handler_client.port = ntohs(client.sin_port);
        strncpy(handler_client.ip,inet_ntoa(client.sin_addr),20);
        strncpy(handler_client.number,msg.number,20);

        pthread_create( &(server_thread[t_count]), NULL, server_exec, (void *)&handler_client );
        t_count++;

    }

    close(s);
    close(ns);
}
