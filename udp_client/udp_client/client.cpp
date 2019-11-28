//
//  client.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "client.hpp"

Client::Client(int port_num) {
    this->port_num = port_num;
}

void Client::initiate() {
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    memset(&server_addr, 0, addr_len);
    
    // Creating UDP socket
    this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->socket_fd < 0) {
        perror("Creation of socket failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = this->port_num;
    
    char msg[] = "Hello from client\n";
    ::sendto(this->socket_fd, (char *) msg, strlen(msg), MSG_CONFIRM,
             (const struct sockaddr *) &server_addr, addr_len);
    printf("Client ba3at\n");
    ::recvfrom(this->socket_fd, (char *) this->buffer, BUFFER_SIZE, MSG_WAITALL,
               (struct sockaddr *) &server_addr, &addr_len);
    printf("Server msg: %s\n", buffer);
}
