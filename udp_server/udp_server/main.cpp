//
//  main.cpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include <iostream>
#include "server.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    Server* server = new Server();
    server->initiate();
    return 0;
}
