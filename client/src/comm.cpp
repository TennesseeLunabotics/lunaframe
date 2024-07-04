#include "../include/comm.hpp"

#include <fstream>
#include <iostream>
#include <string>

/* Does what it says */
void configure_client_context(SSL_CTX* ctx) {
  /*
   * Configure the client to abort the handshake if certificate verification
   * fails
   */
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

  /*
   * We are using a self-signed certificate, so the client must trust it
   * directly.
   */
  if (!SSL_CTX_load_verify_locations(ctx, "C:\\lunakey\\cert.pem", NULL)) {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
}

/* Sets up the SSL context */
SSL_CTX* create_context() {
  const SSL_METHOD* method;
  SSL_CTX* ctx;

  // we are using TLS so we get that method
  method = TLS_client_method();

  // create the context and error check it
  ctx = SSL_CTX_new(method);
  if (ctx == NULL) {
    LUNA_ERROR("Unable to create SSL context");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

int tcp_stream::InitializeStream(const char* hostname, const char* port)
{
  int err;
  printf("sock before %d\n", sock_fd);
  if (sock_fd != -1) {
    printf("Closing old Stream\n");
    CloseStream();
  }
  printf("sock after %d\n", sock_fd);
  //sock_fd = 0;

  // set addr and hints defaults
  sockaddr_in addr{0};
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  addr.sin_family = AF_INET;

  printf("Create context\n");
  context = create_context();

  printf("configure client context\n");
  configure_client_context(context);

  try {
    // resolve the address
    printf("getaddrinfo\n");
    if ((err = getaddrinfo(hostname, port, &hints, &res)) != 0) {
      printf("error %d : %s : %s\n", err, gai_strerror(err), hostname);
      throw std::runtime_error("failed to resolve address");
    }

    // create a socket for communication
    printf("socket\n");
    if ((sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
      throw std::runtime_error("failed to create socket");
    }

    printf("sock after %d\n", sock_fd);

    // connect to the provided host
    printf("connect\n");
    if (connect(sock_fd, res->ai_addr, res->ai_addrlen)) {
      close_socket(sock_fd);
      std::string tmp = std::string("failed to connect to ") + std::string(hostname) + std::string(":") + std::string(port);
      throw std::runtime_error(tmp.c_str());
    }

    unsigned long arg = 1;
    ioctlsocket(sock_fd, FIONBIO, &arg);

    create the ssl object
    printf("ssl_new\n");
    ssl = SSL_new(context);
    printf("setfd\n");
    SSL_set_fd(ssl, sock_fd);

    printf("sslsettlsexthostname\n");
    SSL_set_tlsext_host_name(ssl, hostname);
    /* Configure server hostname check */
    printf("ssl_set1_host\n");
    SSL_set1_host(ssl, hostname);
    int txlen;
    char* txbuf = NULL;
    size_t txcap = 0;
    int result;

    std::string user_input;

    /* Now do SSL connect with server */
    printf("ssl_connect\n");
    if (SSL_connect(ssl) == 1) {
      // temporary debug message
      LUNA_DEBUG("SSL connection to server successful\n");

    } else {
      LUNA_ERROR("SSL connection to server failed\n");
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("failed to connect with SSL");
    }
  }

  // catch any errors
  catch (const std::runtime_error& failure) {
    std::cerr << failure.what() << std::endl;
    return 1;
  }
  return 0;

}

void tcp_stream::StartStream(const char* hostname, const char* port)
{
    while(InitializeStream(hostname, port) != 0) {
    printf("Trying to connect again in 3s... \n");
    Sleep(3000);
  }
}

/* Tries to create a new tcp_stream by connecting to the
 * system designated by hostname and port
 */
tcp_stream::tcp_stream() 
{
  sock_fd = -1;
  ssl = NULL;
  context = NULL;
}

/* Destroy resources if necessary */
tcp_stream::~tcp_stream() {
    // close the socket
    if (sock_fd) {
        close_socket(sock_fd);
    }

    // free the ssl
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    // free the context
    if (context) {
        SSL_CTX_free(context);
    }
}

void tcp_stream::CloseStream() {

    free the ssl
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl = NULL;
    }

    // free the context
    if (context) {
        SSL_CTX_free(context);
        context = NULL;
    }

    // close the socket
    if (sock_fd) {
        close_socket(sock_fd);
        sock_fd = -1;
    }
}

/* send a character message */
void tcp_stream::send(const char* to_send) {
  try {
    if (SSL_write(ssl, &to_send, strlen(to_send)) <= 0) {
        throw std::runtime_error("Failed to send message to remote");
    }
    // if (::send(sock_fd, to_send, strlen(to_send), 0) <= 0) {
    //     throw std::runtime_error("Failed to send message to remote");
    // }
  } catch (const std::runtime_error& failure) {
    std::cerr << failure.what() << std::endl;
    exit(1);
  }
}

/* Send the GAMEPAD state */
void tcp_stream::send(const XINPUT_GAMEPAD& to_send) {
  try {
    if (SSL_write(ssl, &to_send, sizeof(XINPUT_STATE)) <= 0) {
        throw std::runtime_error("Failed to send gamepad state to remote");
    }
    // if (::send(sock_fd, (const char*) &to_send, sizeof(XINPUT_STATE), 0) <= 0) {
    //     throw std::runtime_error("Failed to send gamepad state to remote");
    // }
  } catch (const std::runtime_error& failure) {
    std::cerr << failure.what() << std::endl;
    exit(1);
  }
}

/* Receive a message (not really used) */
int tcp_stream::recieve(char* to_recieve, size_t len) {
  int size = -1;
  try {
    size = SSL_read(ssl, reinterpret_cast<char*>(to_recieve), len);
    if (size <= 0) {
        throw std::runtime_error("Failed to recieve from remote");
    }
    // size = ::recv(sock_fd, reinterpret_cast<char*>(to_recieve), len, 0);
    // if (size <= 0) {
    //     throw std::runtime_error("Failed to recieve from remote");
    // }
  } catch (const std::runtime_error& failure) {
    std::cerr << failure.what() << "len=" << len << std::endl;
    return -1;
  }

  return size;
};

void tcp_stream::SetNoBlocking() {
  unsigned long arg = 1;
  ioctlsocket(sock_fd, FIONBIO, &arg);
}

void tcp_stream::close_socket(SOCKET handle) { closesocket(handle); }