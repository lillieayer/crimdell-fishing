// Floater class, parent of Fish and Hazard
#ifndef FLOATER_HPP
#define FLOATER_HPP
#include "view_adapter.hpp"
#include "Vector2D.hpp"
#include <string>

class Floater {

// Note: to coordinate w sdl, xy pos represents top rightmost coordinate of floater object

    protected:
        Vector2D position;
        Vector2D direction;
        Vector2D speed;
        Vector2D dimens;
        bool caught = false;
        bool active = false; 
        std::string spriteID;

    public:
        Floater(float w, float h, float xSpeed, const std::string& sprite = "")
         : position(Vector2D(-100,-100)), direction(Vector2D(1, 1)), speed(Vector2D(xSpeed, 0)),
          dimens(Vector2D(w, h)), spriteID(sprite) {}

        virtual ~Floater(){}
        void setCaught(bool newState) { caught = newState; }
        bool isCaught() const { return caught; };
        void setActive(bool newState) { active = newState; }
        bool isActive() const { return active; };

        // Sprite management
        const std::string& getSpriteID() const { return spriteID; }
        
        // Getter & Setter methods to access positional information (2d vectors)
        Vector2D getPosition() { return position; }
        void setPosition(const Vector2D& newPos) { position = newPos; }
        void setPosition(float x, float y) { position[0] = x; position[1] = y; }
        
        Vector2D getDirection() { return direction; }
        void setDirection(const Vector2D& newPos) { direction = newPos; }
        void setDirection(float x, float y) { direction[0] = x; direction[1] = y; }
        
        Vector2D getSpeed() { return speed; }
        void setSpeed(const Vector2D& newPos) { speed = newPos; }
        void setSpeed(float x, float y) { speed[0] = x; speed[1] = y; }
        
        Vector2D getDimens() { return dimens; }
        void setDimens(const Vector2D& newPos) { dimens = newPos; }
        void setDimens(float x, float y) { dimens[0] = x; dimens[1] = y; }
        virtual void update() = 0;
};

#endif // FLOATER_HPP 