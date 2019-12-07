//
//  main.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include "client.hpp"
#include "../../headers/file_reader.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    // Get client attributes
    string root_path = "/Users/khaledabdelfattah/Documents/workspace/networks/reliable-data-transport-protocol/udp_client/udp_client/";
    string in_file_path = root_path + "client.in";
    vector<string> lines = read_from_file(in_file_path);
    string port_ip = lines[0];
    int port_num = atoi(lines[1].c_str());
    string file_path = lines[2];
    
    Client* client = new Client(file_path, port_ip, port_num);
    client->initiate();
    return 0;
}
