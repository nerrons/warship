//
//  SimpleAudioManager.h
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//
#include <string>
#include <map>
#include <fmod.hpp>
#include <fmod_errors.h>

#ifndef SimpleAudioManager_h
#define SimpleAudioManager_h

typedef std::map<std::string, FMOD::Sound*> SoundMap;

class SimpleAudioManager {
public:
    SimpleAudioManager();
    ~SimpleAudioManager();
    void update(float elapsed);
    void load(const std::string &path);
    void stream(const std::string &path);
    void play(const std::string &path);
private:
    void loadOrStream(const std::string &path, bool stream);
    FMOD::System *system;
    SoundMap sounds;
};
typedef SimpleAudioManager SAM;

SAM::SimpleAudioManager() {
    FMOD::System_Create(&system);
    system->init(500, FMOD_INIT_NORMAL, 0);
}

SAM::~SimpleAudioManager() {
    // release sounds one by one
    SoundMap::iterator iter;
    for (iter = sounds.begin(); iter != sounds.end(); ++iter) {
        iter->second->release();
    }
    sounds.clear();
    
    // release the system object
    system->release();
    system = nullptr;
}

void SAM::update(float elapsed) {
    system->update();
}

void SAM::loadOrStream(const std::string &path, bool stream) {
    // if already loaded, don't do anything
    if (sounds.find(path) != sounds.end()) return;
    
    // load or stream the file
    FMOD::Sound *sound;
    if (stream) system->createStream(path.c_str(), FMOD_DEFAULT, 0, &sound);
    else system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
    
    // store the sound in the map
    sounds.insert(std::make_pair(path, sound));
}

void SAM::load(const std::string &path) {
    loadOrStream(path, false);
}

void SAM::stream(const std::string &path) {
    loadOrStream(path, true);
}

void SAM::play(const std::string &path) {
    // try to find the sound in the map
    SoundMap::iterator sound = sounds.find(path);
    
    // if not loaded, do nothing
    if (sound == sounds.end()) return;
    else system->playSound(sound->second, nullptr, false, 0);
}

#endif /* SimpleAudioManager_h */
