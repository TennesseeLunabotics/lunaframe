#include "../include/comm.hpp"

/* Sets default values and ensures buffer is all 0's */
COM::COM() {
    server_sock = -1;
    client_socket = -1;
    valread = -1;
    opt = 1;
    addrlen = sizeof(address);
    has_client = false;
    memset(buffer, 0, 1024);
}

/* Shutsdown the server and closes socket if necessary, frees ssl structs*/
COM::~COM() {
    HandleDisconnect();
    //close(server_sock);
    //server_sock = -1;
    destruct_ssl();
    if(server_sock != -1) shutdown(server_sock, SHUT_RDWR);
    close(epoll_fd);
    if(client_socket != -1) close(client_socket);
}

void COM::SendACK() {
    char tmp = '0';
    valread = SSL_write(ssl, &tmp, sizeof(tmp));
    // valread = write(client_socket, &tmp, sizeof(tmp));;

    if (valread <= 0) {
        printf("ACK Fail\n");
        HandleDisconnect();
    }
}

/* Makes the server ready for client connection */
void COM::StartServer(int port) {
    create_context();

    printf("sock=%d\n", server_sock);

    // create our socket
    LUNA_DEBUG("Creating socket\n");
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set the socket options (let the socket ip and port be reused)
    LUNA_DEBUG("Setting socket options\n");
    printf("sock after=%d\n", server_sock);
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // set the address info
    LUNA_DEBUG("Setting port\n");
    address.sin_family = AF_INET;    // specify IPv4
    address.sin_port = htons(port);  // convert port from number to network order

    // this allows us to recieve a connection from any netdev device
    // basically if you had a physical and wireless connection a client
    // would be able to connect to us through either of them
    address.sin_addr.s_addr = INADDR_ANY;

    // bind our socket to a physical address/port
    LUNA_DEBUG("Binding Socket\n");
    if (bind(server_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    LUNA_DEBUG("Server has started\n\n");
}

/* Waits for a client to connect */
void COM::AwaitClient() {
    LUNA_DEBUG("Waiting for client\n\n");

    // allows clients to queue up to be accepted
    // currently allows 3 clients to queue
    // (doesn't really matter cause there is only 1 connecting)
    if (listen(server_sock, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accepts a client
    LUNA_DEBUG("Waiting on accept\n");
    if ((client_socket = accept(server_sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    if (epoll_fd != -5) {
        close(epoll_fd); 
        epoll_fd = -5;
    }
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    event = {.events = EPOLLIN, .data.fd = client_socket};
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event)) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    create_ssl();
    SSL_set_fd(ssl, client_socket);

    // Accept the SSL connection
    LUNA_DEBUG("Waiting to SSL accept\n");
    if (SSL_accept(ssl) <= 0) {
        // Error occurred, log and close down ssl
        LUNA_ERROR("SSL_ACCEPT ERROR\n");
        perror("test");
        HandleDisconnect();
        return;
    }

    LUNA_DEBUG("Client connected\n\n");
    has_client = true;
}

/* Reads a string message from the client */
void COM::RecieveMessage() {
    valread = SSL_read(ssl, buffer, 1024);
    // valread = read(client_socket, buffer, 1024);

    // just to give a little visibility that this failed
    LUNA_DEBUG("VALREAD %d\n", valread);

    // if it fails we handle disconnect (no client if it failed)
    if (valread <= 0) HandleDisconnect();
}

int COM::SendMessage(char *buff, size_t buff_size) {
    //int bytes_written = SSL_write(ssl, buff, buff_size);
    int bytes_written = write(client_socket, buff, buff_size);

    LUNA_DEBUG("BYTES_WRITTEN %d\n", bytes_written);

    if (bytes_written <= 0) {
        HandleDisconnect();
        return -1;
    }
    return 0;
}

int COM::SendImage(std::vector<unsigned char> &buff) {
    char *handle = (char *)buff.data();
    int size = (int)buff.size();

    int bytes_written = SSL_write(ssl, handle, size);
    // int bytes_written = write(client_socket, handle, size);

    LUNA_DEBUG("BYTES_WRITTEN %d\n", bytes_written);

    if (bytes_written <= 0) {
        HandleDisconnect();
        return -1;
    }
    return 0;
}

/* Returns if a client is connected */
bool COM::HasClient() {
    return has_client;
}

/* Prints the buffer out in string format */
void COM::PrintMessage() {
    LUNA_DEBUG("PrintMessage: %s\n", buffer);
}

/* Handle the client dropping connection */
void COM::HandleDisconnect() {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(context);
    printf("closing socket\n");
    close(client_socket);
    close(server_sock);
    server_sock = -1;
    client_socket = -1;
    has_client = false;
    ssl = NULL;
    context = NULL;
}

/* Reads the controller data from the client
 *
 * Returns success/failure
 */
bool COM::GetControllerData(GAMEPAD &g, NANO* n = NULL) {
    valread = 0;
    int ec = 0;
    while (valread < sizeof(GAMEPAD)) {
        ec = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (ec == 0) {
            if (n) n->SetMotorsToZeroAndSend();
            continue;
        } else if (ec == -1) {
            perror("epoll_wait");
            continue;
        }
        valread = SSL_read(ssl, &g, sizeof(GAMEPAD));
        // valread = read(client_socket, &g, sizeof(GAMEPAD));

        // if failure clear out the gamepad struct
        // and handle the disconnect
        if (valread <= 0) {
            memset(&g, 0, sizeof(GAMEPAD));
            HandleDisconnect();
            return false;
        }
    }

    printf("Valread baby %d\n", valread);
    // if failure clear out the gamepad struct
    // and handle the disconnect
    if(valread <= 0) {
      memset(&g, 0, sizeof(GAMEPAD));
      HandleDisconnect();
      return false;
    }

    return true;
}

void COM::create_context() {
    method = TLS_server_method();

    context = SSL_CTX_new(method);
    if (!context) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    configure_context();
}

void COM::configure_context() {
    // Set the key and cert
    if (SSL_CTX_use_certificate_chain_file(context, "/home/lunabotlunabot/cert.pem") <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(context, "/home/lunabotlunabot/private.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void COM::create_ssl() {
    ssl = SSL_new(context);
}

// Shutdown and free structs
void COM::destruct_ssl() {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(context);
}
