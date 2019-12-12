//
//  request_handler.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef request_handler_hpp
#define request_handler_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "packet.hpp"
#include "file_handler.hpp"
//#include "stop_and_wait.hpp"
#include "selective_repeat.hpp"

using namespace std;

class RequestHandler {
private:
    int port_num;
    int req_sock_fd;
    packet request;
    sockaddr_in client_addr;
    sockaddr_in req_addr;
    double seed;
    double plp;
    
public:
    RequestHandler(packet, sockaddr_in, double, double, int);
    void handle(enum PROTOCOL protocol=SELECTIVE_REPEAT);
};

#endif /* request_handler_hpp */
