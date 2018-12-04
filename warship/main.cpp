//
//  main.cpp
//  warship
//
//  Created by Nerrons on 3/12/18.
//  Copyright © 2018 nerrons. All rights reserved.
//

#include <iostream>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <SFML/Window.hpp>
#include "SimpleAudioManager.h"
#include "AudioManager.h"

void exitOnError(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

int main(int argc, const char *argv[]) {
    return 0;
}

/*
int main(int argc, const char * argv[]) {
    sf::Window window(sf::VideoMode(320, 240), "AudioPlayback");
    sf::Clock clock;
    
    SimpleAudioManager am;
    am.load("003.mp3");
    
    while (window.isOpen()) {
        double elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0 / 60.0) continue;
        clock.restart();
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
            if (e.type == sf::Event::KeyPressed
                && e.key.code == sf::Keyboard::Space ) {
                am.play("003.mp3");
            }
        }
        am.update(elapsed);
    }
    
    return 0;
}
*/
