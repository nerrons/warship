//
//  Engine.cpp
//  warship
//
//  Created by Nerrons on 9/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <stdio.h>
#include "Engine.h"

WarChannel wc;

FMOD_RESULT F_CALLBACK
Engine::WriteSilenceData(FMOD_SOUND *sound, void *data, unsigned int length) {
    // callback function is required to have a void pointer, need to
    // cast to correct type (PCM16)
    PCM16 *pcm_data = (PCM16*)data;
    
    // arg length is in bytes; each sample has two bytes (16 bits)
    // num_samples is the number of samples to write to the buffer
    int num_samples = length / 2;
    
    // output silence
    for (int i = 0; i < num_samples; i++) {
        pcm_data[i] = 0;
    }
    
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK
Engine::WriteSoundData(FMOD_SOUND *sound, void *data, unsigned int length) {
    memset(data, 0, length); // clear buffer
    PCM16 *pcm_data = (PCM16*)data;
    int num_samples = length / 2;
    wc.WriteSoundData(pcm_data, num_samples);
    return FMOD_OK;
}

Engine::Engine(int total_duration)
: sampling_rate(44100)
, format(FMOD_SOUND_FORMAT_PCM16)
, total_duration(total_duration)
, update_duration(0.1)
, num_channels(2)
, num_total_bytes(sampling_rate * num_channels * sizeof(PCM16) * total_duration)
, num_update_bytes(sampling_rate * update_duration) {
    FMOD::System_Create(&system);
    system->init(5, FMOD_INIT_NORMAL, 0);
    
    memset(&info, 0, sizeof(FMOD_INFO));
    info.cbsize = sizeof(FMOD_INFO);
    info.defaultfrequency = sampling_rate;
    info.format = format;
    info.numchannels = num_channels;
    info.length = num_total_bytes;
    info.decodebuffersize = num_update_bytes;
    info.pcmreadcallback = Engine::WriteSoundData;
}

Engine::~Engine() {
    sound->release();
    system->release();
}

void Engine::init() {
    system->createStream(nullptr, mode, &info, &sound);
    system->playSound(sound, nullptr, false, nullptr);
}
