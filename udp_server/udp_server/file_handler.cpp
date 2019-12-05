//
//  file_handler.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/29/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "file_handler.hpp"

FileHandler::FileHandler(string path) {
    this->path = path;
}

void FileHandler::set_attributes() {
    file_len = file.tellg();
    num_of_chunks = (ceil((double) file_len / PACKET_SIZE));
    cur_chunk_idx = 0;
    last_chunck_len = file_len % PACKET_SIZE;
    file.seekg(0, ios::beg);
}

bool FileHandler::open_file() {
    file.open(path, ios::in | ios::binary | ios::ate);
    if (!file) {
        return false;
    } else {
        this->set_attributes();
        return true;
    }
}

bool FileHandler::has_next_chunck() {
    if (cur_chunk_idx < num_of_chunks) {
        if (cur_chunk_idx == num_of_chunks - 1)
            current_chunk_len = last_chunck_len;
        else
            current_chunk_len = PACKET_SIZE;
        cur_chunk_idx++;
        return true;
    } else {
        return false;
    }
}

packet* FileHandler::get_next_chunck() {
    char data[current_chunk_len];
    file.read(data, current_chunk_len);
    return make_packet(data, current_chunk_len, cur_chunk_idx);
}

packet* FileHandler::get_fin_packet() {
    char fin_data[] = "";
    return make_packet(fin_data, 1, cur_chunk_idx + 1, true);
}

vector<packet*> FileHandler::get_chunks() {
    vector<packet*> chunks;
    while (has_next_chunck()) {
        chunks.push_back(get_next_chunck());
    }
    // Add fin packet
    chunks.push_back(get_fin_packet());
    return chunks;
}
