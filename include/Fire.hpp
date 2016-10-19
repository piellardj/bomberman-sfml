#ifndef FIRE_HPP_INCLUDED
#define FIRE_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <array>

#include "TimeManager.hpp"

/** A fire is an explosion. It has 4 branches spreading
 *  in 4 directions (left, up, right, down).
 *  A fire spreads at a given speed (see Utilities.hpp)
 *  and remains a certain time once it's fully spread.
 *
 *  A Fire keeps a reference to a TimeManager, a Texture and a Map,
 *  you must ensure they are not deleted elsewhere.
 */
class Map;
class Fire: public sf::Drawable
{
    public:
        Fire (TimeManager const& timeManager, Map& map, sf::Texture const& spriteSheet,
              int oLine, int oCol, unsigned int force);

        /** Spreads the explosion.
         *  @return whether or one of the branches lengthened */
        bool spread ();

        /** @return whether or not the fire has been fully spread
         *          for enough time. */
        bool shouldBeDeleted () const;

        /** @return whether or not the given location is within
         *          the explosion's fire */
        bool isOnFire (int line, int col) const;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        bool isFullySpread () const;

    private:
        TimeManager const& _timeManager;
        Map& _map;

        const int _oLine, _oCol; //origin of the fire
        const int _force; //max length of a branch
        sf::Time _lastUpdate;

        struct Branch {
            int length;
            bool fullySpread;//true when the branch can no longer lengthen
        };
        std::array<Branch, 4> _branches;

        sf::Texture const& _spriteSheet;
};

#endif // FIRE_HPP_INCLUDED
