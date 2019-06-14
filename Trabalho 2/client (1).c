/*
Agostinho Sanches de Araújo ----------------------------- RA: 16507915
Evandro Douglas Capovilla Junior ------------------------ RA: 16023905
Lucas Tenani Felix Martins ------------------------------ RA: 16105744
Pedro Andrade Caccavaro --------------------------------- RA: 16124679
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <pthread.h>

#define clear() printf("\033[H\033[J")

// server requests
#define online  "online"
#define offline "offline"
#define check   "check"

struct message{
    char num_sender[20];
    char text[80];
};

struct command{
    char cmd[20];
    char number[20];
};

struct info{
    char number[20];
    char ip[20];
    int port;
    int socket;
};


struct info onlineUsers[100];

struct info connected_user;

int user_index;
char number[20];
char recvbuf[100],sendbuf[100];

int local_socket, local_port;
char local_ip[20];

int chat_socket;

struct sockaddr_in local_server;
struct sockaddr_in client_server;

pthread_t tid[2];

// Title
void printName(){
    printf(" __          ___           _                     ___        \n");
    printf(" \\ \\        / / |         | |         /\\        |__ \\       \n");
    printf("  \\ \\  /\\  / /| |__   __ _| |_ ___   /  \\   _ __   ) |_ __  \n");
    printf("   \\ \\/  \\/ / | '_ \\ / _` | __/ __| / /\\ \\ | '_ \\ / /| '_ \\ \n");
    printf("    \\  /\\  /  | | | | (_| | |_\\__ \\/ ____ \\| |_) / /_| |_) |\n");
    printf("     \\/  \\/   |_| |_|\\__,_|\\__|___/_/    \\_\\ .__/____| .__/ \n");
    printf("                                           | |       | |    \n");
    printf("                                           |_|       |_|    \n");
    printf("                                    version 1.0 by Titans\n");
}

// Menu
void printMenu(){
    printf("[+] Options:\n");
    printf("1 - Ping user\n");
    printf("2 - Send Message\n");
    printf("3 - View Contacts\n");
    printf("4 - Add New Contact\n");
    printf("5 - Logout\n");
    printf("\n[+] New Messages:\n\n");
}

// Create Server to receive connections
void createLocalServer(){
    char msg[80];
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);


    // socket TCP (stream)
    if ((local_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-] Error");
        exit(3);
    }

    // IP address and Server port
    local_server.sin_family = AF_INET;
    local_server.sin_port = 0;
    local_server.sin_addr.s_addr = inet_addr("127.0.0.1");


    // Connect server to port
    if (bind(local_socket, (struct sockaddr *)&local_server, sizeof(local_server)) < 0)
    {
        perror("[-] Error");
        exit(3);
    }

    // wait for connections and create a queue
    if (listen(local_socket, 1) != 0)
    {
        perror("[-] Error");
        exit(4);
    }

    if (getsockname(local_socket, (struct sockaddr *)&sin, &len) == -1){
        perror("[-] Error");
    }
    else{
        local_port = ntohs(sin.sin_port);
        strncpy(local_ip, inet_ntoa(local_server.sin_addr),20);
    }
}

// thread to keep receiving Messages
void *server_thread(){
    int ns, n;
    struct message msg;

    n = sizeof(local_server);

    while(1){

        if ((ns = accept(local_socket, (struct sockaddr *)&local_server, &n)) == -1)
        {
            perror("[-] Error");
            exit(5);
        }


        if (recv(ns, &msg, sizeof(msg), 0) == -1)
        {
            perror("[-] Error");
            exit(6);
        }
        strtok(msg.num_sender,"\n");
        printf("> Message from %s: %s\n", msg.num_sender,msg.text);
    }
}

// Create connection to client
void goChat(){
    unsigned short port;
    struct hostent *hostnm;

    hostnm = gethostbyname(connected_user.ip);
    if (hostnm == (struct hostent *)0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }
    port = connected_user.port;

    // IP address and Server port
    client_server.sin_family = AF_INET;
    client_server.sin_port = htons(port);
    client_server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    // socket TCP (stream)
    if ((chat_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-] Error");
        exit(3);
    }

    // Connecting with server
    if (connect(chat_socket, (struct sockaddr *)&client_server, sizeof(client_server)) < 0)
    {
        perror("[-] Error");
        exit(4);
    }
}


// TCP Client
void main(int argc, char **argv)
{
    struct sockaddr_in central_server;

    unsigned short port;
    struct hostent *hostnm;

    struct info clientInfo;
    struct info receivedInfo;

    struct command sender;


    int s, count;
    char new[20], contacts[50][20];
    char selectOption;
    int aux = 0;
    int i,j;

    struct message msg;
    struct message msg_total[10];

    int quant_msg, option;

    FILE *fptr;
    char line[20];


    // argv[1] ==> server hostname, argv[2] ==> server port
    if (argc != 3)
    {
        fprintf(stderr, "[-] Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    printName();
    printf("Welcome!\nPlease insert your number: ");

    //pthread_create(&tid[0], NULL, server_thread, NULL);

    fgets( number, 20, stdin );
    printf("[+] Connecting to server...\n");

    // Hostname
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *)0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }
    port = (unsigned short)atoi(argv[2]);

    // IP address and Server port
    central_server.sin_family = AF_INET;
    central_server.sin_port = htons(port);
    central_server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    // socket TCP (stream)
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-] Error");
        exit(3);
    }

    // Connecting with server
    if (connect(s, (struct sockaddr *)&central_server, sizeof(central_server)) < 0)
    {
        perror("[-] Error");
        exit(4);
    }

    createLocalServer();
    pthread_create(&tid[0], NULL, server_thread, NULL);

    strncpy(clientInfo.number,number,20);
    clientInfo.port = local_port;
    strncpy(clientInfo.ip,local_ip,20);
    //Sending id to Server
    if (send(s, &clientInfo, sizeof(clientInfo), 0) < 0)
    {
        perror("Send()");
        exit(5);
    }


    printf("[+] Connection succesful!\n");
    printf("[+] Listening at %s/%d\n", local_ip, local_port);
    printf("[+] Retrieving contacts...\n");

    // Opening File
    fptr = fopen("contacts.txt","a+");
    if(!fptr){
        printf("[-] Error: Could not retrieve contacts\n");
        exit(1);
    }

    printf("[+] Contacts retrieved!\n\n");
    user_index=0;

    do
    {
        printMenu();
         __fpurge(stdin);
         scanf("%d", &option);

        switch (option)
        {
            case 1:
                clear();
                printName();
                printf("Ping number: ");
                __fpurge(stdin);
                fgets( sender.number, 20, stdin );
                strncpy(sender.cmd,"check",20);

                send(s, &sender, sizeof(sender), 0);

                recv( s, &receivedInfo, sizeof(receivedInfo), 0 );

                if( strcmp(receivedInfo.number,"offline") == 0){
                    printf("[-] Offline\n");
                }
                else{
                    printf("[+] Online: %s/%d\n", receivedInfo.ip,receivedInfo.port);
                    for(i = 0; i < user_index; i++){
                        if( strcmp(onlineUsers[i].number, receivedInfo.number) ==0 ){
                            break;
                        }
                    }
                    if(i == user_index){
                        strncpy(onlineUsers[user_index].number,receivedInfo.number,20);
                        onlineUsers[user_index].port = receivedInfo.port;
                        strncpy(onlineUsers[user_index].ip,receivedInfo.ip,20);
                        user_index++;
                    }
                }
                printf("[-] Press 0 to return\n");

                scanf("%i", &option);
                break;

            case 2:
                clear();
                printName();
                printf("Recipient number: ");
                __fpurge(stdin);
                fgets( sender.number, 20, stdin );
                strncpy( sender.cmd, "connect", 20);

                send(s,&sender,sizeof(sender),0);

                recv( s, &connected_user, sizeof(connected_user), 0 );
                if( strcmp(connected_user.number,"offline") == 0){
                    printf("[-] Offline\n");

                    printf("[-] Press 0 to return\n");

                    scanf("%i", &option);strncpy( msg.num_sender,number,20);

                }else{
                    goChat();
                    printf("[+] Sending message\n> ");
                    __fpurge(stdin);
                    fgets( msg.text, 80, stdin );
                    strncpy( msg.num_sender,number,20);

                    if (send(chat_socket, &msg, sizeof(msg), 0) < 0)
                    {
                        perror("Send()");
                        exit(5);
                    }

                }
                break;

            case 3:
                clear();
                printName();
                count = 0;
                rewind(fptr);
                while ( fgets(line, sizeof(line), fptr) != NULL )
                {
                    strncpy(contacts[count],line,sizeof(line));
                    count++;
                }
                printf("[+] Contacts:\n");
                printf(" ");
                for(int i = 0; i < count; i ++){

                    for(j = 0; j < user_index; j++){
                        if( strcmp(onlineUsers[j].number, contacts[i]) == 0 ){
                            strtok(onlineUsers[j].number,"\n");
                            printf("%s (Online: %s/%d)\n", onlineUsers[j].number, onlineUsers[j].ip,onlineUsers[j].port);
                            break;
                        }
                    }
                    if( j == user_index){
                        strtok(contacts[i],"\n");
                        printf("%s (Offline)\n", contacts[i]);
                    }
                }

                printf("[-] Press 0 to return\n");

                scanf("%i", &option);
                break;

            case 4:

                clear();
                printName();
                printf("New contact number: ");
                __fpurge(stdin);
                scanf("%s", new);
                fprintf(fptr,"%s\n",new);
                break;

            case 5:
                strncpy(sender.cmd,"offline",20);
                strncpy(sender.number,number,20);
                if (send(s, &sender, sizeof(sender), 0) < 0)
                {
                    perror("Send()");
                    exit(5);
                }

            default:break;
        }

        clear();
        printName();

    } while (option != 5);

    printf("                             Bye bye!\n");
    printf("                            ¯\\_(ツ)_/¯\n");

    // Close socket
    close(s);

    // printf("Cliente terminou com sucesso.\n");
    // Close file
    fclose(fptr);

    exit(0);
}
