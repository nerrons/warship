//
// Created by Nerrons on 2018-12-20.
//

#include "Warship.h"

struct Shipcore {
    Shipcore();
    ~Shipcore();

    void Update();

    FMOD::System* system;

    int nextChannelId;
    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    SoundMap sounds;
    ChannelMap channels;
};

Shipcore::Shipcore()
: nextChannelId(0) {
    system = nullptr;
    FMOD::System_Create(&system);
    system->init(128, FMOD_INIT_NORMAL, nullptr);
}

Shipcore::~Shipcore() {
    system->release();
}

Shipcore* core = nullptr;

void Warship::Init() {
    core = new Shipcore;
}

void Warship::Update() {
    core->Update();
}

void Warship::Shutdown() {
    delete core;
}

void Shipcore::Update() {
    vector<ChannelMap::iterator> stoppedChannels;
    for (auto iter = channels.begin(), end = channels.end(); iter != end; iter++) {
        bool isPlaying = false;
        iter->second->isPlaying(&isPlaying);
        if (!isPlaying) {
            stoppedChannels.push_back(iter);
        }
    }
    for (auto& iter : stoppedChannels) {
        channels.erase(iter);
    }
    system->update();
}

void Warship::LoadSound(const string& soundName, bool is3D, bool isLooping, bool isStream) {
    // Check if sound is already loaded. If so, do nothing
    auto found = core->sounds.find(soundName);
    if (found != core->sounds.end()) {
        return;
    }

    // generate mode according to the args passed in
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3D ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    // create the sound and register it in the shipcore
    FMOD::Sound* sound = nullptr;
    core->system->createSound(soundName.c_str(), mode, nullptr, &sound);
    if (sound) {
        core->sounds[soundName] = sound;
    }
}

void Warship::UnloadSound(const string &soundName) {
    // Check if sound is loaded. If not, do nothing
    auto found = core->sounds.find(soundName);
    if (found == core->sounds.end()) {
        return;
    }

    // Sound needs to be unloaded
    found->second->release();
    core->sounds.erase(found);
}

int Warship::PlaySound(const string &soundName, const v3f &position, float volume) {
    int channelId = core->nextChannelId;
    core->nextChannelId++;
    auto found = core->sounds.find(soundName);

    // if audio not loaded, try to load. if fails to load, return
    if (found == core->sounds.end()) { // not loaded; load sound
        LoadSound(soundName, false, false, false);
        found = core->sounds.find(soundName);
        if (found == core->sounds.end()) {
            return channelId;
        }
    }

    FMOD::Channel* channel = nullptr;
    core->system->playSound(found->second, nullptr, true, &channel);
    if (channel) {
        FMOD_VECTOR f_position = { position.x, position.y, position.z };
        channel->set3DAttributes(&f_position, nullptr);
        channel->setVolume(volume);
        channel->setPaused(false);
        core->channels[channelId] = channel;
    }
    return channelId;
}

void Warship::SetChannelVolume(int channelId, float volume) {
    auto found = core->channels.find(channelId);
    if (found == core->channels.end()) {
        return;
    }

    found->second->setVolume(volume);
}

