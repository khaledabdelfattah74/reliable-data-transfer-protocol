//
//  file_reader.cpp
//  udp_client
//
//  Created by Khaled Abdelfattah on 12/7/19.
//  Copyright © 2019 Khaled Abdelfattah. All rights reserved.
//

#include "file_reader.hpp"

vector<string> read_from_file(string path) {
    ifstream input_file(path);
    vector<string> lines;
    if (input_file.is_open()) {
        string line;
        while (getline(input_file, line)) {
            lines.push_back(line);
        }
        input_file.close();
    }
    return lines;
}
