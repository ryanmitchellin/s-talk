#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "list.h"

#define BUFFER_SIZE 256
List *messageList;
pthread_mutex_t inputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t messageReady = PTHREAD_COND_INITIALIZER;
char message[BUFFER_SIZE];
int sockfd;
struct sockaddr_in myAddr, remoteAddr;
struct hostent *host;

void *receiveAndPrintMessages(void *arg){
    int sockfd = *((int *)arg);
    char message[BUFFER_SIZE];
    struct sockaddr_in remoteAddr;
    socklen_t addrLen = sizeof(remoteAddr);
    struct hostent *remoteHost;
    char remoteHostname[BUFFER_SIZE];
    while (1){
        pthread_mutex_lock(&outputMutex);
        memset(message, 0, sizeof(message));
        if (recvfrom(sockfd, message, BUFFER_SIZE, 0, (struct sockaddr *)&remoteAddr, &addrLen) < 0){
            perror("Error receiving message");
            break;
        }
        if ((remoteHost = gethostbyaddr((const char *)&remoteAddr.sin_addr, sizeof(struct in_addr), AF_INET)) != NULL){
            strcpy(remoteHostname, remoteHost->h_name);
        }
        else{
            strcpy(remoteHostname, "Unknown Host");
        }
        fputs(remoteHostname, stdout);
        fputs(": ", stdout);
        fputs(message, stdout);
        if (strchr(message, '!') != NULL){
            fputs("Terminating.\n", stdout);
            exit(0);
        }
        pthread_mutex_unlock(&outputMutex);
    }
    close(sockfd);
    pthread_exit(NULL);
}

void *keyboardInput(void *arg){
    while (1){
        char message[BUFFER_SIZE];
        fgets(message, BUFFER_SIZE, stdin);
        if (strlen(message) > 1){
            pthread_mutex_lock(&inputMutex);
            List_append(messageList, message);
            pthread_mutex_unlock(&inputMutex);
            pthread_cond_signal(&messageReady);
        }
        if (strchr(message, '!') != NULL){
            fputs("Terminating.\n", stdout);
            exit(0);
        }
    }
}

void *sendMessages(void *arg){
    while (1){
        if (List_count(messageList) > 0){
            pthread_mutex_lock(&inputMutex);
            while (List_count(messageList) == 0){
                pthread_cond_wait(&messageReady, &inputMutex);
            }
            List_first(messageList);
            char *messageToSend = List_remove(messageList);
            pthread_mutex_unlock(&inputMutex);
            sendto(sockfd, messageToSend, strlen(messageToSend), 0, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
            if (strchr(message, '!') != NULL){
                fputs("Terminating.\n", stdout);
                exit(0);
            }
        }
    }
}

int main(int argc, char *argv[]){
    if (argc != 4){
        fputs("The format: ", stdout);
        fputs(argv[0], stdout);
        fputs(" [my port number] [remote machine name] [remote port number]\n", stdout);
        exit(1);
    }
    int myPort = atoi(argv[1]);
    char *remoteMachine = argv[2];
    int remotePort = atoi(argv[3]);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Error creating socket");
        exit(1);
    }
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(myPort);
    myAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0){
        perror("Error binding socket");
        exit(1);
    }
    host = gethostbyname(remoteMachine);
    if (host == NULL){
        perror("Error getting host by name");
        exit(1);
    }
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(remotePort);
    memcpy(&remoteAddr.sin_addr, host->h_addr, host->h_length);
    fputs("Connected to remote machine. You can start typing messages:\n", stdout);
    pthread_t sendThread, receiveAndPrintThread, keyboardThread;
    if (pthread_create(&sendThread, NULL, sendMessages, NULL) < 0){
        perror("Error creating send thread");
        exit(1);
    }
    if (pthread_create(&receiveAndPrintThread, NULL, receiveAndPrintMessages, (void *)&sockfd) < 0){
        perror("Error creating receive thread");
        exit(1);
    }
    if (pthread_create(&keyboardThread, NULL, keyboardInput, NULL) < 0){
        perror("Error creating keyboard input thread");
        exit(1);
    }
    messageList = List_create();
    pthread_join(sendThread, NULL);
    pthread_join(receiveAndPrintThread, NULL);
    pthread_join(keyboardThread, NULL);
    List_free(messageList, NULL);
    close(sockfd);
    return 0;
}