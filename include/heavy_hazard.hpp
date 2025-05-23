#ifndef HEAVY_HAZARD_HPP
#define HEAVY_HAZARD_HPP

#include "hazard.hpp"
#include "game_constants.hpp"

/**
 * @class HeavyHazard
 * @brief A slow‐moving hazard that “hooks” to the rod.  Must be reeled to the surface and released
 * to continue fishing.
 */
class HeavyHazard : public Hazard {
    public:
    HeavyHazard(float w, float h, float xSpeed) : Hazard(w, h, xSpeed * 0.75f)
    {
        spriteID = Resource::HAZARD_HEAVY_ID;
    }
    void update() override { Hazard::update(); }
    float getSlowFactor() const {return slowFactor; }

    private:
    const float slowFactor = Game::HEAVY_HAZARD_SLOW_FACTOR;
};

#endif //HEAVY_HAZARD_HPP