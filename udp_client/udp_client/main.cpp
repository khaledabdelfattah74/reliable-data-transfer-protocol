//
//  main.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include <iostream>
#include "client.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    Client* client = new Client("/Users/khaledabdelfattah/Documents/workspace/networks/reliable-data-transport-protocol/udp_client/udp_client/storage/BSCRUM-72.mov");
    client->initiate();
    return 0;
}
