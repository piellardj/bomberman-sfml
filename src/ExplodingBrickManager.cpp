#include "ExplodingBrickManager.hpp"

#include "Constants.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <stdexcept>
#include <sstream>

ExplodingBrickManager::ExplodingBrickManager (TimeManager const& timeManager):
            _timeManager (timeManager),
            _bricks(),
            _spriteSheet()
{
    std::string path = "rc/exploding_brick.png";
    if (!_spriteSheet.loadFromFile(path)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << path
            << " (ExplodingBrickManager::ExplodingBrickManager)";
        throw std::runtime_error (oss.str());
    }
}

void ExplodingBrickManager::addBrick (unsigned int line, unsigned int col)
{
    _bricks.push_back(ExplodingBrick{line, col, _timeManager.getTime()});
}

void ExplodingBrickManager::update ()
{
    const sf::Time time = _timeManager.getTime();

    auto it = _bricks.begin();
    while (it != _bricks.end()) {
        if (time - it->birthtime > EXPLODING_BRICK_LIFETIME)
            it = _bricks.erase (it);
        else
            ++it;
    }
}

void ExplodingBrickManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Sprite sprite (_spriteSheet);

    for (ExplodingBrick const& brick : _bricks) {
        int dt = (_timeManager.getTime() - brick.birthtime).asMilliseconds();
        int lifetime = EXPLODING_BRICK_LIFETIME.asMilliseconds();
        int i = std::min(3*dt / lifetime, 2);

        sprite.setTextureRect(sf::IntRect(i*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
        sprite.setPosition (brick.col*TILE_SIZE, brick.line*TILE_SIZE);
        target.draw (sprite, states);
    }
}
