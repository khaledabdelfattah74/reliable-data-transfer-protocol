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
    ssize_t status = 0;
    if (!acks.count(pckt.seqno)) {
        status = send_packet(socket_fd, client_addr, pckt);
        timer_monitor(pckt.seqno, true);
    }
    return status;
}

void SelectiveRepeat::add_packet_to_queue(packet* packet) {
    sender_mtx.lock();
    if (state == FAST_RECOVERY)
        sender_queue.push_front(packet);
    else
        sender_queue.push_back(packet);
    in_queue.insert(packet->seqno);
    sender_mtx.unlock();
}

void SelectiveRepeat::sender_function() {
    while (num_of_acks < packets.size()) {
        while (!sender_queue.empty()) {
            packet* pckt = sender_queue.front();
            sender_queue.pop_front();
            in_queue.erase(pckt->seqno);
            send(*pckt);
        }
    }
}

void SelectiveRepeat::process() {
    thread receiver(&SelectiveRepeat::receive, this);
    receiver.detach();
    
    thread timer_watch_dog(&SelectiveRepeat::watch_timer, this);
    timer_watch_dog.detach();
    
    thread sender(&SelectiveRepeat::sender_function, this);
    sender.detach();
    
    while (num_of_acks < packets.size()) {
        while (next_seqno < packets.size() + 1 &&
               next_seqno < send_base + window_size) {
            if (!packet_manager.to_be_dropped() || next_seqno == 1)
                add_packet_to_queue(packets[next_seqno - 1]);
            else {
                printf("#### Packet loss: %d\n", next_seqno);
                timer_monitor(next_seqno, true);
            }
            next_seqno++;
        }
    }
//    receiver.join();
//    timer_watch_dog.join();
//    sender.join();
    printf("Terminated because of what??? %d\n", num_of_acks);
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
            if (!acks.count(seqno))
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
        auto elapsed_time = chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - timers[seqno]).count();
        if (elapsed_time >= 300 && !acks.count(seqno) && !in_queue.count(seqno))
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
    window_size = 128;
    state = SLOW_START;
    add_packet_to_queue(packets[seqno - 1]);
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
    add_packet_to_queue(packets[ackno]);
}
