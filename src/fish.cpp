// Fish class, inherits from Floater
#include "fish.hpp"
#include "view_adapter.hpp"
#include <iostream>

/**
 * @brief fish implementation for updating path
 */
void Fish::update() {

    position += (direction * speed);
};
