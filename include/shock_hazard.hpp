#ifndef SHOCK_HAZARD_HPP
#define SHOCK_HAZARD_HPP

#include "hazard.hpp"

/**
 * @class ShockHazard
 * @brief A hazard that stuns the rod for 2s and resets its position to the top.
 */
class ShockHazard : public Hazard {
public:
    ShockHazard(float w, float h, float xSpeed) : Hazard(w, h, xSpeed * 1.25f)
    {
        spriteID = Resource::HAZARD_SHOCK_ID;
    }

    void update() override { Hazard::update(); }
};

#endif // SHOCK_HAZARD_HPP