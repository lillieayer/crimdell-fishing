#ifndef MEDIUM_FISH_HPP
#define MEDIUM_FISH_HPP
#include "fish.hpp"

class MediumFish : public Fish {
public:
    MediumFish(float w, float h, float xSpeed) : Fish(w, h, xSpeed, Resource::FISH_MEDIUM_ID) {}
    void update() override;
    
    // Override animation frame IDs
    std::string getAnimFrame1ID() const override { return Resource::FISH_MEDIUM_ANIM1_ID; }
    std::string getAnimFrame2ID() const override { return Resource::FISH_MEDIUM_ANIM2_ID; }
};

#endif // MEDIUM_FISH_HPP