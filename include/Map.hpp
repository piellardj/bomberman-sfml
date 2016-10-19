#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <vector>
#include <memory>

#include "TimeManager.hpp"
#include "ExplodingBrickManager.hpp"
#include "FireManager.hpp"
#include "BombManager.hpp"

/** Class for managing the world the players exist in.
 *  It stores: - the terrain (ROCK, BRICK, EMPTY)
 *             - the powerups
 *             - the bombs
 *             - the explosions (fire)
 *
 *  To save time, the map is rendered on a off-screen texture,
 *  update only when the map changes.
 *
 *  Each tile has the size TILE_SIZE (defined in Constants.hpp).
 *
 *  A Map keeps a reference to a TimeManager,
 *  you must ensure it is not deleted elsewhere.
 */
class Map: public sf::Drawable
{
    public:
        /* ROCK: undestroyable, BRICK: destroyable.
         * The order is important for the drawing */
        enum class Terrain {ROCK=0, BRICK=1, EMPTY=2};
        enum class PowerUp {BOMB=0, BLAST=1, REMOTE=2, NONE=3};

    public:
        /** @throw std::runtime_error if the resources couldn't be loaded */
        Map (TimeManager const& timeManager, sf::Image const& grid,
             unsigned int nbBombsBonus=20, unsigned int nbBlastBonus=20, unsigned int nbRemoteBonus=7);

        void update ();

        unsigned int getNbLines () const;
        unsigned int getNbCol () const;

        BombManager& getBombManager ();

        /** Returns the demanded tile's terrain.
         * if the coordinates are off-map, Terrain::ROCK is returned. */
        Terrain getTerrain (unsigned int line, unsigned int col) const;

        /** Returns the demanded tile's powerup.
         * if the coordinates are off-map, PowerUp::NONE is returned. */
        PowerUp getPowerUp (unsigned int line, unsigned int col) const;

        /** Returns whether or not a tile is walkable (Terrain::EMPTY).
         *  if the coordinates are off-map, false is returned */
        bool isWalkable (unsigned int line, unsigned int col) const;

        /** Returns whether or not a tile is lethal (fire). */
        bool isLethal (unsigned int line, unsigned int col) const;

        /** Burns a tile:
         *  - triggers bomb explosion
         *  - destroys Terrain::BRICK and powerup
         */
        void burnTile (unsigned int line, unsigned int col);

        /** Removes the bonus from the map and returns it */
        PowerUp pickBonus (unsigned int line, unsigned int col);

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        /** Converts 2D coordinates to 1D index (to be used on _grid).
         *  Since the grid is stored in a 1D array, we need a conversion method.
         *  DOES NOT check whether or not the point is off-grid.
         */
        unsigned int index (unsigned int line, unsigned int col) const;

        /** Updates the offescreen buffer texture.
         *  This texture should only updated when the map is updated. This
         *  avoid a slow tile by tile drawing of the map.
         */
        void updateBufferTexture ();

    private:
        TimeManager const& _timeManager;

        unsigned int      _nbLines,
                          _nbCol;
        struct Tile {
            Terrain type;
            PowerUp powerUp;
        };
        std::vector<Tile> _grid;

        std::unique_ptr<ExplodingBrickManager> _explodingBricks;
        std::unique_ptr<FireManager> _fireManager;
        std::unique_ptr<BombManager> _bombManager;

        sf::Texture       _spriteSheetTerrain;
        sf::Texture       _spriteSheetPowerup;
        sf::RenderTexture _bufferTexture;
};

#endif // MAP_HPP_INCLUDED
