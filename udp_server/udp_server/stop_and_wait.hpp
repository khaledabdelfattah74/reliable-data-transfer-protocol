//
//  stop_and_wait.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/30/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef stop_and_wait_hpp
#define stop_and_wait_hpp

#include <stdio.h>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "packet.hpp"
#include "socket_manager.hpp"

using namespace std;

class StopAndWait {
private:
    int socket_fd;
    sockaddr_in client_addr;
    vector<packet*> packets;
    u_int32_t excpected_ackno;
    
public:
    StopAndWait(int, sockaddr_in, vector<packet*>);
    void process();
};

#endif /* stop_and_wait_hpp */
