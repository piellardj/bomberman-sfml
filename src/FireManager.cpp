#include "FireManager.hpp"

#include "Constants.hpp"
#include "Map.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <stdexcept>
#include <sstream>

FireManager::FireManager (TimeManager const& timeManager, Map& map):
            _timeManager (timeManager),
            _map (map),
            _fires (),
            _spriteSheet (),
            _bufferTexture ()
{
    std::string path = "rc/fire.png";
    if (!_spriteSheet.loadFromFile(path)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << path
            << " (FireManager::FireManager)";
        throw std::runtime_error (oss.str());
    }

    _bufferTexture.create (map.getNbCol()*TILE_SIZE, map.getNbLines()*TILE_SIZE);
}

void FireManager::addFire (int line, int col, unsigned int force)
{
    _fires.push_back(Fire(_timeManager, _map, _spriteSheet, line, col, force));
    updateBufferTexture();
}

void FireManager::update ()
{
    bool needToUpdate = false; //does the buffer need to be redrawn

    /* Spreading */
    for (Fire& fire : _fires) {
        if (fire.spread())
            needToUpdate = true;
    }

    /* Removal of old fires (all 4 branches are dead) */
    auto it = _fires.begin();
    while (it != _fires.end()) {
        if (it->shouldBeDeleted()) {
            it = _fires.erase (it);
            needToUpdate = true;
        } else {
            ++it;
        }
    }

    if (needToUpdate)
        updateBufferTexture();
}

bool FireManager::isOnFire (int line, int col) const
{
    for (Fire const& fire : _fires) {
        if (fire.isOnFire (line, col))
            return true;
    }

    return false;
}

void FireManager::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::Sprite sprite (_bufferTexture.getTexture());
    sprite.setScale(1.f, -1.f);
    sprite.setPosition (0.f, _bufferTexture.getSize().y);

    target.draw (sprite, states);
}

void FireManager::updateBufferTexture()
{
    _bufferTexture.clear (sf::Color(255,0,0,0));

    for (Fire const& fire : _fires)
        _bufferTexture.draw (fire);//, sf::BlendNone);
}
