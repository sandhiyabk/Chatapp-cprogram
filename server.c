#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUF_SIZE 1024

int new_socket;

// thread: receive messages from client
void *receive_messages(void *arg) {
    char buffer[BUF_SIZE];
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int valread = read(new_socket, buffer, BUF_SIZE);
        if (valread <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("\nClient: %s\nYou: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buffer[BUF_SIZE];

    // create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // accept client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");

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
        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
