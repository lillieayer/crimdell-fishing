#include "medium_fish.hpp"

void MediumFish::update() {
// Same update behavior as Fish.
    position += (direction * speed);
}