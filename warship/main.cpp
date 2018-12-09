//
//  main.cpp
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
// #include "SimpleAudioManager.h"
#include "AudioManager.h"
#include "WarSound.h"
#include "WarChannel.h"
//#include "Engine.h"
//#include "Engine.cpp"

typedef signed short PCM16;
typedef unsigned int U32;
typedef unsigned short U16;
typedef FMOD_CREATESOUNDEXINFO FMOD_INFO;

WarSound *piano_sound;
WarChannel *channel;

bool channel_active = false;

FMOD_RESULT F_CALLBACK
WriteSoundData(FMOD_SOUND *sound, void *data, unsigned int length) {
    memset(data, 0, length);
    PCM16 *pcm_data = (PCM16*)data;
    int num_samples = length / 2;
    if (channel_active) channel->WriteSoundData(pcm_data, num_samples);
    
    return FMOD_OK;
}

void exitOnError(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

void TestAudioManager() {
    sf::Window window(sf::VideoMode(320, 240), "AudioPlayback");
    sf::Clock clock;
    
    AudioManager am;
    am.LoadSong("003.mp3");
    am.LoadSFX("ysf.mp3");
    
    while (window.isOpen()) {
        double elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0 / 60.0) continue;
        clock.restart();
        
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Space) {
                    am.PlaySong("003.mp3");
                } else if (e.key.code == sf::Keyboard::F) {
                    am.PlaySFX("ysf.mp3", 0.3, 1.0, -3.0, 3.0);
                } else if (e.key.code == sf::Keyboard::X) {
                    am.StopAllSFXs();
                } else if (e.key.code == sf::Keyboard::S) {
                    am.StopSongs();
                } else if (e.key.code == sf::Keyboard::Left) {
                    am.SetMasterVolume(0.0);
                } else if (e.key.code == sf::Keyboard::Right) {
                    am.SetMasterVolume(1.0);
                }
            }
        }
        am.Update(elapsed);
    }
}

void TestWarship() {

}

int main(int argc, const char *argv[]) {
    //TestAudioManager();
    //TestWarship();
    
    //Engine engine(21);
    //engine.init();
    //WarSound *sound = new WarSound("piano.wav");
    //wc.Play(sound);
    
    // Create window and clock
    sf::RenderWindow window(sf::VideoMode(640, 480), "LowLevelAudio");
    sf::Clock clock;
    
    piano_sound = new WarSound("piano.wav");
    channel = new WarChannel();
    
    FMOD::System *system;
    FMOD::System_Create(&system);
    system->init(5, FMOD_INIT_NORMAL, nullptr);
    
    FMOD_CREATESOUNDEXINFO info;
    memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.defaultfrequency = 44100;
    info.format = FMOD_SOUND_FORMAT_PCM16;
    info.numchannels = 2;
    info.length = 44100 * 2 * sizeof(signed short) * 5;
    info.decodebuffersize = 1024;
    info.pcmreadcallback = WriteSoundData;
    
    FMOD::Sound *user_sound;
    system->createStream(nullptr, FMOD_LOOP_NORMAL | FMOD_OPENUSER, &info, &user_sound);
    system->playSound(user_sound, nullptr, false, 0);
    
    channel->Play(piano_sound);
    
    while (window.isOpen()) {
        float elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0f / 60.0f) continue;
        clock.restart();
        
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
            
            if (e.type == sf::Event::KeyPressed) {
                switch (e.key.code) {
                    case sf::Keyboard::A:
                        channel_active = !channel_active; break;
                    default: break;
                }
            }
        }
        
        system->update();
    }
    
    user_sound->release();
    system->release();
    delete channel;
    delete piano_sound;
    
    return 0;
}
