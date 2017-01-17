#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "utils.h"
#include "sim.h"

using namespace std;

int main() {
    
    // initialize particles
    float *part_pos = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_vel = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_acc = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_mas = (float*) safe_calloc(npart, sizeof(float));
    float *part_force = (float*) safe_calloc(2*npart, sizeof(float));
    init(npart, part_pos, part_vel, part_acc, part_mas);

    // create window
    sf::RenderWindow window(sf::VideoMode(screen_size, screen_size), "");

    // create particles
    vector<sf::CircleShape> part_vec;
    for(size_t i=0; i<npart/4; i++) {
        sf::CircleShape shape(part_mas[i]/mass_scale*1.0);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(part_pos[2*i+0], part_pos[2*i+1]);
        part_vec.push_back(shape);
    }
    for(size_t i=npart/4; i<npart/2; i++) {
        sf::CircleShape shape(part_mas[i]/mass_scale*1.0);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(part_pos[2*i+0], part_pos[2*i+1]);
        part_vec.push_back(shape);
    }
    for(size_t i=npart/2; i<3*npart/4; i++) {
        sf::CircleShape shape(part_mas[i]/mass_scale*1.0);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(part_pos[2*i+0], part_pos[2*i+1]);
        part_vec.push_back(shape);
    }
    for(size_t i=3*npart/4; i<npart; i++) {
        sf::CircleShape shape(part_mas[i]/mass_scale*1.0);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(part_pos[2*i+0], part_pos[2*i+1]);
        part_vec.push_back(shape);
    }
    // while window open draw and update particles
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // draw particles
        window.clear();
        for(size_t i=0; i<npart; i++) {
            window.draw(part_vec[i]);
        }
        window.display();

        // update particle
        update(npart, part_pos, part_vel, part_acc, part_mas, part_force);
        for(size_t i=0; i<npart; i++) {
            part_vec[i].setPosition(part_pos[2*i+0], part_pos[2*i+1]);
        }
    }

    return 0;
}
