//
//  AudioManager.h
//  warship
//
//  Created by Nerrons on 4/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <string>
#include <map>
#include <random>
#include <math.h>
#include <fmod.hpp>

#ifndef AudioManager_h
#define AudioManager_h

static std::random_device rd;
static std::mt19937 rng(rd());

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void Update(double elapsed);
    
    void LoadSFX(const std::string &path);
    void LoadSong(const std::string &path);
    
    void PlaySFX(const std::string &path,
                 double minVolume, double maxVolume,
                 double minPitch, double maxPitch);
    void PlaySong(const std::string &path);
    
    void StopAllSFXs();
    void StopSongs();
    
    void SetMasterVolume(double volume);
    void SetSFXsVolume(double volume);
    void SetSongsVolume(double volume);
    
private:
    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    // ok that's one way of using enum
    enum Category { CATEGORY_SFX, CATEGORY_SONG, CATEGORY_COUNT };
    
    void Load(Category type, const std::string &path);
    
    FMOD::System *system;
    FMOD::ChannelGroup *master;
    FMOD::ChannelGroup *groups[CATEGORY_COUNT]; // an array of all non-master groups
    SoundMap sounds[CATEGORY_COUNT]; // sounds[i]: a soundmap of group[i]
    FMOD_MODE modes[CATEGORY_COUNT]; // modes[i]: settings of group[i]
    
    FMOD::Channel *current_song;
    std::string current_song_path;
    std::string next_song_path;
    
    enum FadeState { FADE_NONE, FADE_IN, FADE_OUT };
    FadeState fade_state;
    double fade_time;
};
typedef AudioManager AM;


double ChangeOctave(double freq, double offset) {
    static double octave_ratio = 2.0;
    return freq * pow(octave_ratio, offset);
}

double ChangeSemitone(double freq, double offset) {
    static double semitone_ratio = pow(2.0, 1.0 / 12.0);
    return freq * pow(semitone_ratio, offset);
}

AM::AudioManager()
: current_song(nullptr)
, fade_state(FADE_NONE)
, fade_time(1.5) {
    // create system
    FMOD::System_Create(&system);
    system->init(500, FMOD_INIT_NORMAL, 0);
    system->getMasterChannelGroup(&master);
    
    // create channels for each cat
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        system->createChannelGroup(nullptr, &groups[i]);
        master->addGroup(groups[i]);
    }
    
    // setup modes for each cat
    modes[CATEGORY_SFX] = FMOD_DEFAULT;
    modes[CATEGORY_SONG] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;
}

AM::~AudioManager() {
    // release sounds in each cat
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        for (SoundMap::iterator iter = sounds[i].begin();
             iter != sounds[i].end();
             ++iter) {
            iter->second->release();
        }
        sounds[i].clear();
    }
    
    // release system
    system->release();
}

void AM::LoadSFX(const std::string &path) {
    Load(CATEGORY_SFX, path);
}

void AM::LoadSong(const std::string &path) {
    Load(CATEGORY_SONG, path);
}

void AM::Load(Category cat, const std::string &path) {
    // if already loaded, do nothing
    if (sounds[cat].find(path) != sounds[cat].end()) {
        return;
    }
    
    FMOD::Sound *sound;
    system->createSound(path.c_str(), modes[cat], nullptr, &sound);
    sounds[cat].insert(std::make_pair(path, sound));
}

void AM::PlaySFX(const std::string &path,
                 double minVolume, double maxVolume,
                 double minPitch, double maxPitch) {
    // if sound not loaded, do nothing
    SoundMap::iterator sound = sounds[CATEGORY_SFX].find(path);
    if (sound == sounds[CATEGORY_SFX].end()) return;
    
    // calc random volume and pitch
    std::uniform_real_distribution<> new_volume_dist(minVolume, maxVolume);
    std::uniform_real_distribution<> new_pitch_dist(minPitch, maxPitch);
    float new_volume = max(min(new_volume_dist(rng), 1.0), 0.0);
    float new_pitch = max(min(new_pitch_dist(rng), 1.0), 0.0);
    
    // start a new channel; add to channel group
    // play the sfx with the volume and pitch
    FMOD::Channel *channel;
    system->playSound(sound->second, nullptr, false, &channel);
    channel->setChannelGroup(groups[CATEGORY_SFX]); // assign to the sfx group
    channel->setVolume(new_volume);
    float freq;
    channel->getFrequency(&freq);
    channel->setFrequency(ChangeSemitone(freq, new_pitch));
    channel->setPaused(false);
}

void AM::StopAllSFXs() {
    groups[CATEGORY_SFX]->stop();
}

// this feels like a state machine
void AM::PlaySong(const std::string &path) {
    // if song is already playing, do nothing
    if (current_song_path == path) return;

    // if something is currently playing, stop them and
    // set the new song as the next song
    if (current_song != nullptr) {
        StopSongs();
        next_song_path = path;
    }
    
    // if song is not in the soundmap, do nothing
    // else store the corresponding sound in the variable sound
    SoundMap::iterator sound = sounds[CATEGORY_SONG].find(path);
    if (sound == sounds[CATEGORY_SONG].end()) return;
    
    // if nothing is being played and the sound of the song is present:
    // start fading in the song immediately
    current_song_path = path;
    system->playSound(sound->second, groups[CATEGORY_SONG], true, &current_song);
    current_song->setChannelGroup(groups[CATEGORY_SONG]);
    current_song->setVolume(0.0);
    current_song->setPaused(false);
    fade_state = FADE_IN;
}

void AM::StopSongs() {
    if (current_song != nullptr) {
        fade_state = FADE_OUT;
    }
    next_song_path.clear();
}

void AM::Update(double elapsed) {
    if (current_song == nullptr && !next_song_path.empty()) {
        PlaySong(next_song_path);
        next_song_path.clear();
    } else if (current_song != nullptr && fade_state == FADE_IN) {
        float volume;
        current_song->getVolume(&volume);
        float new_volume = volume + elapsed / fade_time;
        if (new_volume >= 1.0) {
            current_song->setVolume(1.0f);
            fade_state = FADE_NONE;
        } else {
            current_song->setVolume(new_volume);
        }
    } else if (current_song != nullptr && fade_state == FADE_OUT) {
        float volume;
        current_song->getVolume(&volume);
        float new_volume = volume - elapsed / fade_time;
        if (new_volume <= 0.0) {
            current_song->stop();
            current_song_path.clear();
            fade_state = FADE_NONE;
        } else {
            current_song->setVolume(new_volume);
        }
    }
    system->update();
}

void AM::SetMasterVolume(double volume) {
    master->setVolume(volume);
}

void AM::SetSFXsVolume(double volume) {
    groups[CATEGORY_SFX]->setVolume(volume);
}

void AM::SetSongsVolume(double volume) {
    groups[CATEGORY_SONG]->setVolume(volume);
}

#endif /* AudioManager_h */
