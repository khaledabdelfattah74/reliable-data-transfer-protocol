//
//  socket_manager.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "socket_manager.hpp"

ssize_t send_packet(int socket_fd, sockaddr_in client_addr, packet packet) {
    return ::sendto(socket_fd, &packet, sizeof(packet), MSG_CONFIRM,
                    (const struct sockaddr *) &client_addr, sizeof(client_addr));
}

void set_socket_timeout(int socket_fd, int seconds, int millis) {
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = millis;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }
}
