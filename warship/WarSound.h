//
//  WarSound.h
//  warship
//
//  Created by Nerrons on 8/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <fstream>

#ifndef WarSound_h
#define WarSound_h

typedef signed short PCM16;
typedef unsigned int U32;
typedef unsigned short U16;

class WarSound {
public:
    WarSound(const char *path);
    ~WarSound();
    
    U32 sampling_rate;
    U16 num_channels;
    U16 bits_per_sample;
    PCM16 *data;
    U32 num_samples;
};

typedef WarSound WS;

WS::WarSound(const char *path) {
    // open file as binary
    std::ifstream file(path, std::ios::in | std::ios::binary);
    
    // read num_channel and sampling_rate
    file.seekg(22);
    file.read((char*)&num_channels, 2);
    file.read((char*)&sampling_rate, 4);
    //std::cout << num_channels << std::endl;
    //std::cout << sampling_rate << std::endl;
    
    // read bits_per_sample
    file.seekg(34);
    file.read((char*)&bits_per_sample, 2);
    
    // read size of data; init data array; read data
    U32 num_bytes;
    file.seekg(40);
    file.read((char*)&num_bytes, 4);
    num_samples = num_bytes / 2;
    data = new PCM16[num_samples];
    file.read((char*)data, num_bytes);
}

WS::~WarSound() {
    delete[] data;
}



#endif /* WarSound_h */
