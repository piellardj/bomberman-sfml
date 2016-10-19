#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

#include <array>
#include <string>

#include "TimeManager.hpp"
#include "Map.hpp"
#include "Constants.hpp"
#include "PlayerKeyBinding.hpp"

/** Class handling the Bomberman, his interactions with the map and the player.
 *
 *  To each Player is associated a unique PlayerId
 *  (see Constants.hpp, BombManager.hpp).
 *
 *  A Player is controlled with the keyboard (cf PlayerKeyBinding.hpp).
 *
 *  A player is locailized on two levels:
 *  - location on the map (on which tile is it : _nLine, _nCol
 *  - location inside the tile: _dLine, _dCol, within 0..TILE_SIZE-1
 *
 *  A player can be either dead or alive ; a dead Player is no longer
 *  controlable.
 *
 *  A Player keeps a reference to a TimeManager and a Map,
 *  you must ensure they are not deleted elsewhere.
 */
class Player: public sf::Drawable
{
    public:
        /** @throws std::runtime_error if the resources couldn't be loaded */
        Player (PlayerId id, TimeManager const& timeManager, Map& map,
                PlayerKeyBinding const& keys, std::string const& spritesPath,
                int line, int col,
                unsigned int blast=1, unsigned int maxBombs=1, bool hasRemote=false);

        void update ();

        void manageEvents (sf::Event const& event);

        void getTile (int& line, int& col) const;

        void die ();

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        /** Goes to the nearest tile center in the direction _currDir */
        void center (unsigned int &left);

        /** Adjusts dX to be within the range 0 .. TILE_SIZE-1
         *  and updates nX. */
        void adjust (int& nX, int& dX);

        /** Picks the bonus of the current tile on the map. */
        void pickBonus ();

    private:
        TimeManager const& _timeManager;
        Map& _map;
        const PlayerKeyBinding _keys;
        const PlayerId _id;

        sf::Time _lastUpdate;
        bool     _isDead;
        unsigned int _animationStep; //for the walking animation
        sf::Texture _spriteSheet;

        int _nLine, _nCol; //current tile's position
        int _dLine, _dCol; //position in the tile (0 .. TILE_SIZE-1)

        bool _isRunning;
        Dir _currDir, _nextDir;

        /* Bonuses */
        unsigned int _blastForce;
        unsigned int _maxBombs;
        bool _hasRemote;
};

#endif // PLAYER_HPP_INCLUDED
