// Render fishing phase
#ifndef FISHING_VIEW_HPP
#define FISHING_VIEW_HPP
#include "fishing_logic.hpp"
#include "view_adapter.hpp"
#include "pause_menu.hpp"
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "texture_manager.hpp"
using namespace std;

class FishingView{
    private:
        int screenW, screenH;
        int effectiveLevel;
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;
        TTF_Font* hoverFont;  // Added hover font for pause menu
        const shared_ptr<FishingLogic> logicPtr;
        TextureManager textureManager;
        PauseMenu pauseMenu;
        
        void drawFish();
        void drawHazard();
        void drawFishingRod();
        void drawScore();
        void drawBackground();
        void drawTimer();
        void drawHitboxes();
        void drawStyledBox(int x, int y, int width, int height, const std::string& text);
        bool loadSprites();
        bool showHitboxes = false;
        
    public:
        FishingView(const std::shared_ptr<FishingLogic>& logicPtr, SDL_Window* window, SDL_Renderer* renderer, float screenH, float screenW);
        ~FishingView();
        void drawView(); 
        UserInput::Action handleKeyboardInput(SDL_Event &e);
        void toggleHitboxes();
        UserInput::Action handleContinuousInput();
        void handlePauseMenuInput(SDL_Event &e);
        void updateResolution(float newWidth, float newHeight);
};

#endif // FISHING_VIEW_HPP