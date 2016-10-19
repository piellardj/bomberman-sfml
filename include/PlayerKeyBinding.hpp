#ifndef PLAYERKEYBINDING_HPP_INCLUDED
#define PLAYERKEYBINDING_HPP_INCLUDED

#include <SFML/Window/Keyboard.hpp>

#include "Constants.hpp"

/** Basic structure used for storing key bindings */
struct PlayerKeyBinding {
    sf::Keyboard::Key left;
    sf::Keyboard::Key up;
    sf::Keyboard::Key right;
    sf::Keyboard::Key down;

    sf::Keyboard::Key placeBomb;
    sf::Keyboard::Key remote;

    /** @return whether or not the key is associated to a directional action */
    bool isDirectionnalKey (sf::Keyboard::Key key) const
    {
        return (key == up) || (key == right) || (key == down) || (key == left);
    }

    /** @return the direction associated to the key.
     *          Dir::LEFT if the key is not associated to any direction */
    Dir getDirection (sf::Keyboard::Key key) const
    {
        if (key == up)
            return Dir::UP;
        if (key == right)
            return Dir::RIGHT;
        if (key == down)
            return Dir::DOWN;
        return Dir::LEFT;
    }
};

#endif // PLAYERKEYBINDING_HPP_INCLUDED
