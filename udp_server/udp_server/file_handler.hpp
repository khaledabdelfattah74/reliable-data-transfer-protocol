//
//  file_handler.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef file_handler_hpp
#define file_handler_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include "packet.hpp"

using namespace std;

class FileHandler {
private:
    string path;
    ifstream file;
    long long int num_of_chunks;
    u_int32_t cur_chunk_idx = 0;
    long long int cur_len = 0;
    long long int file_len;
    int last_chunck_len;
    int current_chunk_len;
    void set_attributes();
    packet* get_fin_packet();
    
public:
    FileHandler(string path);
    bool open_file();
    bool has_next_chunck();
    packet* get_next_chunck();
    vector<packet*> get_chunks();
};

#endif /* file_handler_hpp */
