#include "Fire.hpp"

#include "Constants.hpp"
#include "TimeManager.hpp"
#include "Map.hpp"

#include <SFML/Graphics/Sprite.hpp>


Fire::Fire (TimeManager const& timeManager, Map& map, sf::Texture const& spriteSheet, int oLine, int oCol, unsigned int force):
            _timeManager (timeManager),
            _map (map),
            _oLine (oLine),
            _oCol (oCol),
            _force (force),
            _lastUpdate (timeManager.getTime()),
            _branches (),
            _spriteSheet (spriteSheet)
{
    _branches.fill (Branch{0, (force==0)});
}

bool Fire::spread()
{
    bool didSpread = false;

    const sf::Time time = _timeManager.getTime();

    while (!isFullySpread() && time - _lastUpdate > FIRE_SPEED) {
        for (int iDir = 0 ; iDir < 4 ; ++iDir) {
            Branch& branch = _branches[iDir];

            if (branch.fullySpread)
                continue;

            int line = _oLine + D_LINE[iDir]*(branch.length+1),
                col = _oCol + D_COL[iDir]*(branch.length+1);
            if (_map.getTerrain(line,col) == Map::Terrain::ROCK) {
                branch.fullySpread = true;
            } else {
                if (_map.getTerrain(line,col) == Map::Terrain::BRICK)
                    branch.fullySpread = true;
                _map.burnTile (line, col);
                ++branch.length;
                if (branch.length == _force)
                    branch.fullySpread = true;
                didSpread = true;
            }
        }

        _lastUpdate += FIRE_SPEED;
    }

    return didSpread;
}

bool Fire::shouldBeDeleted () const
{
    return isFullySpread() && _timeManager.getTime() - _lastUpdate > FIRE_LIFETIME;
}

bool Fire::isFullySpread() const
{
    for (Branch const& branch : _branches)
        if (!branch.fullySpread)
            return false;
    return true;
}

bool Fire::isOnFire (int line, int col) const
{
    if (line == _oLine) {
        if (col <= _oCol && _oCol - col <= _branches[static_cast<int>(Dir::LEFT)].length)
            return true;
        if (col >= _oCol && col - _oCol <= _branches[static_cast<int>(Dir::RIGHT)].length)
            return true;
    } else if (col == _oCol) {
        if (line <= _oLine && _oLine - line <= _branches[static_cast<int>(Dir::UP)].length)
            return true;
        if (line >= _oLine && line - _oLine <= _branches[static_cast<int>(Dir::DOWN)].length)
            return true;
    }

    return false;
}

void Fire::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::Sprite sprite (_spriteSheet);
    sprite.setTextureRect (sf::IntRect(0, 4*TILE_SIZE, TILE_SIZE, TILE_SIZE));
    sprite.setPosition (_oCol*TILE_SIZE, _oLine*TILE_SIZE);
    target.draw (sprite, states);

    for (int iDir = 0 ; iDir < 4 ; ++iDir) {
        for (int length = 1 ; length <= _branches[iDir].length ; ++length) {
            int spriteX = 0;

            if (length != _branches[iDir].length)
                spriteX = 1;

            sprite.setTextureRect(sf::IntRect(spriteX*TILE_SIZE, iDir*TILE_SIZE,
                                              TILE_SIZE, TILE_SIZE));
            sprite.setPosition ((_oCol + D_COL[iDir]*length)*TILE_SIZE,
                                (_oLine + D_LINE[iDir]*length)*TILE_SIZE);
            target.draw (sprite, states);
        }
    }
}
