#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;

int main() {

    // create window
    sf::RenderWindow window(sf::VideoMode(512, 512), "");
    sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();

        sf::Vector2f pos = shape.getPosition();
        shape.setPosition(pos.x+1.0, pos.y+1.0);
    }

    return 0;
}
