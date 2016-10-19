#include "Player.hpp"

#include "Constants.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <stdexcept>
#include <sstream>
#include <iostream>

const int MID_TILE = (TILE_SIZE-1) / 2;

Player::Player (PlayerId id, TimeManager const& timeManager, Map& map,
                PlayerKeyBinding const& keys, std::string const& spritesPath,
                int line, int col,
                unsigned int blast, unsigned int maxBombs, bool hasRemote):
            _timeManager (timeManager),
            _map (map),
            _keys (keys),
            _id (id),
            _lastUpdate (timeManager.getTime()),
            _isDead (false),
            _animationStep (0),
            _spriteSheet (),
            _nLine (line),
            _nCol (col),
            _dLine (MID_TILE),
            _dCol (MID_TILE),
            _isRunning (false),
            _currDir (Dir::DOWN),
            _nextDir (Dir::DOWN),
            _blastForce (blast),
            _maxBombs (maxBombs),
            _hasRemote (hasRemote)
{
    if (!_spriteSheet.loadFromFile(spritesPath)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << spritesPath
            << " (Player::Player)";
        throw std::runtime_error (oss.str());
    }

    _map.getBombManager().addPlayerProfile (_id, _blastForce, !_hasRemote);
}

void Player::update ()
{
    if (_isDead)
        return;
    if (!_isRunning) {
        _lastUpdate = _timeManager.getTime();
        return;
    }

    const sf::Time dt = _timeManager.getTime() - _lastUpdate;

    unsigned int pixels = dt.asMilliseconds() / PLAYER_SPEED.asMilliseconds();//pixels left to walk
    _lastUpdate += PLAYER_SPEED * static_cast<sf::Int64>(pixels);
    _animationStep += pixels;

    while (pixels > 0) {
        bool left =      _map.isWalkable(_nLine+0, _nCol-1),
             upLeft =    _map.isWalkable(_nLine-1, _nCol-1),
             up   =      _map.isWalkable(_nLine-1, _nCol+0),
             upRight =   _map.isWalkable(_nLine-1, _nCol+1),
             right =     _map.isWalkable(_nLine+0, _nCol+1),
             downRight = _map.isWalkable(_nLine+1, _nCol+1),
             down =      _map.isWalkable(_nLine+1, _nCol+0),
             downLeft =  _map.isWalkable(_nLine+1, _nCol-1);

         switch (_currDir) {
            case Dir::LEFT:
                if (_dLine < MID_TILE) {
                    if (left)
                        _currDir = Dir::DOWN;
                    else if (upLeft)
                        _currDir = Dir::UP;
                    else
                        pixels = 0;
                }
                else if (_dLine > MID_TILE) {
                    if (left)
                        _currDir = Dir::UP;
                    else if (downLeft)
                        _currDir = Dir::DOWN;
                    else
                        pixels = 0;
                }
                else {
                    if (_dCol == MID_TILE) {
                        if (left)
                            _currDir = Dir::LEFT;
                        else
                            pixels = 0;
                    }
                    else
                        _currDir = Dir::LEFT;
                }
            break;
            case Dir::UP:
                if (_dCol < MID_TILE) {
                    if (up)
                        _currDir = Dir::RIGHT;
                    else if (upLeft)
                        _currDir = Dir::LEFT;
                    else
                        pixels = 0;
                }
                else if (_dCol > MID_TILE) {
                    if (up)
                        _currDir = Dir::LEFT;
                    else if (upRight)
                        _currDir = Dir::RIGHT;
                    else
                        pixels = 0;
                }
                else {
                    if (_dLine == MID_TILE) {
                        if (up)
                            _currDir = Dir::UP;
                        else
                            pixels = 0;
                    }
                    else
                        _currDir = Dir::UP;
                }
            break;
            case Dir::RIGHT:
                if (_dLine < MID_TILE) {
                    if (right)
                        _currDir = Dir::DOWN;
                    else if (upRight)
                        _currDir = Dir::UP;
                    else
                        pixels = 0;
                }
                else if (_dLine > MID_TILE) {
                    if (right)
                        _currDir = Dir::UP;
                    else if (downRight)
                        _currDir = Dir::DOWN;
                    else
                        pixels = 0;
                }
                else {
                    if (_dCol == MID_TILE) {
                        if (right)
                            _currDir = Dir::RIGHT;
                        else
                            pixels = 0;
                    }
                    else
                        _currDir = Dir::RIGHT;
                }
            break;
            case Dir::DOWN:
                if (_dCol < MID_TILE) {
                    if (down)
                        _currDir = Dir::RIGHT;
                    else if (downLeft)
                        _currDir = Dir::LEFT;
                    else
                        pixels = 0;
                } else if (_dCol > MID_TILE) {
                    if (down)
                        _currDir = Dir::LEFT;
                    else if (downRight)
                        _currDir = Dir::RIGHT;
                    else
                        pixels = 0;
                } else {
                    if (_dLine == MID_TILE) {
                        if (down)
                            _currDir = Dir::DOWN;
                        else
                            pixels = 0;
                    } else {
                        _currDir = Dir::DOWN;
                    }
                }
            break;
            default:
                std::cerr << "Direction inconnue (Player::update)" << std::endl;
            break;
        }

        center (pixels);
        if (_dCol == MID_TILE && _dLine == MID_TILE) {
            std::array<bool, 4> freeDir = {left, up, right, down};
            if (freeDir[static_cast<int>(_nextDir)])
                _currDir = _nextDir;
        }
    }
}

void Player::manageEvents (sf::Event const& event)
{
    if (_isDead)
        return;

    switch (event.type) {
        case sf::Event::KeyReleased:
            if( _keys.isDirectionnalKey(event.key.code)) {
                Dir tmpDir = _keys.getDirection(event.key.code);

                if (tmpDir == _currDir) {
                    if (_currDir != _nextDir)
                        _currDir = _nextDir;
                    else
                        _isRunning = false;
                } else if (tmpDir == _nextDir) {
                    _nextDir = _currDir;
                    _isRunning = false;
                }
            }
        break;
        case sf::Event::KeyPressed:
            if (_keys.isDirectionnalKey(event.key.code)) {
                _nextDir = _keys.getDirection(event.key.code);

                if (!_isRunning || areOpposedDirections(_currDir, _nextDir))
                    _currDir = _nextDir;

                _isRunning = true;
            } else if (event.key.code == _keys.placeBomb) {
                unsigned int bombsLeft = _maxBombs - _map.getBombManager().getNbBomb(_id);
                if (bombsLeft > 0)
                    _map.getBombManager().putBomb (_id, _nLine, _nCol);
            } else if (event.key.code == _keys.remote) {
                if (_hasRemote)
                    _map.getBombManager().destroy (_id);
            }
        break;
        default:
        break;
    }
}

void Player::getTile (int& line, int& col) const
{
    line = _nLine;
    col = _nCol;
}

void Player::die ()
{
    if (!_isDead) {
        _isDead = true;
        _lastUpdate = _timeManager.getTime();
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const int SPRITE_HEIGHT = 46;
    int spriteX, spriteY;

    if (_isDead) {
        spriteY = 4;
        const sf::Time time = _timeManager.getTime() - _lastUpdate;
        spriteX = time.asMilliseconds() / DEATH_ANIMATION_STEP.asMilliseconds();
        if (spriteX >= 3)
            return;
    } else { // !_isDead == alive
        spriteY = static_cast<int>(_currDir);
        spriteX = (_animationStep / 18) % 4;
        spriteX = std::min (spriteX, 4 - spriteX);
        if (!_isRunning)
            spriteX = 1;
    }

    sf::Sprite sprite(_spriteSheet);
    sprite.setOrigin (0, SPRITE_HEIGHT - TILE_SIZE);
    sprite.setTextureRect(sf::IntRect(spriteX*TILE_SIZE, spriteY*SPRITE_HEIGHT,
                                          TILE_SIZE, SPRITE_HEIGHT));
    sprite.setPosition (_nCol*TILE_SIZE + _dCol-MID_TILE,
                        _nLine*TILE_SIZE + _dLine-MID_TILE);
    target.draw (sprite, states);
}

void Player::center (unsigned int& left)
{
    int goalDLine = _dLine, goalDCol = _dCol;

    switch (_currDir) {
        case Dir::LEFT:
            goalDCol = (_dCol <= MID_TILE) ? -MID_TILE : MID_TILE;
        break;
        case Dir::UP:
            goalDLine = (_dLine <= MID_TILE) ? -MID_TILE : MID_TILE;
        break;
        case Dir::RIGHT:
            goalDCol = (_dCol >= MID_TILE) ? TILE_SIZE+MID_TILE : MID_TILE;
        break;
        case Dir::DOWN:
            goalDLine = (_dLine >= MID_TILE) ? TILE_SIZE+MID_TILE : MID_TILE;
        break;
        default:
            std::cerr << "Direction inconnue (Player::center)" << std::endl;
            left = 0;
            return;
        break;
    }

    int iDir = static_cast<int>(_currDir);
    while (left > 0 && (_dLine != goalDLine || _dCol != goalDCol)) {
        _dLine += D_LINE[iDir];
        _dCol += D_COL[iDir];
        --left;
    }

    /* Maybe we left the previous tile, so we need to adjust */
    adjust (_nLine, _dLine);
    adjust (_nCol, _dCol);
}

void Player::adjust (int& nX, int& dX)
{
    while (dX < 0) {
        dX += TILE_SIZE;
        --nX;
        pickBonus();
    }
    while (dX >= TILE_SIZE) {
        dX -= TILE_SIZE;
        ++nX;
        pickBonus();
    }
}

void Player::pickBonus ()
{
    Map::PowerUp bonus = _map.pickBonus(_nLine, _nCol);

    switch (bonus) {
        case Map::PowerUp::BLAST:
            ++_blastForce;
            _map.getBombManager().setBombForce(_id, _blastForce);
        break;
        case Map::PowerUp::BOMB:
            ++_maxBombs;
        break;
        case Map::PowerUp::REMOTE:
            _hasRemote = true;
            _map.getBombManager().setAutoDestruction(_id, false);
        break;
        case Map::PowerUp::NONE:
        break;
        default:
            std::cerr << "Bonus inconnu (Player::center)" << std::endl;
        break;
    }
}
