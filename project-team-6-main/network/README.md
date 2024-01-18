# CS50 Blackjack Network Module

## Network

The client-server model is one of the most commonly used communication paradigms in networked systems. 
A server is a long-running program that stands idle until some clients connect to the server and request service. 

When the client contacts the server over Transmission Control Protocol (TCP), it asks to ‘connect’ and the server must then ‘accept’ the connection. 
Once open, the connection between a ‘socket’ on the client and a ‘socket’ on the server acts as if there is a ‘stream’ of bytes flowing from client to server, and a ‘stream’ of bytes flowing from server to client. 
The client writes to its socket and the bytes stream over the connection to the server, who then reads bytes from its socket… and vice versa.

In order to simplify the connection between the dealer and player in our game of Blackjack, the *network* module, defined in `network.h` and implemented in `network.c`, implements the setup and shutdown functions as well as message passing through the socket.

## Usage

The *network* module exports the following functions:

```c
void start_server(const int port, int* server_fd, int* new_socket);
void shutdown_server(int new_socket, int server_fd);
bool connect_to_server(const char *server_addr, const int port, int* client_fd, int* sock);
void close_client(int client_fd);
void send_message(char* message, int sock);
void recieve_message(char* buffer, int sock);
void send_join_message(char* team_name, int sock);
```

## Files

* `Makefile` - compilation procedure
* `network.h` - the interface
* `network.c` - the implementation
* `client.c` - unit test driver from client side
* `server.c` - unit test driver from server side

## Compilation

To compile, type `make` at the command line while in the network folder.