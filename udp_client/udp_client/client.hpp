//
//  client.hpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef client_hpp
#define client_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../../udp_server/udp_server/constants.hpp"

using namespace std;

class Client {
private:
    int socket_fd;
    int port_num=PORT;
    char buffer[BUFFER_SIZE];
    
public:
    Client(int port_num=PORT);
    void initiate();
};

#endif /* client_hpp */
