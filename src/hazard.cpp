// Hazard class, inherits from floater
#include "hazard.hpp"
#include "view_adapter.hpp"
#include <iostream>

void Hazard::update() {
    position += (direction * speed);
};