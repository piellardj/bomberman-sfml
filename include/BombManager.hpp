#ifndef BOMBMANAGER_HPP_INCLUDED
#define BOMBMANAGER_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <list>
#include <map>

#include "TimeManager.hpp"
#include "Constants.hpp"

/** Manages bombs and their destruction.
 *
 *  Each bomb is associated to a PlayerId that must have been
 *  previously declared with addPlayerProfile. Along the PlayerId
 *  is stored information such as blast force and auto-destruction.
 *
 *  Creates explosions (Fire) at the destruction of a bomb.
 *  By default a bomb explodes after a given time (see Constants.hpp),
 *  but a PlayerProfile must state that bombs don't explodes automatically.
 *
 *  Garantees at most 1 bomb per square.
 *
 *  A BombManager keeps a reference to a TimeManager and a FireManager,
 *  you must ensure they are not deleted elsewhere.
 */
class FireManager;
class BombManager: public sf::Drawable
{
    private:
        /* These structures must be declared here because std::map and std::list
         * don't support forward declaration */
        struct PlayerProfile {
            unsigned int bombForce;
            bool autoDestruction;
        };

        struct Bomb {
            const unsigned int line, col;
            const PlayerId player;
            const sf::Time birthtime;
        };

    public:
        /** @throws std::runtime_error if the resources couldn't be loaded */
        BombManager (TimeManager const& timeManager, FireManager& fireManager);

        /** Adds a bomb to the manager.
         *  The PlayerId must have been previously declared.
         *  At most one bomb per position (line, col).
         */
        void putBomb (PlayerId player, unsigned int line, unsigned int col);

        /** Returns whether or not a bomb exists at the given location */
        bool isThereABomb (unsigned int line, unsigned int col) const;

        /** If a bomb exists at the given position, destroys it. */
        void destroy (unsigned int line, unsigned int col);

        /** Destroys all bombs belonging to the given player.
         *  The PlayerId must have been previously declared.
         */
        void destroy (PlayerId player);

        /** Declaration of a PlayerId. */
        void addPlayerProfile (PlayerId player, unsigned int bombForce, bool autoDestruction);
        void setAutoDestruction (PlayerId player, bool activation);
        void setBombForce (PlayerId player, unsigned int force);

        /** @return The number of bombs owned by the player currently on the map */
        unsigned int getNbBomb (PlayerId player) const;

        void update ();

    private:
        void draw (sf::RenderTarget &target, sf::RenderStates states) const;

        /** Removes the bomb from the list and creates an explosion */
        std::list<Bomb>::iterator destroy (std::list<Bomb>::iterator bomb);

    private:
        TimeManager const& _timeManager;

        FireManager& _fireManager;

        std::map<PlayerId, PlayerProfile> _profiles;
        std::list<Bomb> _bombs;

        sf::Texture _spriteSheet;
};

#endif // BOMBMANAGER_HPP_INCLUDED
