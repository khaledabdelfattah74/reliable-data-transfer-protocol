//
//  packet.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef packet_hpp
#define packet_hpp

#include <stdio.h>
#include <iostream>
#include "constants.hpp"

using namespace std;

typedef struct packet {
    // Headers
    u_int16_t cksum;
    u_int16_t len;
    u_int32_t seqno;
    bool fin;
    // Data
    char data[PACKET_SIZE];
    
    packet() {}
    
    packet(char data[], int len, int seqno, bool fin) {
        copy(data, data + len, this->data);
        this->seqno = seqno;
        this->len = len;
        this->fin = fin;
    }
} packet;

typedef struct ack_packet {
    // Headers
    u_int16_t cksum;
    u_int16_t len;
    u_int32_t ackno;
    
    ack_packet() {}
    
    ack_packet(int ackno, int len) {
        this->ackno = ackno;
        this->len = len;
    }
} ack_packet;

packet* make_packet(char[], int, int, bool fin=false);
ack_packet* make_ack_packet(int, int len=8);

#endif /* packet_hpp */
