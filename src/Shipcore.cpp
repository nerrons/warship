// Shipcore
// The implementation details of Warship.
// Created by Nerrons on 2018-12-23.
//

// TODO: Implement Virtual Check.
// TODO: Make some sound effects for testing. (footsteps, drums)
// TODO: Implement ambient reverb and updating reverb params according to the ear position.
// TODO: Implement walls, obstruction and occlution.
// TODO: Make sounds movable by supporting callback functions that changes the position of a sound

#include <iostream>
#include "Warship.h"

struct Shipcore {
    Shipcore();
    ~Shipcore();

    void SetEarPosition(const v3f &newPosition, bool isRelative);
    void Update(float delta);
    bool SoundLoaded(int soundId);
    void LoadSound(int soundId);

    FMOD::System* system;

    enum class VirtStyle {
        RESET, PAUSE, MUTE
    };

    // Warchan is a wrapper of FMOD channels.
    struct Warchan {
        Warchan(Shipcore& shipcore, int soundId, Warship::SoundInfo* soundInfo,
                VirtStyle virtStyle, const v3f& position, float volume);
        ~Warchan();

        enum class State {
            INIT, TOPLAY, DEVIRTING, LOADING, PREPLAYING, PLAYING, VIRTING, VIRT, STOPPING, STOPPED
        };

        Shipcore& shipcore;
        FMOD::Channel* fmodChannel = nullptr;
        Warship::SoundInfo* soundInfo;

        int soundId;
        VirtStyle virtStyle;
        float virtDistance;
        float virtFadeInTime = 2.0f;
        float virtFadeOutTime = 2.0f;
        float stopFadeOutTime = 4.0f;

        State state = State::INIT;
        float volume = 1.0f;
        bool stopRequested = false;
        v3f position;
        bool positionChangeFlag = false;
        bool virtFlagIsEffective = false;
        bool virtFlag = false;

        void Update(float delta);
        void UpdateParams();
        void UpdateFadeIn(float delta);
        void UpdateFadeOut(float delta);
        bool VirtualCheck(bool allowOneshot) const;
        bool IsPlaying() const;
        bool IsOneShot() const;
    };

    v3f earPosition;
    int coreSampleRate;
    int nextSoundId;
    int nextWarchanId;
    typedef map<int, FMOD::Sound*> SoundMap;
    typedef map<int, Warship::SoundInfo*> SoundInfoMap;
    typedef map<int, unique_ptr<Warchan>> WarchanMap;
    SoundMap sounds;
    SoundInfoMap soundInfos;
    WarchanMap warchans;
};

Shipcore::Shipcore()
        : earPosition{ 0.0f, 0.0f, 0.0f }
        , coreSampleRate(44100)
        , nextSoundId(0)
        , nextWarchanId(0){
    system = nullptr;
    FMOD::System_Create(&system);
    FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG);
    system->init(128, FMOD_INIT_NORMAL, nullptr);
    system->getSoftwareFormat(&coreSampleRate, nullptr, nullptr);
    FMOD_VECTOR listenerPos = { earPosition.x, earPosition.y, earPosition.z };
    FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
    system->set3DListenerAttributes(0, &listenerPos, &listenerVel, nullptr, nullptr);
}

Shipcore::~Shipcore() {
    // Release all sounds and the system
    for (auto s : sounds) {
        s.second->release();
    }
    sounds.clear();
    system->release();
    system = nullptr;
}

Shipcore::Warchan::Warchan(Shipcore &shipcore, int soundId, Warship::SoundInfo *soundInfo,
        VirtStyle virtStyle, const v3f &position, float volume)
        : shipcore(shipcore)
        , soundInfo(soundInfo)
        , soundId(soundId)
        , virtStyle(virtStyle)
        , virtDistance(soundInfo->virtDistance)
        , position(position) {}

Shipcore::Warchan::~Warchan(){

}

void Shipcore::SetEarPosition(const v3f &newPosition, bool isRelative) {
    FMOD_VECTOR listenerPos;
    FMOD_VECTOR listenerVel;
    if (isRelative) {
        listenerPos = { earPosition.x + newPosition.x,
                        earPosition.y + newPosition.y,
                        earPosition.z + newPosition.z };
        listenerVel = { 0.0f, 0.0f, 0.0f };
    } else {
        listenerPos = { newPosition.x, newPosition.y, newPosition.z };
        listenerVel = { 0.0f, 0.0f, 0.0f };
    }
    earPosition = { listenerPos.x, listenerPos.y, listenerPos.z };
    cout << earPosition.x << " " << earPosition.y << " " << earPosition.z << endl;
    system->set3DListenerAttributes(0, &listenerPos, &listenerVel, nullptr, nullptr);
}

void Shipcore::LoadSound(int soundId) {
    // check if sound is registered
    auto info = soundInfos.find(soundId);
    if (info == soundInfos.end()) {
        return;
    }

    // generate mode according to the args passed in
    FMOD_MODE mode = FMOD_NONBLOCKING;
    mode |= info->second->is3D      ?           FMOD_3D : FMOD_2D;
    mode |= info->second->isLooping ?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= info->second->isStream  ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    // create the sound and register it in the shipcore
    FMOD::Sound* sound = nullptr;
    system->createSound(info->second->soundName.c_str(), mode, nullptr, &sound);
    if (sound) {
        sounds[soundId] = sound;
    }
}

bool Shipcore::SoundLoaded(int soundId) {
    // Sounds in the SoundMap are created after system->createSound() finishes.
    auto found = sounds.find(soundId);
    return !(found == sounds.end());
}

void Shipcore::Update(float delta) {
    vector<WarchanMap::iterator> stoppedChannels;
    // iterate through warchans, update their states  respectively
    for (auto iter = warchans.begin(), end = warchans.end(); iter != end; iter++) {
        iter->second->Update(delta);
        if (iter->second->state == Warchan::State::STOPPED) {
            stoppedChannels.push_back(iter);
        }
    }
    // free stopped channels
    for (auto& iter : stoppedChannels) {
        warchans.erase(iter);
    }
    system->update();
}

// The main update function for each warchan.
void Shipcore::Warchan::Update(float delta) {
    switch(state) {
        case State::INIT:
            [[fallthrough]];
        case State::DEVIRTING:
            [[fallthrough]];
        case State::TOPLAY: {
            // TOPLAY -> STOPPING
            if (stopRequested) {
                state = State::STOPPING;
                return;
            }

            // TOPLAY -> STOPPING / VIRT
            if (VirtualCheck(true)) {
                if (IsOneShot()) { // change this to oneshot check
                    state = State::STOPPING;
                } else {
                    state = State::VIRT;
                }
                return;
            }

            // If sound needs to be loaded
            // TOPLAY -> LOADING
            if (!shipcore.SoundLoaded(soundId)) {
                shipcore.LoadSound(soundId);
                state = State::LOADING;
                return;
            }

            // If no special cases
            // TOPLAY -> PLAYING
            fmodChannel = nullptr;
            auto found = shipcore.sounds.find(soundId);
            if (found != shipcore.sounds.end()) {
                shipcore.system->playSound(found->second, nullptr, true, &fmodChannel);
            }
            if (fmodChannel) {
                if (state == State::DEVIRTING) {
                    // cout << "devirting" << endl;
                }
                state = State::PLAYING;
                FMOD_VECTOR pos{position.x, position.y, position.z};
                fmodChannel->set3DAttributes(&pos, nullptr);
                fmodChannel->set3DMinMaxDistance(soundInfo->minDistance, soundInfo->maxDistance);
                fmodChannel->setVolume(volume);
                fmodChannel->setPaused(false);
            } else {
                state = State::STOPPING;
            }
        } break;

        case State::LOADING:
            if (shipcore.SoundLoaded(soundId)) {
                state = State::TOPLAY;
            }
            break;
        case State::PREPLAYING:
            UpdateFadeIn(delta);
        case State::PLAYING:
            UpdateParams();
            if (!IsPlaying() || stopRequested) {
                state = State::STOPPING;
                return;
            }
            if (VirtualCheck(false)) {
                state = State::VIRTING;
            }
            break;
        case State::STOPPING:
            UpdateFadeOut(delta);
            UpdateParams();
            if (!IsPlaying() || volume == 0.0f) {
                fmodChannel->stop();
                state = State::STOPPED;
                return;
            }
            break;
        case State::STOPPED:
            break;
        case State::VIRTING:
            UpdateFadeOut(delta);
            UpdateParams();
            if (!VirtualCheck(false)) {
                state = State::PREPLAYING;
                break;
            }
            break;
        case State::VIRT:
            if (stopRequested) {
                state = State::STOPPING;
            } else if (!VirtualCheck(false)) {
                if (virtStyle == VirtStyle::RESET) {
                    state = State::DEVIRTING;
                } else if (virtStyle == VirtStyle::PAUSE) {
                    fmodChannel->setPaused(false);
                    state = State::PREPLAYING;
                } else {
                    state = State::PREPLAYING;
                }
            }
            break;
    }
}

void Shipcore::Warchan::UpdateFadeIn(float delta) {
    float currentVolume;
    fmodChannel->getVolume(&currentVolume);
    float newVolume = currentVolume + delta / virtFadeInTime;
    if (newVolume >= volume) {
        if (state == State::PREPLAYING) {
            state = State::PLAYING;
        }
    } else {
        fmodChannel->setVolume(newVolume);
    }
}

void Shipcore::Warchan::UpdateFadeOut(float delta) {
    float currentVolume;
    fmodChannel->getVolume(&currentVolume);
    float newVolume = currentVolume;
    if (state == State::STOPPING) {
        newVolume = currentVolume - delta / stopFadeOutTime;
    } else if (state == State::VIRTING) {
        newVolume = currentVolume - delta / virtFadeOutTime;
    }
    if (newVolume <= 0.0f) {
        if (state == State::STOPPING) {
            fmodChannel->stop();
            state = State::STOPPED;
        } else if (state == State::VIRTING) {
            state = State::VIRT;
            if (virtStyle == VirtStyle::RESET) {
                fmodChannel->stop();
            } else if (virtStyle == VirtStyle::PAUSE) {
                fmodChannel->setPaused(true);
            }
        }
    } else {
        fmodChannel->setVolume(newVolume);
    }
}

bool Shipcore::Warchan::IsPlaying() const {
    bool result = false;
    fmodChannel->isPlaying(&result);
    return result;
}

bool Shipcore::Warchan::IsOneShot() const {
    int loopCount = true;
    fmodChannel->getLoopCount(&loopCount);
    return (loopCount == 0);
}

void Shipcore::Warchan::UpdateParams() {
    if (positionChangeFlag) {
        FMOD_VECTOR newPos = { position.x, position.y, position.z };
        fmodChannel->set3DAttributes(&newPos, nullptr);
        positionChangeFlag = false;
    }
}

bool Shipcore::Warchan::VirtualCheck(bool allowOneshot) const {
    if (virtFlagIsEffective) {
        return virtFlag;
    } 
}
