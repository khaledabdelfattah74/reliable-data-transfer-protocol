//
//  main.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include "server.hpp"
#include "../../headers/file_reader.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    string root_path = "/Users/khaledabdelfattah/Documents/workspace/networks/reliable-data-transport-protocol/udp_server/udp_server/";
    string in_file_path = root_path + "server.in";
    vector<string> lines = read_from_file(in_file_path);
    int port_num = atoi(lines[0].c_str());
    double seed = stod(lines[1]);
    double plp = stod(lines[2]);
    
    Server* server = new Server(port_num, seed, plp);
    server->initiate();
    return 0;
}
