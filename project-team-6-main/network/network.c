/* 
 * network.c - Blackjack network module
 * 
 * see network.h for more information
 *
 * Alexander Ye, Nov 2022
 * CS 50 Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <unistd.h>
#include <netinet/in.h>
#include "network.h"
#include "mem.h"


void start_server(const int port, int* server_fd, int* new_socket) 
{
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf("Setting up socket\n");
    // Creating socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Attaching socket to the PORT
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Binding socket to the PORT
    if (bind(*server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(*server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection... ");
    fflush(stdout); //ensure line above prints before waiting for connection

    if ((*new_socket = accept(*server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        printf("\n");
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Server started.\n");
}


void shutdown_server(int new_socket, int server_fd)
{
    // closing the connected socket
    close(new_socket);
    printf("Closing connected socket \n");
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    printf("Closing listening socket \n");
}


bool connect_to_server(const char *server_addr, const int port, int* client_fd, int* new_socket) 
{
    bool status = false;
    struct sockaddr_in serv_addr;
    
    if ((*new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
  
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_addr, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
  
    if ((*client_fd = connect(*new_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    status = true;
    //printf("Got connection: %d\n", status);
    return status;
}


void close_client(int client_fd)
{
    // closing the connected socket
    close(client_fd);
    printf("Closing client\n");
}


void send_message(char* message, int sock)
{
    printf("Sending message %s\n", message);
    send(sock, message, strlen(message) + 1, 0);
}


void recieve_message(char* buffer, int sock)
{
    memset(buffer, 0, 1024);  // clear up the buffer
    int valread = read(sock, buffer, 1024);
    if (valread < 0) {
        perror("reading message");
        exit(EXIT_FAILURE);
    }

    printf("Read: %s\n", buffer);
}


void send_join_message(char* team_name, int sock)
{
    char* join = mem_malloc_assert(strlen(team_name) + 6, "Bad JOIN string\n");
    sprintf(join, "JOIN %s", team_name);
    send_message(join, sock);
    free(join);
}
