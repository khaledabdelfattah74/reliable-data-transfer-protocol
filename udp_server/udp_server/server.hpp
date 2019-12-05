//
//  server.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef server_hpp
#define server_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "constants.hpp"
#include "request_handler.hpp"

using namespace std;

class Server {
private:
    int socket_fd;
    int port_num=PORT;
    enum PROTOCOL protocol;
    char buffer[BUFFER_SIZE];
    
public:
    Server(int port_num=PORT, enum PROTOCOL protocol=STOP_AND_WAIT);
    void initiate();
};

#endif /* server_hpp */
