#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

int base = 500, maxInterval = 8000; // ms
int multiplier = 2;
int maxRetry = 10;

int getInterval (int nowInterval) {
    if (nowInterval >= maxInterval) return maxInterval;
    else if (nowInterval == 0) return base;
    else return nowInterval * multiplier;
}

void waitRetry (int *retryTimes, int *sleepInterval) {
    (*retryTimes)++;
    *sleepInterval = getInterval(*sleepInterval);
    printf("\nRetry %d time(s)\n", *retryTimes);
    printf("Wait for %d ms to send the next message.\n", *sleepInterval);
    usleep(*sleepInterval * 1000);
}

int main (int argc, char *argv[]) {
    if (argc != 4) {
        printf("3 parameters are expected. Server IP, port, and message.\n");
    }

    char serverIP[20];
    strcpy(serverIP, argv[1]);
    int port = atoi(argv[2]);
    
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[100], client_message[100];
    socklen_t server_struct_length = sizeof(server_addr);
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Set client message:
    strcpy(client_message, argv[3]);
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (socket_desc < 0) {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // set socket timeout
    struct timeval tv;
    tv.tv_sec = 1; // 1 sec
    tv.tv_usec = 0;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_SNDTIMEO, &tv,sizeof(tv)) < 0) {
        perror("Cannot set socket timeout");
    }
    if (setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
        perror("Cannot set socket timeout");
    }
    
    // Set server's port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(serverIP);

    // send the message to server and receive the same one back
    int retryTimes = 0;
    int sleepInterval = 0;
    while (retryTimes < maxRetry) {
        printf("Send the message to server...\n");
        if (sendto(socket_desc, client_message, strlen(client_message), 0,
            (struct sockaddr*)&server_addr, server_struct_length) < 0){
            printf("Unable to send message\n");
            
            waitRetry(&retryTimes, &sleepInterval);
            continue; // cannot send the msg out from client, resend the msg again
        }
        
        // Receive the server's response:
        if (recvfrom(socket_desc, server_message, sizeof(server_message), 0,
            (struct sockaddr*)&server_addr, &server_struct_length) < 0){
            perror("Error while receiving server's message");

            waitRetry(&retryTimes, &sleepInterval);
            continue; // cannot receive response from server, resend the msg again
        } else {
            printf("Received message from IP: %s and port: %i\n",
                inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
            printf("Server's response: %s\n", server_message);
            break;
        }
    }
    
    // Close the socket:
    close(socket_desc);
    
    if (retryTimes == maxRetry) return 1;
    else return 0;
}