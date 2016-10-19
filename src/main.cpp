#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <iostream>

#include "World.hpp"

int main()
{
    World* world = nullptr;
    try {
        world = new World("rc/grid.bmp");
    } catch (std::exception const& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    sf::RenderWindow window(sf::VideoMode(world->widthInPixels(), world->heightInPixels()),
                            "Bomberman");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                world->manageEvents(event);
        }

        world->update ();

        window.clear();
        window.draw(*world);
        window.display();
    }

    delete world;

    return EXIT_SUCCESS;
}
