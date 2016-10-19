#include "Map.hpp"

#include "Constants.hpp"
#include "ItemGenerator.hpp"

#include <stdexcept>
#include <cassert>
#include <sstream>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

const sf::Color ROCK_COLOR = sf::Color(0,0,0);
const sf::Color BRICK_COLOR = sf::Color(128,128,128);

Map::Map (TimeManager const& timeManager, sf::Image const& grid,
          unsigned int nbBombsBonus, unsigned int nbBlastBonus, unsigned int nbRemoteBonus):
            _timeManager (timeManager),
            _nbLines (grid.getSize().y),
            _nbCol (grid.getSize().x),
            _grid (grid.getSize().x*grid.getSize().y, Tile{Terrain::EMPTY, PowerUp::NONE}),
            _explodingBricks (new ExplodingBrickManager(timeManager)),
            _fireManager (nullptr),
            _bombManager (nullptr),
            _spriteSheetTerrain (),
            _spriteSheetPowerup (),
            _bufferTexture()
{
    /* Loading of the map */
    unsigned int nbBricks = 0;
    for (unsigned int iLine = 0 ; iLine < _nbLines ; ++iLine) {
        for (unsigned int iCol = 0 ; iCol < _nbCol ; ++iCol) {
            if (grid.getPixel(iCol, iLine) == ROCK_COLOR)
                _grid[index(iLine,iCol)].type = Terrain::ROCK;
            else if (grid.getPixel(iCol, iLine) == BRICK_COLOR) {
                _grid[index(iLine,iCol)].type = Terrain::BRICK;
                ++nbBricks;
            }
        }
    }

    /* Placement of the power-ups */
    std::vector<unsigned int> powerUpAmount(3, 0);
    powerUpAmount[static_cast<int>(PowerUp::BOMB)] = nbBombsBonus;
    powerUpAmount[static_cast<int>(PowerUp::BLAST)] = nbBlastBonus;
    powerUpAmount[static_cast<int>(PowerUp::REMOTE)] = nbRemoteBonus;
    ItemGenerator generator(nbBricks, powerUpAmount);
    for (unsigned int iLine = 0 ; iLine < _nbLines ; ++iLine) {
        for (unsigned int iCol = 0 ; iCol < _nbCol ; ++iCol) {
            if (_grid[index(iLine,iCol)].type == Terrain::BRICK) {
                int hiddenPowerup = generator.generate();
                if (hiddenPowerup >= 0)
                    _grid[index(iLine,iCol)].powerUp = static_cast<PowerUp>(hiddenPowerup);
            }
        }
    }

    /* Loading of the sprites */
    std::string path = "rc/map.png";
    if (!_spriteSheetTerrain.loadFromFile(path)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << path << " (Map::Map)";
        throw std::runtime_error (oss.str());
    }
    path = "rc/power_ups.png";
    if (!_spriteSheetPowerup.loadFromFile(path)) {
        std::ostringstream oss;
        oss << "Erreur: impossible de charger " << path << " (Map::Map)";
        throw std::runtime_error (oss.str());
    }

    /* Allocation and update of the background texture */
    if (!_bufferTexture.create (TILE_SIZE*_nbCol, TILE_SIZE*_nbLines))
        throw std::runtime_error("Error: impossible d'initialiser Map._backgroundTexture");
    updateBufferTexture ();

    _fireManager.reset (new FireManager(timeManager, *this));

    _bombManager.reset (new BombManager(timeManager, *_fireManager));
}

unsigned int Map::index (unsigned int line, unsigned int col) const
{
    return line * _nbCol + col;
}

void Map::update()
{
    assert (_explodingBricks);
    _explodingBricks->update ();

    assert (_fireManager);
    _fireManager->update ();

    assert (_bombManager);
    _bombManager->update ();
}

unsigned int Map::getNbLines () const
{
    return _nbLines;
}
unsigned int Map::getNbCol () const
{
    return _nbCol;
}

Map::Terrain Map::getTerrain (unsigned int line, unsigned int col) const
{
    if (line >= _nbLines || col >= _nbCol)
        return Terrain::ROCK;
    return _grid[index(line,col)].type;
}

Map::PowerUp Map::getPowerUp (unsigned int line, unsigned int col) const
{
    if (line >= _nbLines || col >= _nbCol)
        return PowerUp::NONE;
    return _grid[index(line,col)].powerUp;
}

bool Map::isWalkable (unsigned int line, unsigned int col) const
{
    return getTerrain(line, col) == Terrain::EMPTY && !_bombManager->isThereABomb (line, col);
}

bool Map::isLethal (unsigned int line, unsigned int col) const
{
    assert (_fireManager);
    return _fireManager->isOnFire (line, col);
}

BombManager& Map::getBombManager ()
{
    assert (_bombManager);
    return *_bombManager;
}

void Map::burnTile (unsigned int line, unsigned int col)
{
    if (line >= _nbLines || col >= _nbCol)
        return;

    _bombManager->destroy (line, col);

    switch (_grid[index(line,col)].type) {
        case Terrain::BRICK:
            _grid[index(line,col)].type = Terrain::EMPTY;
            updateBufferTexture();

            assert (_explodingBricks);
            _explodingBricks->addBrick (line, col);
        break;
        case Terrain::EMPTY:
            _grid[index(line,col)].powerUp = PowerUp::NONE;
        break;
        default:
        break;
    }
}

Map::PowerUp Map::pickBonus (unsigned int line, unsigned int col)
{
    if (line >= _nbLines || col >= _nbCol)
        return PowerUp::NONE;

    PowerUp returned = _grid[index(line,col)].powerUp;
    _grid[index(line,col)].powerUp = PowerUp::NONE;
    return returned;
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    /* Drawing of the static precomputed background : rock, brick, empty */
    {
        sf::Sprite sprite (_bufferTexture.getTexture());
        sprite.setScale(1.f, -1.f);
        sprite.setPosition (0.f, _bufferTexture.getSize().y);
        target.draw (sprite, states);
    }

    /* Drawing of the dynamic power-ups (only on EMPTY terrain) */
    sf::Sprite sprite (_spriteSheetPowerup);

    int step = (_timeManager.getTime().asMilliseconds() / POWERUP_ANIMATION_STEP.asMilliseconds()) % 6;
    step = std::min (step, 6 - step);
    for (unsigned int iLine = 0 ; iLine < _nbLines ; ++iLine) {
        for (unsigned int iCol = 0 ; iCol < _nbCol ; ++iCol) {
            if (_grid[index(iLine,iCol)].type != Terrain::EMPTY)
                continue;
            if (_grid[index(iLine,iCol)].powerUp == PowerUp::NONE)
                continue;

            int spriteY = static_cast<int>(_grid[index(iLine,iCol)].powerUp);
            sprite.setTextureRect(sf::IntRect(step*TILE_SIZE, spriteY*TILE_SIZE,
                                              TILE_SIZE, TILE_SIZE));
            sprite.setPosition(iCol*TILE_SIZE, iLine*TILE_SIZE);
            target.draw (sprite, states);
        }
    }

    assert (_explodingBricks);
    target.draw (*_explodingBricks, states);

    assert (_bombManager);
    target.draw (*_bombManager, states);

    assert (_fireManager);
    target.draw (*_fireManager, states);
}

void Map::updateBufferTexture ()
{
    _bufferTexture.clear (sf::Color::Green);

    sf::Sprite sprite (_spriteSheetTerrain);

    for (unsigned int iLine = 0 ; iLine < _nbLines ; ++iLine) {
        for (unsigned int iCol = 0 ; iCol < _nbCol ; ++iCol) {
            unsigned int i = static_cast<int>(_grid[index(iLine,iCol)].type);

            sprite.setTextureRect(sf::IntRect(0, i*TILE_SIZE,
                                              TILE_SIZE, TILE_SIZE));

            sprite.setPosition (TILE_SIZE * iCol, TILE_SIZE * iLine);
            _bufferTexture.draw (sprite);
        }
    }
}
