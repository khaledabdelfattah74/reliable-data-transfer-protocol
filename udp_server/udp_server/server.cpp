//
//  server.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "server.hpp"

Server::Server(int port_num, double seed, double plp, enum PROTOCOL protocol) {
    this->port_num = port_num;
    this->seed = seed;
    this->plp = plp;
    this->protocol = protocol;
}

void Server::initiate() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(server_addr);
    memset(&server_addr, 0, addr_len);
    memset(&client_addr, 0, addr_len);
    
    // Socket creation
    this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->socket_fd < 0) {
        perror("Creation of socket failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = this->port_num;
    
    // Binding
    int binding_status = ::bind(this->socket_fd, (const struct sockaddr *) &server_addr, addr_len);
    if (binding_status < 0) {
        perror("Error in binding on the desired socket");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = getpid();
    socklen_t client_len = sizeof(client_addr);
    
    while (pid) {
        packet request;
        ssize_t len = ::recvfrom(this->socket_fd, &request, sizeof(request), MSG_WAITALL,
                   (struct sockaddr *) &client_addr, &client_len);
        if (len < 0 || !request.check())
            continue;
        printf("Client msg: %s, length: %zd\n", request.data, len);
        ack_packet ack = *make_ack_packet(0);
        // Send ack
        ::sendto(this->socket_fd, &ack, sizeof(ack), MSG_CONFIRM,
                 (const struct sockaddr *) &client_addr, client_len);
        
        pid = fork();
        if (pid == 0) {
            RequestHandler* request_handler = new RequestHandler(request, client_addr, seed, plp);
            request_handler->handle();
        }
    }
    printf("One of the clients processed\n");
}
