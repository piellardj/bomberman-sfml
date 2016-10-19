#ifndef EXPLODINGBRICK_HPP_INCLUDED
#define EXPLODINGBRICK_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <list>

#include "TimeManager.hpp"

/** Class for managing the animation of exploding bricks.
 *
 *  A ExplodingBrick keeps a reference to a TimeManager,
 *  you must ensure it is not deleted elsewhere.
 */
class ExplodingBrickManager: public sf::Drawable
{
    public:
        /** @throws std::runtime_error if the resources couldn't be loaded */
        ExplodingBrickManager (TimeManager const& timeManager);

        void addBrick (unsigned int line, unsigned int col);

        /** Deletes old bricks */
        void update ();

    private:
        void draw (sf::RenderTarget &target, sf::RenderStates states) const;

    private:
        TimeManager const& _timeManager;

        struct ExplodingBrick {
            const unsigned int line, col;
            const sf::Time birthtime;
        };
        std::list<ExplodingBrick> _bricks;

        sf::Texture _spriteSheet;
};

#endif // EXPLODINGBRICK_HPP_INCLUDED
