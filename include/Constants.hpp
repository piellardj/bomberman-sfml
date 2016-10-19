#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <SFML/System/Time.hpp>

#include <array>

/* CONSTANTS */
const int TILE_SIZE = 32;

const sf::Time POWERUP_ANIMATION_STEP = sf::milliseconds (130);

const sf::Time EXPLODING_BRICK_LIFETIME = sf::milliseconds (200);

const sf::Time BOMB_LIFETIME = sf::milliseconds(2000);//time before autodestruction

const sf::Time FIRE_LIFETIME = sf::milliseconds(1000); //lifetime once the fire is fully spread
const sf::Time FIRE_SPEED = sf::milliseconds(40); //time for spreading through a tile

const sf::Time PLAYER_SPEED = sf::milliseconds(7); //time for walking a pixel
const sf::Time DEATH_ANIMATION_STEP = sf::milliseconds(800);


/* DATA TYPES */
typedef int PlayerId;

// the order is important a correct display of the map, etc.
enum class Dir {LEFT, UP, RIGHT, DOWN};
const std::array<int, 4> D_LINE = {+0, -1, +0, +1};
const std::array<int, 4> D_COL  = {-1, +0, +1, +0};

inline bool areOpposedDirections (Dir D1, Dir D2)
{
    if ((D1 == Dir::LEFT && D2 == Dir::RIGHT) || (D1 == Dir::RIGHT && D2 == Dir::LEFT))
        return true;
    if ((D1 == Dir::UP && D2 == Dir::DOWN) || (D1 == Dir::DOWN && D2 == Dir::UP))
        return true;
    return false;
}

#endif // UTILITIES_HPP_INCLUDED
