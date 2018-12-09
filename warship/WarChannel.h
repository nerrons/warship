//
//  WarChannel.h
//  warship
//
//  Created by Nerrons on 8/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include "WarSound.h"

#ifndef WarChannel_h
#define WarChannel_h

class WarChannel {
public:
    WarChannel() : sound(nullptr), position(0) {}
    ~WarChannel();
    void Play(WarSound *wsound);
    void Stop();
    void WriteSoundData(PCM16 *data, int count);
private:
    WarSound *sound;
    long position;
}; typedef WarChannel WC;

WC::~WarChannel() {}

void WC::Play(WS *s) {
    sound = s;
    position = 0;
}

void WC::Stop() {
    sound = nullptr;
}

void WC::WriteSoundData(PCM16 *dest, int num_samples) {
    // if no sound in this channel, do nothing
    if (sound == nullptr) return;
    
    for (int i = 0; i < num_samples; i += 2) {
        // if reached the end, stop and return
        if (position >= sound->num_samples) {
            Stop();
            return;
        }
        
        // write one byte
        PCM16 pcm_value = sound->data[position];
        dest[i]     = pcm_value;
        dest[i + 1] = pcm_value;
        position += 1;
    }
}

#endif /* WarChannel_h */
