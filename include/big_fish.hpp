#ifndef BIG_FISH_HPP
#define BIG_FISH_HPP
#include "fish.hpp"

class BigFish : public Fish {
public:
    BigFish(float w, float h, float xSpeed) : Fish(w, h, xSpeed, Resource::FISH_BIG_ID) {}
    void update() override;
    
    // Override animation frame IDs
    std::string getAnimFrame1ID() const override { return Resource::FISH_BIG_ANIM1_ID; }
    std::string getAnimFrame2ID() const override { return Resource::FISH_BIG_ANIM2_ID; }
};

#endif // BIG_FISH_HPP