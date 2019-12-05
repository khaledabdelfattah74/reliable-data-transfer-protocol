//
//  file_buffer.hpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 12/1/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef file_buffer_hpp
#define file_buffer_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include "../../udp_server/udp_server/packet.hpp"

using namespace std;

enum BUFFER_STATUS {
    CONTINUE,
    FINISH,
    DUP_ACKS
};

class FileBuffer {
private:
    string path;
    ofstream file;
    u_int32_t next_seqno;
    unordered_map<u_int32_t, packet> packets;
    bool open_file();
    void write_packet(packet);
    int num_of_unordered_packets = 0;
    
public:
    FileBuffer(string);
    enum BUFFER_STATUS add_packet(packet);
    bool has_packet(u_int32_t);
    void set_next_seqno(u_int32_t);
    u_int32_t get_next_seqno();
};

#endif /* file_buffer_hpp */
