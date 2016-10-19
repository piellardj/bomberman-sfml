#include "World.hpp"

#include "Constants.hpp"

#include <SFML/Graphics/Image.hpp>

#include <cassert>
#include <stdexcept>
#include <sstream>

World::World (std::string const& gridPath):
            _timeManager (),
            _map (nullptr),
            _players({nullptr, nullptr})
{
    sf::Image grid;
    if (!grid.loadFromFile(gridPath)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << gridPath << " (World::World)";
        throw std::runtime_error(oss.str());
    }

    _map.reset (new Map (_timeManager, grid));

    PlayerKeyBinding keys;
    keys.up = sf::Keyboard::Z;
    keys.right = sf::Keyboard::D;
    keys.left = sf::Keyboard::Q;
    keys.down = sf::Keyboard::S;
    keys.placeBomb = sf::Keyboard::A;
    keys.remote = sf::Keyboard::E;
    _players[0].reset (new Player (0, _timeManager, *_map, keys, "rc/player1.png",
                                   1, 1));

    keys.up = sf::Keyboard::Up;
    keys.right = sf::Keyboard::Right;
    keys.left = sf::Keyboard::Left;
    keys.down = sf::Keyboard::Down;
    keys.placeBomb = sf::Keyboard::L;
    keys.remote = sf::Keyboard::M;
    _players[1].reset (new Player (1, _timeManager, *_map, keys, "rc/player2.png",
                                   _map->getNbLines()-2, _map->getNbCol()-2));
}

unsigned int World::widthInPixels () const
{
    assert (_map);
    return _map->getNbCol() * TILE_SIZE;
}
unsigned int World::heightInPixels () const
{
    assert (_map);
    return _map->getNbLines() * TILE_SIZE;
}

void World::update ()
{
    assert (_map);
    _map->update ();

    for (std::unique_ptr<Player> const& playerPtr : _players) {
        assert (playerPtr);
        playerPtr->update ();

        int line, col;
        playerPtr->getTile (line, col);
        if (_map->isLethal(line, col))
            playerPtr->die ();
    }
}

void World::manageEvents (sf::Event const& event)
{
    switch (event.type) {
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::P) {
                if (_timeManager.isPaused())
                    _timeManager.resume();
                else
                    _timeManager.pause();

                return;
            }
        break;
        default:
        break;
    }

    if (_timeManager.isPaused())
        return;

    for (std::unique_ptr<Player> const& playerPtr : _players) {
        assert (playerPtr);
        playerPtr->manageEvents (event);
    }
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    assert (_map);
    target.draw (*_map, states);

    for (std::unique_ptr<Player> const& playerPtr : _players)
        target.draw (*playerPtr, states);
}
