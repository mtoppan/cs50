/* 
 * network.h - a header file for CS50 Group 6 Blackjack 'network' module
 * 
 * The network module provides the client (player) and server (dealer) with setup and shutdown functions 
 * as well as the ability to pass messages between each other through a socket.
 *
 * Alexander Ye, Nov 2022
 * CS 50 Fall 2022
 */

#ifndef NETWORK_H
#define NETWORK_H

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


/********* start_server *********
* Going to be called by server, our dealer module
* Starts the server and a new socket if a client connects. Otherwise, exits failure.
* Caller should initilize two integers for the server file descriptor and new socket 
* and pass in the address of them to be updated.
*/
void start_server(const int port, int* server_fd, int* new_socket);


/********* shutdown_server *********
* Closes the connected socket and listening socket of server
*/
void shutdown_server(int new_socket, int server_fd);


/********* connect_to_server *********
* Going to be called by a client, our player module
* Connects a client to the server. Otherwise, exits failure.
* Caller should initilize two integers for the client file descriptor and new socket 
* and pass in the address to them to be updated.
* Returns a boolean whether the connection was made. 0 if false, 1 if true
*/
bool connect_to_server(const char *server_addr, const int port, int* client_fd, int* sock);


/********* close_client *********
* Closes the connected socket and listening socket of server
*/
void close_client(int client_fd);

/********* send_message *********
* Going to be called by both the client and server
* Sends message through the socket to the other side
*/
void send_message(char* message, int sock);

/********* recieve_message *********
* Going to be called by both the client and server
* Recieves message through the socket and reads into a buffer
* Caller must initialize a character array for the message to be read into 
*/
void recieve_message(char* buffer, int sock);

/********* send_join_message *********
* A convenient wrapper function called by the client 
* Sends a join message with a passed in team name through the socket to the other side
*/
void send_join_message(char* team_name, int sock);

#endif // NETWORK_H