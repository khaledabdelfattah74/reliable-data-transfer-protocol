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
#include "packet.hpp"
#include "socket_manager.hpp"
#include "../../headers/timer.hpp"

using namespace std;

class SelectiveRepeat {
private:
    int socket_fd;
    sockaddr_in client_addr;
    vector<packet*> packets;
    int window_size = 10;
    u_int32_t send_base = 1;
    u_int32_t next_seqno = 1;
    unordered_map<u_int32_t, int> acks;
    mutex sender_mtx;
    mutex timer_mtx;
    u_int32_t num_of_acks = 0;
    Timer timer;
    ssize_t send(packet);
    void receive();
    void set_timer();
    void handle_timeout(u_int32_t);
    
public:
    SelectiveRepeat(int, sockaddr_in, vector<packet*>);
    void set_cnwd(int);
    void process();
};

#endif /* selective_repeat_hpp */
