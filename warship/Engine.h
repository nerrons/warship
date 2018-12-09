//
//  Engine.h
//  warship
//
//  Created by Nerrons on 8/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <fmod.hpp>
#include "WarSound.h"
#include "WarChannel.h"

#ifndef Engine_h
#define Engine_h

typedef signed short PCM16;
typedef unsigned int U32;
typedef unsigned short U16;
typedef FMOD_CREATESOUNDEXINFO FMOD_INFO;

extern WarChannel wc;

class Engine {
public:
    Engine(int total_duration);
    ~Engine();
    
    void init();
private:
    // basic structure
    PCM16 buffer[44100]; // 44.1kHz * 21s * 2 channels
    FMOD::System *system;
    FMOD::Sound *sound;
    FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENUSER;
    
    // sound info structure
    FMOD_INFO info;
    int sampling_rate;
    FMOD_SOUND_FORMAT format;
    int total_duration; // total duration of the sound in seconds
    int update_duration; // duration of each update in seconds
    int num_channels;
    int num_total_bytes;
    int num_update_bytes;
    
    // callback functions
    
    static FMOD_RESULT F_CALLBACK WriteSilenceData(FMOD_SOUND *sound, void *data,
                                                   unsigned int length);
    static FMOD_RESULT F_CALLBACK WriteSoundData(FMOD_SOUND *sound, void *data,
                                                 unsigned int length);
};

#endif /* Engine_h */
