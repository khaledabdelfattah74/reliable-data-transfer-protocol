//
//  socket_manager.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef socket_manager_hpp
#define socket_manager_hpp

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

using namespace std;

ssize_t send_packet(int, sockaddr_in, packet);
void set_socket_timeout(int, int, int);

#endif /* socket_manager_hpp */
