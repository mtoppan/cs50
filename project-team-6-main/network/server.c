/* 
 * server.c - test program for the server side
 *
 * input: none
 * output: none
 *
 * compile: make
 * usage: ./server
 *
 * CS 50 Group 6, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include "network.h"


void play(int new_socket);

int main(int argc, char* argv[]) 
{
    int PORT = 8086;
    int server_fd, new_socket;
    
    start_server(PORT, &server_fd, &new_socket);

    play(new_socket);
    
    // once done playing, shut down server
    shutdown_server(new_socket, server_fd);

    return 0;
}

void play(int new_socket)
{
    bool finish = false;
    // a buffer for the server to read messages into
    char buffer[1024] = { 0 };

    while (! finish) {
        recieve_message(buffer, new_socket);
        // check the contents of the read message
        if (strncmp(buffer, "JOIN", 5)!=0){
            send_message("CARDS", new_socket);
            send_message("DECISION", new_socket);
            sleep(1);
        }
        if (strncmp(buffer, "STAND", 6)!=0) {
            send_message("QUIT", new_socket);
            sleep(1);
            finish = true;
        }
    }
}