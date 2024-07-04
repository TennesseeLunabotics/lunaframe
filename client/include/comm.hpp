#ifndef COMM_H
#define COMM_H

// general includes
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <mstcpip.h>
#include <WS2tcpip.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

// our custom includes
#include "ansicodes.hpp"
#include "controller.hpp"

// windows libs to include
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "fwpuclnt.lib")

/* Handles TCP communication.
 * Uses OpenSSL to encrypt the communication.
 */
class tcp_stream
{
public:

  /* Tries to connect via TCP to ip:port */
  tcp_stream();
  ~tcp_stream();

  /* Send a string message */
  void send(const char *to_send);

  /* Send the XINPUT GAMEPAD state */
  void send(const XINPUT_GAMEPAD &to_send);

  /* Recieve a message from the connection */
  int recieve(char *to_recieve, size_t len);

  /* Calls Init*/
  void StartStream(const char* hostname, const char* port);

  void CloseStream();

  void SetNoBlocking();

private:
  /* Shutdown the socket (ran by deconstructor) */
  void close_socket(SOCKET handle);

  /* Initialize a connection */
  int InitializeStream(const char* hostname, const char* port);
  

  SOCKET sock_fd;               // socket handle
  struct addrinfo hints, *res;    // connection variables
  SSL_CTX* context;
  SSL* ssl;
};
#endif