// Fish class, inherits from Floater
#ifndef FISH_HPP
#define FISH_HPP
#include "floater.hpp"
#include "view_adapter.hpp"
#include "game_constants.hpp"

class Fish : public Floater { 
    public:
        Fish(float w, float h, float xSpeed, const std::string& spriteID = Resource::FISH_SMALL_ID) : Floater(w, h, xSpeed, spriteID) {}
        void update() override;
        
        // Animation-related methods
        virtual std::string getAnimFrame1ID() const { return Resource::FISH_SMALL_ANIM1_ID; }
        virtual std::string getAnimFrame2ID() const { return Resource::FISH_SMALL_ANIM2_ID; }
};

#endif // FISH_HPP