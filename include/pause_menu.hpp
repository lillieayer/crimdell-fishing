#ifndef PAUSE_MENU_HPP
#define PAUSE_MENU_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <functional>
#include <string>
#include "text_box.hpp"
#include "fishing_logic.hpp"

class PauseMenu {
public:
    PauseMenu(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* hoverFont, int screenWidth, int screenHeight);
    ~PauseMenu() = default;
    
    // Core methods
    void render();
    bool handleEvent(SDL_Event& e);
    
    // Set the fishing logic pointer to control pause state
    void setFishingLogic(std::shared_ptr<FishingLogic> logicPtr) { fishingLogic = logicPtr; }
    void setFonts(TTF_Font* newFont, TTF_Font* newHoverFont);
    
    void setScreenDimensions(int newWidth, int newHeight);
    
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* hoverFont;
    int screenWidth;
    int screenHeight;
    
    // Pause menu buttons
    TextBox resumeButton;
    TextBox quitButton;
    
    // Reference to fishing logic to control pause state
    std::shared_ptr<FishingLogic> fishingLogic;
    
    // Handle button clicks
    void mouseClick(int x, int y);
};

#endif // PAUSE_MENU_HPP