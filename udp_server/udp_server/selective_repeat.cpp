//
//  selective_repeat.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 12/4/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "selective_repeat.hpp"

SelectiveRepeat::SelectiveRepeat(int socket_fd, sockaddr_in client_addr, vector<packet*> packets) {
    this->socket_fd = socket_fd;
    this->client_addr = client_addr;
    this->packets = packets;
    this->timer = Timer();
}

void SelectiveRepeat::set_cnwd(int window_size) {
    this->window_size = window_size;
}

ssize_t SelectiveRepeat::send(packet pckt) {
    sender_mtx.lock();
    ssize_t status = send_packet(socket_fd, client_addr, pckt);
    sender_mtx.unlock();
    return status;
}

void SelectiveRepeat::process() {
    thread receiver(&SelectiveRepeat::receive, this);
    receiver.detach();
    
    set_timer();
    while (num_of_acks < packets.size()) {
        while (next_seqno < packets.size() + 1 &&
               next_seqno < send_base + window_size) {
//            if (next_seqno != 2)
            send(*packets[next_seqno - 1]);
            next_seqno++;
        }
    }
}

void SelectiveRepeat::receive() {
    printf("Ana hnaaaaa\n");
    socklen_t client_len = sizeof(client_addr);
    while (num_of_acks < packets.size()) {
        ack_packet ack;
        ssize_t len = ::recvfrom(socket_fd, &ack, sizeof(ack), MSG_WAITALL,
                                 (struct sockaddr *) &client_addr, &client_len);
        if (len < 0)
            continue;
        printf("Recived ackno: %d\n", ack.ackno);
        int& acks_num = acks[ack.ackno];
        acks_num++;
        num_of_acks += (acks_num == 1);
        // Fast retransmit
        if (acks_num >= 3)
            send(*packets[ack.ackno - 1]);
        if (ack.ackno == send_base) {
            send_base++;
            set_timer();
        }
    }
}

void SelectiveRepeat::set_timer() {
    timer_mtx.lock();
    printf("Setting timer of: %d\n", send_base);
    timer.setTimeout([&]() {
        printf("***** Timeout packet: %d\n", send_base);
        this->handle_timeout(send_base);
    }, 300);
    timer_mtx.unlock();
}

void SelectiveRepeat::handle_timeout(u_int32_t seqno) {
    send(*packets[seqno - 1]);
}
