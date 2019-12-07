//
//  packet_loss_simulator.hpp
//  udp_server
//
//  Created by Khaled Abdelfattah on 12/7/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#ifndef packet_loss_simulator_hpp
#define packet_loss_simulator_hpp

#include <stdio.h>
#include <random>

using namespace std;

class PacketLossSimulator {
    double plp;
    double seed;
    default_random_engine engine;
    uniform_real_distribution<> distribution;
    
public:
    PacketLossSimulator() {}
    
    PacketLossSimulator(double plp, double seed) {
        this->plp = plp;
        this->seed = seed;
        this->engine = default_random_engine((int) time(0));
        this->distribution = uniform_real_distribution<>(0.0f, 1.0f);
    }
    
    bool to_be_dropped() {
        return (distribution(engine) <= plp);
    }
};

#endif /* packet_loss_simulator_hpp */
