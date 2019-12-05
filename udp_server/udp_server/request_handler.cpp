//
//  request_handler.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "request_handler.hpp"

RequestHandler::RequestHandler(packet request, sockaddr_in client_addr) {
    this->request = request;
    this->client_addr = client_addr;
}

void RequestHandler::handle(enum PROTOCOL protocol) {
    req_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (req_sock_fd < 0) {
        perror("Creation of socket failed");
        exit(EXIT_FAILURE);
    }
    
    req_addr = {};
    req_addr.sin_port = 0;
    req_addr.sin_family = AF_INET;
    req_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int binding_status = ::bind(req_sock_fd, (const struct sockaddr *) &req_addr, sizeof(req_addr));
    if (binding_status < 0) {
        perror("Error in binding on the desired socket");
        exit(EXIT_FAILURE);
    }
    
    // TODO Add protocols
    string path = "/Users/khaledabdelfattah/Documents/workspace/networks/reliable-data-transport-protocol/udp_server/udp_server/public/info.txt";
    FileHandler* handler = new FileHandler(path);
    if (handler->open_file()) {
        vector<packet*> packts = handler->get_chunks();
        printf("packets #: %d\n", (int) packts.size());

        if (protocol == STOP_AND_WAIT) {
//            StopAndWait* processor = new StopAndWait(req_sock_fd, client_addr, packts);
//            processor->process();
        } else if (protocol == SELECTIVE_REPEAT) {
            SelectiveRepeat* processor = new SelectiveRepeat(req_sock_fd, client_addr, packts);
            processor->process();
        }
    }
}
