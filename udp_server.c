#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    if (argc != 3) {
        printf("2 parameters are expected. Server IP and port.\n");
    }
    char serverIP[20];
    strcpy(serverIP, argv[1]);
    int port = atoi(argv[2]);

    int socket_desc; // server socket id
    struct sockaddr_in server_addr, client_addr; // ip address of server and client
    char server_message[2000], client_message[2000];
    socklen_t client_struct_length = sizeof(client_addr); // size of client address
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(serverIP);
    
    // Bind the socket to the port and the server's IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    
    while(1) {
        printf("Listening for incoming messages...\n\n");
        
        // Receive client's message:
        if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
            (struct sockaddr*)&client_addr, &client_struct_length) < 0){
            printf("Couldn't receive\n");
            printf("Restart the receiving process\n");
            continue; // restart the receiving process
        } else {
            printf("Received message from IP: %s and port: %i\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            
            printf("Message from client: %s\n", client_message);
        }

        // Respond to client:
        strcpy(server_message, client_message);
        
        if (sendto(socket_desc, server_message, strlen(server_message), 0,
            (struct sockaddr*)&client_addr, client_struct_length) < 0){
            printf("Can't send\n");
            printf("Restart the receiving process\n");
            continue; // restart the receiving process
        } else {
            break;
        }
    }
    
    // Close the socket:
    close(socket_desc);
    
    return 0;
}