//
//  stop_and_wait.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/30/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "stop_and_wait.hpp"

StopAndWait::StopAndWait(int socket_fd, sockaddr_in client_addr, vector<packet*> packets) {
    this->socket_fd = socket_fd;
    this->client_addr = client_addr;
    this->packets = packets;
}

void StopAndWait::process() {
    set_socket_timeout(socket_fd, 5, 0);
    socklen_t client_len = sizeof(client_addr);

    for (packet* packet : packets) {
        excpected_ackno = packet->seqno;
        while (true) {
            struct packet packet_to_send = *packet;
            send_packet(socket_fd, client_addr, packet_to_send);
            ack_packet ack;
            ssize_t len = ::recvfrom(this->socket_fd, &ack, sizeof(ack), MSG_WAITALL,
                                     (struct sockaddr *) &client_addr, &client_len);
            printf("ackno: %d\n", ack.ackno);
            if (len > 0 && ack.ackno == excpected_ackno)
                break;
        }
    }
}
