//
//  packet.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "packet.hpp"

packet* make_packet(char data[], int len, int seqno, bool fin) {
    return new packet(data, len, seqno, fin);
}

ack_packet* make_ack_packet(int ackno, int len) {
    return new ack_packet(ackno, len);
}
