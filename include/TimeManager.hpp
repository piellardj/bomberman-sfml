#ifndef TIMEMANAGER_HPP_INCLUDED
#define TIMEMANAGER_HPP_INCLUDED

#include <SFML/System/Clock.hpp>

/** Class to be used as a clock.
 *  It is a lightweight wrapper of a sf::Clock,
 *  adding the possibility of pausing or stretching time.
 */
class TimeManager
{
    public:
        TimeManager ();

        /** Sets the speed.
         * @param speed Must be > 0.f. If not, a warning
         *              message is written in cerr.
         */
        void setSpeed (float speed);

        /** Pauses the clock. If already paused,
         *  an error message is written is cerr.
         */
        void pause ();

        /** Resumes the clock. If already resumed,
         *  an warning message is written is cerr.
         */
        void resume ();

        sf::Time getTime () const;

        bool isPaused () const;

    private:
        /** Updates the cached value of the time. */
        void update ();

    private:
        float _speed; //garanteed > 0.f
        bool _paused;

        sf::Clock _clock;
        sf::Time _time; //cached value of the time
};

#endif // TIMEMANAGER_IHPP_NCLUDED
