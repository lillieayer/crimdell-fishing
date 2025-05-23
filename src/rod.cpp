#include "rod.hpp" // Class headerfile
#include "game_constants.hpp" // For rod speed/size
#include "scaling.hpp" // For scaling helpers

Rod::Rod(float speedMultiplier) : position(Vector2D(Dimens::CENTER_X, Dimens::CENTER_Y)), 
             direction(Vector2D(0, 1)), 
             speed(Vector2D(0, Game::ROD_SPEED * Scale::getHeightScaleFactor() * speedMultiplier)), 
             dimens(Vector2D(Game::ROD_RADIUSX_PERCENT, Game::ROD_RADIUSY_PERCENT)),
             velocity(0),
             maxVelocity(Game::ROD_SPEED * Scale::getHeightScaleFactor() * speedMultiplier) {}

/**
 * @brief moves the rod upwards and checks bounds
 */
void Rod::castUp() {
    velocity = -maxVelocity;
    direction[1] = -1;
}

/**
 * @brief moves the rod downwards and checks bounds
 */
void Rod::castDown() {
    velocity = maxVelocity;
    direction[1] = 1;
}

/**
 * @brief moves the rod upwards at a custom speed
 * @param customSpeed The custom speed to use instead of maxVelocity
 */
void Rod::castUpSpeed(float customSpeed) {
    velocity = -customSpeed;
    direction[1] = -1;
}

/**
 * @brief moves the rod downwards at a custom speed
 * @param customSpeed The custom speed to use instead of maxVelocity
 */
void Rod::castDownSpeed(float customSpeed) {
    velocity = customSpeed;
    direction[1] = 1;
}

void Rod::stopMovement() {
    velocity = 0;
}

void Rod::updatePos() {
    float maxRod = Scale::scaleH(Game::ROD_MAX_LENGTH_PERCENT);
    float minRod = Scale::scaleH(Game::ROD_MIN_LENGTH_PERCENT);

    if (velocity != 0) {
        float newY = position[1] + velocity;

        if (newY > minRod && newY < maxRod) {
            position[1] = newY;
        } else {
            if (newY <= minRod) position[1] = minRod;
            if (newY >= maxRod) position[1] = maxRod;
            velocity = 0;
        }
    }
}