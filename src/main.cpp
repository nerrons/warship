//
//  main.cpp
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <SFML/Window.hpp>
#include <vector>
#include "fmod.hpp"
#include "Warship.h"

int main(int argc, const char *argv[]) {
    sf::Window window(sf::VideoMode(320, 240), "AudioPlayBack");
    sf::Clock clock;
    Warship ship;
    Warship::Init();
    Warship::SoundInfo info;
    info.soundName = "resource/003.mp3";
    info.is3D = true;
    info.isLooping = false;
    info.isStream = false;
    info.minDistance = 1.0f;
    info.maxDistance = 20.0f;
    int soundId = ship.RegisterSound(info);
    int warchanId;
    bool virtFlag = false;
    vector<int> warchanIds;

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
                if (e.key.code == sf::Keyboard::Space) {
                    v3f pos{ 10.0f, 10.0f, 0.0f };
                    warchanId = ship.PlaySound(soundId, pos, 1.0);
                    warchanIds.push_back(warchanId);
                }
                if (e.key.code == sf::Keyboard::S) {
                    for (auto &w : warchanIds) {
                        cout << w << " ";
                        ship.StopWarchan(w);
                    }
                    warchanIds.clear(); // Warchans are not destroyed but will remain in STOPPED forever
                    cout << " --- S" << endl;
                }
                if (e.key.code == sf::Keyboard::V) {
                    for (auto &w : warchanIds) {
                        cout << w << " ";
                        virtFlag = !virtFlag;
                        if (virtFlag) {
                            ship.VirtualizeWarchan(w);
                        } else {
                            ship.DevirtualizeWarchan(w);
                        }
                    }
                    cout << " --- V" << endl;
                }
                if (e.key.code == sf::Keyboard::A) {
                    for (auto &w : warchanIds) {
                        cout << w << " ";
                    }
                    cout << " --- A" << endl;
                }
            }
        }
        Warship::Update(elapsed);
    }

    Warship::Shutdown();
    return 0;
}
