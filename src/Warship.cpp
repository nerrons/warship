// Warship
// The user interface.
// Created by Nerrons on 2018-12-20.
//

#include <iostream>
#include "Warship.h"
#include "Shipcore.cpp"

Shipcore* core = nullptr;

void Warship::Init() {
    core = new Shipcore;
}

void Warship::Update(float delta) {
    core->Update(delta);
}

void Warship::Shutdown() {
    delete core;
}

int Warship::RegisterSound(Warship::SoundInfo &soundInfo, bool loadAfterReg) {
    int soundId = core->nextSoundId;
    core->nextSoundId++;
    core->soundInfos[soundId] = &soundInfo;
    if (loadAfterReg) {
        core->LoadSound(soundId);
    }
    return soundId;
}

void Warship::UnregisterSound(int soundId) {

}

void Warship::LoadSound(int soundId) {
    core->LoadSound(soundId);
}

void Warship::UnloadSound(int soundId) {
    // Check if sound is loaded. If not, do nothing
    auto found = core->sounds.find(soundId);
    if (found == core->sounds.end()) {
        return;
    }

    // Sound needs to be unloaded
    found->second->release();
    core->sounds.erase(found);
}

bool Warship::SoundLoaded(int soundId) {
    return core->SoundLoaded(soundId);
}

int Warship::PlaySound(int soundId, const v3f &position, float volume) {
    int warchanId = core->nextWarchanId;
    core->nextWarchanId++;
    auto found = core->sounds.find(soundId);

    // if audio not loaded, just consider a fail.
    if (found == core->sounds.end()) {
        return warchanId;
    }

    core->warchans[warchanId] = make_unique<Shipcore::Warchan>(
            *core, soundId, core->soundInfos[soundId],
            Shipcore::VirtStyle::RESET, position, volume);
    return warchanId;
}

void Warship::StopSound(int soundId) {

}

void Warship::SetWarchanVolume(int warchanId, float volume) {
    auto found = core->warchans.find(warchanId);
    if (found == core->warchans.end()) {
        return;
    }

    found->second->volume = volume;
    found->second->fmodChannel->setVolume(volume);
}

void Warship::StopWarchan(int warchanId){
    auto found = core->warchans.find(warchanId);
    if (found == core->warchans.end()) {
        return;
    }
    found->second->stopRequested = true;
}

void Warship::VirtualizeWarchan(int warchanId){
    auto found = core->warchans.find(warchanId);
    if (found == core->warchans.end()) {
        return;
    }
    found->second->virtFlag = true;
}

void Warship::DevirtualizeWarchan(int warchanId){
    auto found = core->warchans.find(warchanId);
    if (found == core->warchans.end()) {
        return;
    }
    found->second->virtFlag = false;
}