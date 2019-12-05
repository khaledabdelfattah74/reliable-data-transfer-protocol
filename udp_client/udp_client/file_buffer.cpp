//
//  file_buffer.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 12/1/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "file_buffer.hpp"

FileBuffer::FileBuffer(string path) {
    this->path = path;
    this->open_file();
}

void FileBuffer::set_next_seqno(u_int32_t next_seqno) {
    this->next_seqno = next_seqno;
}

bool FileBuffer::open_file() {
    this->file.open(path, ios::out | ios::binary);
    if (this->file)
        return true;
    return false;
}

void FileBuffer::write_packet(packet packet) {
    file.write(packet.data, packet.len);
}

bool FileBuffer::has_packet(u_int32_t seqno) {
    return packets.count(seqno);
}

u_int32_t FileBuffer::get_next_seqno() {
    return next_seqno;
}

enum BUFFER_STATUS FileBuffer::add_packet(packet packet) {
    if (packet.seqno == next_seqno) {
        packets[packet.seqno] = packet;
        while (has_packet(next_seqno)) {
            struct packet cur_packet = packets[next_seqno];
            if (cur_packet.fin) {
                file.close();
                return FINISH;
            }
            write_packet(cur_packet);
            next_seqno++;
        }
        num_of_unordered_packets = 0;
    } else if (packet.seqno > next_seqno) {
        packets[packet.seqno] = packet;
        num_of_unordered_packets++;
    } else {
        // No action
    }
    if (num_of_unordered_packets == 3)
        return DUP_ACKS;
    return CONTINUE;
}
