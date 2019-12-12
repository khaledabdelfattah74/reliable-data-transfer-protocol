//
//  selective_repeat.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 12/4/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef selective_repeat_hpp
#define selective_repeat_hpp

#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <thread>
#include <random>
#include <chrono>
#include <deque>
#include <unordered_set>
#include <fstream>
#include "packet.hpp"
#include "socket_manager.hpp"
#include "../../headers/timer.hpp"
#include "packet_loss_simulator.hpp"

using namespace std;

enum STATE {
    SLOW_START,
    CONGESTION_AVOIDANCE,
    FAST_RECOVERY
};

enum TIMER_ACTION {
    SET_TIMER,
    REMOVE_TIMER,
    CHECK_TIMER
};

class SelectiveRepeat {
private:
    int socket_fd;
    sockaddr_in client_addr;
    vector<packet*> packets;
    int window_size = 1;
    int ssthresh = 128;
    u_int32_t send_base = 1;
    u_int32_t next_seqno = 1;
    unordered_map<u_int32_t, int> acks;
    u_int32_t num_of_acks = 0;
    enum STATE state = SLOW_START;
    PacketLossSimulator packet_manager;
    unordered_map<int, chrono::high_resolution_clock::time_point> timers;
    ofstream file;
    long long trans_round = 1;
    mutex sender_mtx;
    mutex timer_mtx;
    mutex congestion_mtx;
    
    ssize_t send(packet);
    void receive();
    void watch_timer();
    void set_timer(u_int32_t);
    void check_timeout(u_int32_t);
    void remove_timer(u_int32_t);
    void access_timers(u_int32_t, enum TIMER_ACTION);
    void handle_timeout(u_int32_t);
    void handle_new_ack(u_int32_t);
    void handle_fast_recovery(u_int32_t);
    void update_congestion_attr(int, int, enum STATE);
    
    
public:
    SelectiveRepeat(int, sockaddr_in, vector<packet*>, double, double);
    void process();
};

#endif /* selective_repeat_hpp */
