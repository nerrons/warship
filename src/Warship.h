//
// Created by Nerrons on 2018-12-20.
//

#ifndef WARSHIP_WARSHIP_H
#define WARSHIP_WARSHIP_H

#include <SFML/System/Vector3.hpp>
#include <string>
#include <vector>
#include <map>
#include "fmod.hpp"

using namespace std;

typedef sf::Vector3<float> v3f;

class Warship {
public:
    static void Init();
    static void Update(float delta);
    static void Shutdown();

    struct SoundInfo {
        string soundName;
        float defaultVolume;
        float minDistance;
        float maxDistance;
        float virtDistance;
        bool is3D;
        bool isLooping;
        bool isStream;
    };

    void SetEarPosition(const v3f &newPosition, bool isRelative);
    int RegisterSound(SoundInfo& soundInfo, bool loadAfterReg = true);
    void UnregisterSound(int soundId);
    void LoadSound(int soundId);
    void UnloadSound(int soundId);
    bool SoundLoaded(int soundId);
    int PlaySound(int soundId, const v3f &position, float volume);
    void StopSound(int soundId);
    void SetWarchanVolume(int warchanId, float volume);
    void SetWarchanPosition(int warchanId, const v3f &newPosition, bool isRelative);
    void StopWarchan(int warchanId);
    void VirtualizeWarchan(int warchanId);
    void DevirtualizeWarchan(int warchanId);
};


#endif //WARSHIP_WARSHIP_H
