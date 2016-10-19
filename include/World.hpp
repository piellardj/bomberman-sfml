#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <memory>
#include <array>

#include "TimeManager.hpp"
#include "Map.hpp"
#include "Player.hpp"

/** Top level class hosting the map and the players. */
class World: public sf::Drawable
{
    public:
        World (std::string const& gridPath);

        unsigned int widthInPixels () const;
        unsigned int heightInPixels () const;

        void update ();

        void manageEvents (sf::Event const& event);

   private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        TimeManager _timeManager;

        std::unique_ptr<Map> _map;
        std::array<std::unique_ptr<Player>, 2> _players;
};

#endif // WORLD_HPP_INCLUDED
