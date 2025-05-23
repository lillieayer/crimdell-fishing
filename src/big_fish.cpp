#include "big_fish.hpp"

void BigFish::update() {
// Same update behavior as Fish.
    position += (direction * speed);
}