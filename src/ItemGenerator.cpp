#include "ItemGenerator.hpp"

#include <iostream>
#include <cstdlib>

ItemGenerator::ItemGenerator(unsigned int nbTries, std::vector<unsigned int> const& nbItems):
            _nbTriesLeft (nbTries),
            _nbItemsLeft (nbItems)
{
    unsigned int sum = 0;
    for (unsigned int nb : _nbItemsLeft)
        sum += nb;
    if (nbTries < sum)
        std::cerr << "ATTENTION: tous les items ne pourront être générés (ItemGenerator::ItemGenerator)" << std::endl;

    srand(time(NULL));
}

int ItemGenerator::generate ()
{
    if (_nbTriesLeft == 0) {
        std::cerr << "ATTENTION: aucun autre item ne peut être généré (ItemGenerator::generate)" << std::endl;
        return -1;
    }

    unsigned int sum = 0;
    for (unsigned int nb : _nbItemsLeft)
        sum += nb;

    unsigned int rMax = std::max (sum, _nbTriesLeft);
    unsigned int random = rand () % rMax;

    int returnedItem = -1;
    if (sum > random) {
        sum = 0;
        while (sum <= random) {
            ++returnedItem;
            sum += _nbItemsLeft[returnedItem];
        }
        --_nbItemsLeft[returnedItem];
    }

    --_nbTriesLeft;
    return returnedItem;
}
