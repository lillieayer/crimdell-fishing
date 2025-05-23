#ifndef ROD_HPP
#define ROD_HPP
#include "view_adapter.hpp"
#include "game_constants.hpp"
#include "Vector2D.hpp"

class Rod 
{
    private: 
        // note xy denotes the pos of the hook (start hook in center)
        Vector2D position;
        Vector2D direction;
        Vector2D speed;
        Vector2D dimens;
        float velocity;
        const float maxVelocity;

    public: 
        Rod(float speedMultipier); // default
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
        
        void castUp();
        void castDown();        
        void castUpSpeed(float customSpeed);
        void castDownSpeed(float customSpeed);
        void stopMovement();
        void updatePos();
        float getMaxVelocity() const { return maxVelocity; }
};

#endif // ROD_HPP