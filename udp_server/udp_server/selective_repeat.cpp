//
//  selective_repeat.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 12/4/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "selective_repeat.hpp"

SelectiveRepeat::SelectiveRepeat(int socket_fd, sockaddr_in client_addr, vector<packet*> packets,
                                 double plp, double seed) {
    this->socket_fd = socket_fd;
    this->client_addr = client_addr;
    this->packets = packets;
    this->packet_manager = PacketLossManager(plp, seed);
    this->timeout_interval = chrono::milliseconds(300);
}

void SelectiveRepeat::set_cnwd(int window_size) {
    this->window_size = window_size;
}

ssize_t SelectiveRepeat::send(packet pckt) {
//    sender_mtx.lock();
    ssize_t status = 0;
    if (!acks.count(pckt.seqno)) {
        status = send_packet(socket_fd, client_addr, pckt);
//        timer_monitor(pckt.seqno, true);
        set_timer(pckt.seqno);
    }
//    sender_mtx.unlock();
    return status;
}

void SelectiveRepeat::process() {
    thread receiver(&SelectiveRepeat::receive, this);
    receiver.detach();
    
    thread timer_watch_dog(&SelectiveRepeat::watch_timer, this);
    timer_watch_dog.detach();
    
    while (num_of_acks < packets.size()) {
        while (next_seqno < packets.size() + 1 &&
               next_seqno < send_base + window_size) {
            if (!packet_manager.to_be_dropped() || next_seqno == 1)
                send(*packets[next_seqno - 1]);
            else {
                timer_monitor(next_seqno, true);
//                printf("#### Packet loss: %d\n", next_seqno);
                set_timer(next_seqno);
            }
            next_seqno++;
        }
    }
}

void SelectiveRepeat::receive() {
    socklen_t client_len = sizeof(client_addr);
    while (num_of_acks < packets.size()) {
        ack_packet ack;
        ssize_t len = ::recvfrom(socket_fd, &ack, sizeof(ack), MSG_WAITALL,
                                 (struct sockaddr *) &client_addr, &client_len);
        if (len < 0)
            continue;
        printf("Recived ackno: %d\n", ack.ackno);

        // New Ack
        if (!acks.count(ack.ackno)) {
            acks[ack.ackno] = 1;
            num_of_acks++;
            update_window_size();
        } else {
        // Fast retransmit
            acks[ack.ackno]++;
            if (acks[ack.ackno] >= 3) {
                handle_fast_recovery(ack.ackno);
                acks[ack.ackno] = 1;
            }
        }
        // Reset timer
        if (ack.ackno == send_base) {
            while (send_base < packets.size() + 1 && acks.count(send_base))
                send_base++;
        }
    }
}

void SelectiveRepeat::watch_timer() {
    while (num_of_acks < packets.size()) {
        for (int seqno = send_base; seqno < send_base + window_size &&
             seqno < packets.size() + 1; seqno++) {
            timer_monitor(seqno, false);
        }
    }
}

void SelectiveRepeat::set_timer(u_int32_t seqno) {
//    printf("Setting timer of: %d\n", seqno);
    timers[seqno] = chrono::high_resolution_clock::now();
}

void SelectiveRepeat::check_timeout(u_int32_t seqno) {
    if (timers.count(seqno)) {
        auto elapsed_time = (chrono::high_resolution_clock::now() - timers[seqno]);
        if (elapsed_time >= timeout_interval && !acks.count(seqno))
            handle_timeout(seqno);
    }
}

void SelectiveRepeat::timer_monitor(u_int32_t seqno, bool action) {
    timer_mtx.lock();
    if (action) {
        set_timer(seqno);
    } else {
        check_timeout(seqno);
    }
    timer_mtx.unlock();
}

void SelectiveRepeat::handle_timeout(u_int32_t seqno) {
    printf("***** Timeout packet: %d\n", seqno);
    ssthresh = max(window_size / 2, 8);
    window_size = 1;
    state = SLOW_START;
    send(*packets[seqno - 1]);
}

void SelectiveRepeat::update_window_size() {
    switch (state) {
        case SLOW_START:
            window_size *= 2;
            if (window_size >= ssthresh)
                state = CONGESTION_AVOIDANCE;
            break;
        case CONGESTION_AVOIDANCE:
            window_size++;
            break;
        case FAST_RECOVERY:
            window_size = ssthresh;
            state = CONGESTION_AVOIDANCE;
            break;
        default:
            break;
    }
}

void SelectiveRepeat::handle_fast_recovery(u_int32_t ackno) {
    printf("****** Entering fast recovery state: %d\n", ackno);
    ssthresh = max(window_size / 2, 8);
    window_size = ssthresh + 3;
    state = FAST_RECOVERY;
    send(*packets[ackno]);
}
