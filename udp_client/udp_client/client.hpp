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
#include "file_buffer.hpp"
#include "../../udp_server/udp_server/constants.hpp"
#include "../../udp_server/udp_server/packet.hpp"
#include "../../udp_server/udp_server/socket_manager.hpp"

using namespace std;

class Client {
private:
    int socket_fd;
    struct sockaddr_in server_addr;
    int port_num=PORT;
    char buffer[BUFFER_SIZE];
    string path;
    FileBuffer* file_buffer;
    bool send_request_packet();
    void send_duplicate_acks(u_int32_t);
    
public:
    Client(string path, string server_ip="localhost", int port_num=PORT);
    void initiate();
};

#endif /* client_hpp */
