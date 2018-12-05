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
    sf::Window window(sf::VideoMode(320, 240), "AudioPlayback");
    sf::Clock clock;
    
    AudioManager am;
    am.LoadSong("003.mp3");
    am.LoadSFX("ysf.mp3");
    
    while (window.isOpen()) {
        double elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0 / 60.0) continue;
        clock.restart();
        
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Space) {
                    am.PlaySong("003.mp3");
                } else if (e.key.code == sf::Keyboard::F) {
                    am.PlaySFX("ysf.mp3", 0.3, 1.0, -3.0, 3.0);
                } else if (e.key.code == sf::Keyboard::X) {
                    am.StopAllSFXs();
                } else if (e.key.code == sf::Keyboard::S) {
                    am.StopSongs();
                } else if (e.key.code == sf::Keyboard::Num0) {
                    am.SetMasterVolume(0.0);
                } else if (e.key.code == sf::Keyboard::Num1) {
                    am.SetSongsVolume(0.2);
                } else if (e.key.code == sf::Keyboard::Num2) {
                    am.SetSongsVolume(0.4);
                } else if (e.key.code == sf::Keyboard::Num3) {
                    am.SetSongsVolume(0.6);
                } else if (e.key.code == sf::Keyboard::Num4) {
                    am.SetSongsVolume(0.8);
                } else if (e.key.code == sf::Keyboard::Num5) {
                    am.SetSongsVolume(1.0);
                }
            }
        }
        am.Update(elapsed);
    }
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
