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
typedef map<string, int> SoundIdMap;

class Warship {
public:
    static void Init();
    static void Update();
    static void Shutdown();

    struct SoundInfo {
        string soundName;
        float defaultVolume;
        float minDistance;
        float maxDistance;

    };

    void LoadSound(const string& soundName, bool is3D, bool isLooped, bool isStream);
    void UnloadSound(const string& soundName);
    int PlaySound(const string& soundName, const v3f& position, float volume);
    void SetChannelVolume(int channelId, float volume);
};


#endif //WARSHIP_WARSHIP_H
