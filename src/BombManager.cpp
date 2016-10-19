#include "BombManager.hpp"

#include "Constants.hpp"
#include "FireManager.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>
#include <sstream>
#include <stdexcept>

BombManager::BombManager(TimeManager const& timeManager, FireManager& fireManager):
            _timeManager (timeManager),
            _fireManager (fireManager),
            _profiles (),
            _bombs (),
            _spriteSheet ()
{
    std::string path = "rc/bomb.png";
    if (!_spriteSheet.loadFromFile(path)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << path
            << " (BombManager::BombManager)";
        throw std::runtime_error (oss.str());
    }
}

void BombManager::putBomb (PlayerId player, unsigned int line, unsigned int col)
{
    //The PlayerId must have been declared in BombManager::addPlayerProfile
    assert (_profiles.find(player) != _profiles.end());

    // at most 1 bomb per tile
    if (isThereABomb(line, col))
        return;

    _bombs.push_back (Bomb{line, col, player, _timeManager.getTime()});
}

bool BombManager::isThereABomb (unsigned int line, unsigned int col) const
{
    for (Bomb const& bomb : _bombs) {
        if (bomb.line == line && bomb.col == col)
            return true;
    }
    return false;
}

void BombManager::destroy (unsigned int line, unsigned int col)
{
    /* As we know there is at most 1 bomb at the given location,
     * if we have found one, no need to check for others. */
    for (auto itBomb = _bombs.begin() ; itBomb != _bombs.end() ; ++itBomb) {
        if (itBomb->line == line && itBomb->col == col) {
            destroy(itBomb);
            return;
        }
    }
}

void BombManager::destroy (PlayerId player)
{
    //The PlayerId must have been declared in BombManager::addPlayerProfile
    assert (_profiles.find(player) != _profiles.end());

    auto itBomb = _bombs.begin();
    while (itBomb != _bombs.end()) {
        if (itBomb->player == player)
            itBomb = destroy(itBomb);
        else
            ++itBomb;
    }
}

void BombManager::addPlayerProfile (PlayerId player, unsigned int bombForce, bool autoDestruction)
{
    //A PlayerId can only be declared once
    assert (_profiles.find(player) == _profiles.end());

    _profiles.emplace (player, PlayerProfile{bombForce, autoDestruction});
}

void BombManager::setAutoDestruction (PlayerId player, bool activation)
{
    auto itProfile = _profiles.find (player);

    //The PlayerId must have been declared in BombManager::addPlayerProfile
    assert (itProfile != _profiles.end());

    itProfile->second.autoDestruction = activation;
}

void BombManager::setBombForce (PlayerId player, unsigned int force)
{
    auto itProfile = _profiles.find (player);

    //The PlayerId must have been declared in BombManager::addPlayerProfile
    assert (itProfile != _profiles.end());

    itProfile->second.bombForce = force;
}

unsigned int BombManager::getNbBomb (PlayerId playerId) const
{
    unsigned int counter = 0;
    for (Bomb const& bomb : _bombs) {
        if (bomb.player == playerId)
            ++counter;
    }
    return counter;
}

void BombManager::update ()
{
    const sf::Time time = _timeManager.getTime();

    auto itBomb = _bombs.begin();
    while ( itBomb != _bombs.end() ) {
        auto itProfile = _profiles.find (itBomb->player);

        //The PlayerId must have been declared in BombManager::addPlayerProfile
        assert (itProfile != _profiles.end());

        bool autoDestruction = itProfile->second.autoDestruction;

        if (autoDestruction && time - itBomb->birthtime > BOMB_LIFETIME)
            itBomb = destroy (itBomb);
        else
            ++itBomb;
    }
}

void BombManager::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::Sprite sprite (_spriteSheet);
    unsigned int step = (_timeManager.getTime().asMilliseconds() / POWERUP_ANIMATION_STEP.asMilliseconds()) % 4;
    step = std::min(step, 4-step);

    sprite.setTextureRect (sf::IntRect(step*TILE_SIZE, 0*TILE_SIZE, TILE_SIZE, TILE_SIZE));

    for (Bomb const& bomb : _bombs) {
        sprite.setPosition (bomb.col*TILE_SIZE, bomb.line*TILE_SIZE);
        target.draw (sprite, states);
    }
}

std::list<BombManager::Bomb>::iterator BombManager::destroy (std::list<Bomb>::iterator itBomb)
{
    auto itProfile = _profiles.find (itBomb->player);
    assert (itProfile != _profiles.end());

    int force = itProfile->second.bombForce;
    _fireManager.addFire (itBomb->line, itBomb->col, force);

    return _bombs.erase (itBomb);
}
