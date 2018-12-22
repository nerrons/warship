//
//  main.cpp
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <SFML/Window.hpp>
#include "fmod.hpp"
#include "Warship.h"

int main(int argc, const char *argv[]) {
    sf::Window window(sf::VideoMode(320, 240), "AudioPlayBack");
    sf::Clock clock;
    Warship ship;
    ship.Init();
    ship.LoadSound("resource/003.mp3", false, false, false);

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
                    v3f pos{ 0.0f, 0.0f, 0.0f };
                    ship.PlaySound("resource/003.mp3", pos, 1.0);
                }
            }
        }

        ship.Update();
    }

    return 0;
}
