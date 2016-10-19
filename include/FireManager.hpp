#ifndef FIREMANAGER_HPP_INCLUDED
#define FIREMANAGER_HPP_INCLUDED

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Time.hpp>

#include <list>

#include "TimeManager.hpp"
#include "Fire.hpp"

/** Manages the explosions (see Fire) taking place on the map:
 *  their spreading and their death.
 *
 *  To save time, the explosions are rendered on a off-screen texture,
 *  updated only when needed.
 *
 *  A FireManager keeps a reference to a TimeManager and a Map,
 *  you must ensure they are not deleted elsewhere.
 */
class Map;
class FireManager: public sf::Drawable
{
    public:
        /** @throws std::runtime_error if the resources couldn't be loaded */
        FireManager (TimeManager const& timeManager, Map& map);

        /** Adds a fire on the map.
         *  @param force maximum length of a branch
         */
        void addFire (int line, int col, unsigned int force);

        /** Spreads fires and destroys old ones */
        void update ();

        /** Tells whether or not the given position is within an explosion */
        bool isOnFire (int line, int col) const;

   private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        void updateBufferTexture ();

    private:
        TimeManager const& _timeManager;
        Map& _map;

        std::list<Fire> _fires;

        sf::Texture _spriteSheet;
        sf::RenderTexture _bufferTexture;
};

#endif
