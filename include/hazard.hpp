// Hazard class, inherits from floater
#ifndef HAZARD_HPP
#define HAZARD_HPP
#include "floater.hpp"
#include "view_adapter.hpp"
#include "game_constants.hpp"

class Hazard : public Floater {
    public:
        Hazard(float w, float h, float xSpeed) : Floater(w, h, xSpeed * 2.5f, Resource::HAZARD_BARRACUDA_ID) {}
        void update() override;

        virtual std::string getAnimFrame1ID() const { return Resource::HAZARD_BARRACUDA_ANIM1_ID; }
        virtual std::string getAnimFrame2ID() const { return Resource::HAZARD_BARRACUDA_ANIM2_ID; }
};

#endif // HAZARD_HPP