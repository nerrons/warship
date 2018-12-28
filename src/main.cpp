//
//  main.cpp
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Window.hpp>
#include <vector>
#include "fmod.hpp"
#include "Warship.h"

string v3f_str(const v3f &v) {
    ostringstream os;
    os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return os.str();
}

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
    info.minDistance = 5.0f;
    info.maxDistance = 20.0f;
    info.virtDistance = 20.0f;
    int soundId = ship.RegisterSound(info);
    int warchanId;
    bool virtFlag = false;
    vector<int> warchanIds;
    v3f soundPos{ 10.0f, 10.0f, 0.0f };
    v3f earPos{ 0.0f, 0.0f, 0.0f };

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
                    warchanId = ship.PlaySound(soundId, soundPos, 1.0);
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
                if (e.key.code == sf::Keyboard::Up) {
                    ship.SetEarPosition({ 0.0f, 1.0f, 0.0f }, true);
                    earPos += { 0.0f, 1.0f, 0.0f };
                    cout << v3f_str(earPos) << endl;
                }
                if (e.key.code == sf::Keyboard::Down) {
                    ship.SetEarPosition({ 0.0f, -1.0f, 0.0f }, true);
                    earPos += { 0.0f, -1.0f, 0.0f };
                    cout << v3f_str(earPos) << endl;
                }
                if (e.key.code == sf::Keyboard::Left) {
                    ship.SetEarPosition({ -1.0f, 0.0f, 0.0f }, true);
                    earPos += { -1.0f, 0.0f, 0.0f };
                    cout << v3f_str(earPos) << endl;
                }
                if (e.key.code == sf::Keyboard::Right) {
                    ship.SetEarPosition({ 1.0f, 0.0f, 0.0f }, true);
                    earPos += { 1.0f, 0.0f, 0.0f };
                    cout << v3f_str(earPos) << endl;
                }
            }
        }
        Warship::Update(elapsed);
    }

    Warship::Shutdown();
    return 0;
}
