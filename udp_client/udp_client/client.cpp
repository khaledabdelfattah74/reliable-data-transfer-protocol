//
//  client.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "client.hpp"

Client::Client(string path, int port_num) {
    this->port_num = port_num;
    this->path = path;
    file_buffer = new FileBuffer(path);
}

void Client::initiate() {
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
    
    file_buffer->set_next_seqno(1);
    while (true) {
        packet pckt;
        ssize_t len = ::recvfrom(this->socket_fd, &pckt, sizeof(pckt), MSG_WAITALL,
                   (struct sockaddr *) &server_addr, &addr_len);
        printf("data: %d\nfin: %d\n", pckt.seqno, pckt.fin);

        if (len < 0)
            continue;
        
        ack_packet ack = *make_ack_packet(pckt.seqno);
        ::sendto(this->socket_fd, &ack, sizeof(ack), MSG_CONFIRM,
                 (const struct sockaddr *) &server_addr, addr_len);
        
        enum BUFFER_STATUS buffer_status = file_buffer->add_packet(pckt);
        
        if (buffer_status == FINISH)
            break;
        else if (buffer_status == DUP_ACKS)
            send_duplicate_acks(file_buffer->get_next_seqno());
    }
    close(socket_fd);
}

void Client::send_duplicate_acks(u_int32_t seqno) {
    socklen_t addr_len = sizeof(server_addr);
    ack_packet ack = *make_ack_packet(seqno - 1);
    for (int i = 0; i < 3; i++)
        ::sendto(this->socket_fd, &ack, sizeof(ack), MSG_CONFIRM,
                 (const struct sockaddr *) &server_addr, addr_len);
}
