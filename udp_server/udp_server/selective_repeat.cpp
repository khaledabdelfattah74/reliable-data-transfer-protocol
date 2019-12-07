//
//  selective_repeat.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 12/4/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "selective_repeat.hpp"

SelectiveRepeat::SelectiveRepeat(int socket_fd, sockaddr_in client_addr, vector<packet*> packets,
                                 double seed, double plp) {
    this->socket_fd = socket_fd;
    this->client_addr = client_addr;
    this->packets = packets;
    this->packet_manager = PacketLossManager(plp, seed);
    this->timeout_interval = chrono::milliseconds(300);
    set_socket_timeout(socket_fd, 0, 500);
}

void SelectiveRepeat::update_congestion_attr(int window_size, int ssthresh, enum STATE state) {
    congestion_mtx.lock();
    this->window_size = window_size;
    this->ssthresh = ssthresh;
    this->state = state;
    trans_round++;
    string tuple = to_string(trans_round) + ", " + to_string(window_size) + "\n";
    file.write(tuple.c_str(), tuple.length());
    congestion_mtx.unlock();
}

ssize_t SelectiveRepeat::send(packet pckt) {
    sender_mtx.lock();
    ssize_t status = 0;
    status = send_packet(socket_fd, client_addr, pckt);
    sender_mtx.unlock();
    return status;
}

void SelectiveRepeat::add_packet_to_queue(packet* packet) {
    sender_mtx.lock();
    if (state == FAST_RECOVERY)
        sender_queue.push_front(packet);
    else
        sender_queue.push_back(packet);
//    in_queue.insert(packet->seqno);
    sender_mtx.unlock();
}

void SelectiveRepeat::sender_function() {
    while (num_of_acks < packets.size()) {
        while (!sender_queue.empty()) {
            packet* pckt = sender_queue.front();
            send(*pckt);
            sender_queue.pop_front();
//            in_queue.erase(pckt->seqno);
        }
    }
}

void SelectiveRepeat::process() {
    thread receiver(&SelectiveRepeat::receive, this);
    receiver.detach();
    
    thread timer_watch_dog(&SelectiveRepeat::watch_timer, this);
    timer_watch_dog.detach();
    
//    thread sender(&SelectiveRepeat::sender_function, this);
//    sender.detach();
    
    string path = "/Users/khaledabdelfattah/Documents/workspace/networks/reliable-data-transport-protocol/udp_server/udp_server/con-control-graph.csv";
    file.open(path, ios::out | ios::binary);
    string headers = "Transimission Round, Congestion window\n";
    file.write(headers.c_str(), headers.length());
    string tuple = to_string(trans_round) + ", " + to_string(window_size) + "\n";
    file.write(tuple.c_str(), tuple.length());

    
    while (num_of_acks < packets.size()) {
        while (next_seqno < packets.size() + 1 &&
               next_seqno < send_base + window_size) {
            if (!packet_manager.to_be_dropped() || next_seqno == 1)
                send(*packets[next_seqno - 1]);
            else {
                printf("#### Packet loss: %d\n", next_seqno);
            }
            timer_monitor(next_seqno, true);
            next_seqno++;
        }
    }
    printf("Terminated because of what??? %d\n", num_of_acks);
    file.close();
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
        // Erase timer for packet with ackno = ack.ackno
        timer_monitor(ack.ackno, 2);
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
        auto elapsed_time = chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - timers[seqno]).count();
        if (elapsed_time >= 300)
            handle_timeout(seqno);
    }
}

void SelectiveRepeat::timer_monitor(u_int32_t seqno, int action) {
    timer_mtx.lock();
    if (action == 1) {
        set_timer(seqno);
    } else if (action == 2) {
        if (timers.count(seqno))
            timers.erase(seqno);
    } else {
        check_timeout(seqno);
    }
    timer_mtx.unlock();
}

void SelectiveRepeat::handle_timeout(u_int32_t seqno) {
    printf("***** Timeout packet: %d\n", seqno);
    int new_ssthresh = max(window_size / 2, 8);
    int new_window_size = 1;
    enum STATE new_state = SLOW_START;
    update_congestion_attr(new_window_size, new_ssthresh, new_state);
    send(*packets[seqno - 1]);
    set_timer(seqno);
}

void SelectiveRepeat::update_window_size() {
    enum STATE new_state = state;
    int new_window_size = window_size;
    switch (state) {
        case SLOW_START:
            new_window_size = window_size * 2;
            if (new_window_size >= ssthresh)
                new_state = CONGESTION_AVOIDANCE;
            break;
        case CONGESTION_AVOIDANCE:
            new_window_size = window_size + 1;
            break;
        case FAST_RECOVERY:
            new_window_size = ssthresh;
            new_state = CONGESTION_AVOIDANCE;
            break;
        default:
            break;
    }
    update_congestion_attr(new_window_size, ssthresh, new_state);
}

void SelectiveRepeat::handle_fast_recovery(u_int32_t ackno) {
    printf("****** Entering fast recovery state: %d\n", ackno);
    int new_ssthresh = max(window_size / 2, 8);
    int new_window_size = new_ssthresh + 3;
    update_congestion_attr(new_window_size, new_ssthresh, FAST_RECOVERY);
    send(*packets[ackno]);
    timer_monitor(next_seqno, 1);
}
