/* 
 * client.c - test program for the client side
 *
 * input: none
 * output: none
 *
 * compile: make
 * usage: ./client (open a sepearte terminal)
 *
 * CS 50 Group 6, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <unistd.h>
#include "network.h"
#include "mem.h"


int main() {
    const char *SERVER_ADDRESS = "129.170.64.104";
    const int PORT = 8086;
    int sock, client_fd;
    // a buffer for the server to read messages into
    char buffer[1024] = { 0 }; 

    char* name = mem_malloc_assert(9, "failed to allocate memory for name\n");
    strcpy(name, "team_six");

    connect_to_server(SERVER_ADDRESS, PORT, &client_fd, &sock);
    send_join_message(name, sock);

    free(name);

    bool finish = false;

    while (! finish) {
        recieve_message(buffer, sock);
        // check the contents of the read message
        if (strncmp(buffer, "DECISION", 9) == 0) {
            send_message("STAND", sock);
            sleep(1);
        }
        if (strncmp(buffer, "QUIT", 5) == 0) {
            finish = true;
        }
    }

    close_client(client_fd);
}