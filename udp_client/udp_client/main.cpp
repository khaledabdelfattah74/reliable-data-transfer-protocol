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
    Client* client = new Client();
    client->initiate();
    return 0;
}
