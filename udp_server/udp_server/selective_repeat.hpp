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
#include "packet.hpp"
#include "socket_manager.hpp"
#include "../../headers/timer.hpp"
#include <fstream>

using namespace std;

enum STATE {
    SLOW_START,
    CONGESTION_AVOIDANCE,
    FAST_RECOVERY
};

class PacketLossManager {
    double plp;
    double seed;
    default_random_engine engine;
    uniform_real_distribution<> distribution;
    
public:
    PacketLossManager() {}
    
    PacketLossManager(double plp, double seed) {
        this->plp = plp;
        this->seed = seed;
        this->engine = default_random_engine((int) time(0));
        this->distribution = uniform_real_distribution<>(0.0f, 1.0f);
    }
    
    bool to_be_dropped() {
        return (distribution(engine) <= plp);
    }
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
    mutex sender_mtx;
    mutex timer_mtx;
    mutex congestion_mtx;
    u_int32_t num_of_acks = 0;
    Timer timer;
    ssize_t send(packet);
    enum STATE state = SLOW_START;
    PacketLossManager packet_manager;
    unordered_map<int, chrono::high_resolution_clock::time_point> timers;
    unordered_set<int> in_queue;
    chrono::milliseconds timeout_interval;
    deque<packet*> sender_queue;
    void sender_function();
    void receive();
    void watch_timer();
    void set_timer(u_int32_t);
    void check_timeout(u_int32_t);
    void timer_monitor(u_int32_t, int);
    void add_packet_to_queue(packet*);
    void handle_timeout(u_int32_t);
    void update_window_size();
    void handle_fast_recovery(u_int32_t);
    void update_congestion_attr(int, int, enum STATE);
    ofstream file;
    long long trans_round = 1;
    
public:
    SelectiveRepeat(int, sockaddr_in, vector<packet*>, double, double);
    void process();
};

#endif /* selective_repeat_hpp */
