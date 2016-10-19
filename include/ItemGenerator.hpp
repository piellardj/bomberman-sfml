#ifndef ITEMGENERATOR_HPP_INCLUDED
#define ITEMGENERATOR_HPP_INCLUDED

#include <vector>

/** Class used to randomly generate items when a brick is destroyed. */
class ItemGenerator
{
    public:
        /** Constructor.
         *  @param nbTries amount of bricks to be destroyed
         *  @param nbItems For each item i in 0 .. nbItems.size()-1,
                           nbItems[i] = the amount of this item in the map.
         */
        ItemGenerator (unsigned int nbTries, std::vector<unsigned int> const& nbItems);

        /** Tries and see if an item is generated.
         *  @return -1 if no item is generated
         *          item number else
         */
        int generate ();

    private:
        unsigned int _nbTriesLeft;
        std::vector<unsigned int> _nbItemsLeft;
};

#endif // ITEMGENERATOR_HPP_INCLUDED
