#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUF_SIZE 1024

int sock;

// thread: receive messages from server
void *receive_messages(void *arg) {
    char buffer[BUF_SIZE];
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int valread = read(sock, buffer, BUF_SIZE);
        if (valread <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("\nServer: %s\nYou: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // convert IPv4 address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to server!\n");

    // create thread for receiving
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // main thread: send messages
    while (1) {
        printf("You: ");
        fflush(stdout);
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, "/quit") == 0) {
            printf("Closing chat...\n");
            break;
        }
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
    return 0;
}
