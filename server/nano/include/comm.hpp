#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>

#include "ansicodes.hpp"
#include "gamepad.hpp"

#define PORT 5240
#define VIDEOPORT 5241
#define MAX_EVENTS 5
#define TIMEOUT 500

class NANO; // Forward declaration

/* Handles TCP communication with the
 * windows client
 */
class COM {
   public:
    COM();
    ~COM();

    /* Starts the server on the defined PORT */
    void StartServer(int port);

    /* Waits for a client to connect */
    void AwaitClient();

    /* Reads a string message from the client */
    void RecieveMessage();

    int SendMessage(char* buff, size_t buff_siz);

    int SendImage(std::vector<unsigned char>& buff);

    /* Returns if a client is connected */
    bool HasClient();

    /* Prints the buffer out in string format */
    void PrintMessage();

    void SendACK();

    /* Reads the controller data from the client */
    bool GetControllerData(GAMEPAD& g, NANO* n = NULL);

    /* Destroys all resources */
    void HandleDisconnect();

   private:
    void create_ssl();
    void create_context();
    void configure_context();
    void destruct_ssl();

    int opt;                     // used to set socket options
    signed int valread;          // return value of read and write calls
    int addrlen;                 // size of sockaddr_in
    int server_sock;             // handle on the server socket (used to accept clients)
    int client_socket;           // handle on the client socket (used to read/write)
    bool has_client;             // whether a client is connected
    char buffer[1024];           // holds the recieved data
    struct sockaddr_in address;  // server addr information (used to setup server)
    int epoll_fd = -5;
    struct epoll_event event;
    struct epoll_event[MAX_EVENTS] events;
    SSL_CTX* context;
    SSL* ssl;
    const SSL_METHOD* method;
};