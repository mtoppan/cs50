/* 
 * dealertest.c - test client to run alongside dealer
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

/**************** main ****************/
/* communicates with dealer like a player would 
 * connects to server and makes game decisions
 */
int main() {
    const char *SERVER_ADDRESS = "129.170.64.104";
    const int PORT = 8086;
    int sock, client_fd;
    char buffer[1024] = { 0 }; 

    char name[] = "team_six";

    //connects to dealer's server and communicates that it joined
    connect_to_server(SERVER_ADDRESS, PORT, &client_fd, &sock);
    send_join_message(name, sock);

    bool finish = false;

    //loops playing the game until player quits 
    while (! finish) {
        recieve_message(buffer, sock);
        //"STAND" action is hardcoded
        //can change it to "HIT" to test 
        if (strncmp(buffer, "DECISION", 9) == 0) {
            send_message("STAND", sock);
        }
        if (strncmp(buffer, "QUIT", 5) == 0) {
            finish = true;
        }
     }

    //close the client 
    close_client(client_fd);
}