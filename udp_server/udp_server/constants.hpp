
//
//  constants.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 11/28/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef constants_hpp
#define constants_hpp

#define BUFFER_SIZE 512
#define PORT 7001
#define MSG_CONFIRM 0
#define PACKET_SIZE 500

enum PROTOCOL {
    STOP_AND_WAIT,
    GO_BACK_N,
    SELECTIVE_REPEAT
};


#endif /* constants_hpp */
