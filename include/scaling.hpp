#ifndef SCALING_HPP
#define SCALING_HPP

#include "game_constants.hpp"

/**
 * @namespace Scale
 * @brief Basic utilities for scaling visual elements
 */
 namespace Scale
 {
    /**
     * @brief Scale width based on resolution
     * @param widthPercent Width value to scale as a percentiage of screen width
     * @return Scaled width in pixels
     */
    inline float scaleW(float widthPercent) {
        return Dimens::SCREEN_W * widthPercent;
    }

    /** 
     * @brief Scale height based on resolution
     * @param heightPercent Height value to scale as a percentage of screen width
     * @return Scaled height in pixels
     */
    inline float scaleH(float heightPercent) {
        return Dimens::SCREEN_H * heightPercent;
    }

     /**
     * @brief Scale X position based on reference resolution
     * @param x X position to scale as a percentage of screen width (0.0 to 1.0)
     * @return Scaled X position in pixels
     */
    inline float scaleX(float xPercent) {
        return Dimens::SCREEN_W * xPercent;
    }

    /**
     * @brief Scale Y position based on reference resolution
     * @param y Y position to scale as a percentage of screen height (0.0 to 1.0)
     * @return Scaled Y position in pixels
     */
    inline float scaleY(float yPercent) {
        return Dimens::SCREEN_H * yPercent;
    }

    /**
     * @brief Get scaling factor for width
     * @return Width scaling factor
     */
    inline float getWidthScaleFactor() {
        return Dimens::SCREEN_W / 1024.0f; // 1024 is the reference width
    }

    /**
     * @brief Get scaling factor for height
     * @return Height scaling factor
     */
    inline float getHeightScaleFactor() {
        return Dimens::SCREEN_H / 768.0f; // 768 is the reference height
    }

    /**
    * @brief Scale a font size based on resolution height
    * @param baseSize Base font size to scale
    * @return Scaled font size in pixels
    */
   inline int scaleFontSize(int baseSize) {
       return static_cast<int>(baseSize * getHeightScaleFactor());
   }
}

 #endif