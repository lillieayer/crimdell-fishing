// Update the FishingView constructor to initialize the pause menu

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <sstream>
#include <iomanip>
#include <iostream> // For debug messages

#include "fishing_view.hpp" // Class headerfile
#include "fishing_logic.hpp" // For gamestate
#include "fish.hpp" // For fish class
#include "medium_fish.hpp" // For med fish class
#include "big_fish.hpp" // For big fish class
#include "hazard.hpp" // For hazard class
#include "heavy_hazard.hpp"
#include "shock_hazard.hpp"
#include "rod.hpp" // For rod class
#include "game_constants.hpp" // For visuals scaling constants and sprite filepaths
#include "scaling.hpp" // For scaling helpers
#include "pause_menu.hpp" // For pause menu

/**
 * @brief basic error thrower
 * @param msg Reference to error message
 */
void throwError(const std::string &msg)
    {
        std::cerr << msg << " (" << SDL_GetError() << ")" << std::endl;
        exit(0);
    }

/**
 * @brief View constructor, attempts to load font, sprites, TextureManager
 */
FishingView::FishingView(const std::shared_ptr<FishingLogic> &logicPtr, SDL_Window* window, SDL_Renderer* renderer, float screenH, float screenW)
    : logicPtr(logicPtr), 
      window(window), 
      renderer(renderer), 
      screenH(screenH), 
      screenW(screenW), 
      textureManager(renderer), 
      showHitboxes(false),
      pauseMenu(renderer, nullptr, nullptr, Dimens::SCREEN_W, Dimens::SCREEN_H)
      {

    // init TTF - should be redundant but game crashes without it
    if(TTF_Init() < 0) throwError("Unable to initialize TTF!");
    int scaledFontSize = Scale::scaleFontSize(Visuals::FONT_SIZE);
    font = TTF_OpenFont(Resource::WATERLILY.c_str(), scaledFontSize);
    int scaledHoverFontSize = Scale::scaleFontSize(Visuals::HOVER_FONT_SIZE);
    hoverFont = TTF_OpenFont(Resource::ARIAL.c_str(), scaledHoverFontSize);
    
    // Update pause menu with loaded fonts instead of reinitializing
    pauseMenu.setFonts(font, hoverFont);
    pauseMenu.setFishingLogic(logicPtr);

    if (!loadSprites()) {
        std::cerr << "Failed to load sprites!" << std::endl;
    }

    // Load regular background
    if (!textureManager.loadTexture(Resource::BG_FISHING_ID, Resource::BG_FISHING)) {
        std::cerr << "Failed to load fishing background" << std::endl;
    }
    
    // Load animated background frames
    if (!textureManager.loadTexture(Resource::BG_FISHING_ANIM1_ID, Resource::BG_FISHING_ANIM1)) {
        std::cerr << "Failed to load fishing background animation frame 1" << std::endl;
    }
    
    if (!textureManager.loadTexture(Resource::BG_FISHING_ANIM2_ID, Resource::BG_FISHING_ANIM2)) {
        std::cerr << "Failed to load fishing background animation frame 2" << std::endl;
    }
}

/**
 * @brief View destructor, closes font
 */
FishingView::~FishingView()
{
    TTF_CloseFont(font);
    TTF_CloseFont(hoverFont);
    // quit text SDL
}

/**
 * @brief Loads sprites into texture manager
 * @return bool true if successful, false if not
 */
bool FishingView::loadSprites() {
    bool success = true;

    // Load regular fish sprites (for caught fish)
    success &= textureManager.loadTexture(Resource::FISH_SMALL_ID, Resource::FISH_SMALL);
    success &= textureManager.loadTexture(Resource::FISH_MEDIUM_ID, Resource::FISH_MEDIUM);
    success &= textureManager.loadTexture(Resource::FISH_BIG_ID, Resource::FISH_BIG);
    
    // Load animated fish sprites
    success &= textureManager.loadTexture(Resource::FISH_SMALL_ANIM1_ID, Resource::FISH_SMALL_ANIM1);
    success &= textureManager.loadTexture(Resource::FISH_SMALL_ANIM2_ID, Resource::FISH_SMALL_ANIM2);
    success &= textureManager.loadTexture(Resource::FISH_MEDIUM_ANIM1_ID, Resource::FISH_MEDIUM_ANIM1);
    success &= textureManager.loadTexture(Resource::FISH_MEDIUM_ANIM2_ID, Resource::FISH_MEDIUM_ANIM2);
    success &= textureManager.loadTexture(Resource::FISH_BIG_ANIM1_ID, Resource::FISH_BIG_ANIM1);
    success &= textureManager.loadTexture(Resource::FISH_BIG_ANIM2_ID, Resource::FISH_BIG_ANIM2);
    
    // Load hazard sprites
    success &= textureManager.loadTexture(Resource::HAZARD_HEAVY_ID, Resource::HAZARD_HEAVY);
    success &= textureManager.loadTexture(Resource::HAZARD_SHOCK_ID, Resource::HAZARD_SHOCK);
    success &= textureManager.loadTexture(Resource::HAZARD_BARRACUDA_ID, Resource::HAZARD_BARRACUDA);
    success &= textureManager.loadTexture(Resource::HAZARD_BARRACUDA_ANIM1_ID, Resource::HAZARD_BARRACUDA_ANIM1);
    success &= textureManager.loadTexture(Resource::HAZARD_BARRACUDA_ANIM2_ID, Resource::HAZARD_BARRACUDA_ANIM2);

    return success;
}

/**
 * @brief Draws each fish in fish vector
 */
void FishingView::drawFish()
{
    // get ref since expensive
    std::vector<std::shared_ptr<Fish>>& allFish = logicPtr->getAllFishRef();
    for(int i = 0; i < allFish.size(); i++)
    {
        if (!allFish[i]->isActive()) continue;
        
        int x = allFish[i]->getPosition()[0];
        int y = allFish[i]->getPosition()[1];
        int w = allFish[i]->getDimens()[0];
        int h = allFish[i]->getDimens()[1];

        // Get correct sprite direction
        SDL_RendererFlip flip = (allFish[i]->getDirection()[0] > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        // Get the appropriate sprite IDs from the fish itself
        std::string baseId = allFish[i]->getSpriteID();
        std::string anim1Id = allFish[i]->getAnimFrame1ID();
        std::string anim2Id = allFish[i]->getAnimFrame2ID();
        
        // Draw sprite
        if (allFish[i]->isCaught()) {
            // Caught fish uses static texture
            // Swap dimensions back
            int oldW = h;
            int oldH = w;

            Rod rod = logicPtr->getRod();
            int rodX = rod.getPosition()[0];
            int rodY = rod.getPosition()[1];

            // Make fish hang from mouth
            int adjX = rodX - oldW / 2;
            int adjY = rodY + oldH;

            // Rotate based on direction
            double angle = (allFish[i]->getDirection()[0] > 0) ? -90.0 : 90.0;

            textureManager.drawRotatedTexture(baseId, adjX, adjY, oldW, oldH, angle, flip);
        } else {
            // Swimming fish uses animated texture
            textureManager.drawAnimatedTexture(baseId, anim1Id, anim2Id, x, y, w, h, Game::FISH_ANIMATION_DELAY, flip);
        }
    }
}

/**
 * @brief Draws each hazard in hazard vector
 */
void FishingView::drawHazard()
{
    std::vector<std::shared_ptr<Hazard>>& hazards = logicPtr->getHazardsRef();
    for (int i = 0; i < hazards.size(); i++)
    {
        if (!hazards[i]->isActive()) continue;
        
        int x = hazards[i]->getPosition()[0];
        int y = hazards[i]->getPosition()[1];
        int w = hazards[i]->getDimens()[0];
        int h = hazards[i]->getDimens()[1];

        // Get correct sprite direction
        SDL_RendererFlip flip = (hazards[i]->getDirection()[0] > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        if (auto heavyHazard = dynamic_cast<HeavyHazard*>(hazards[i].get()))
        {
            if (hazards[i]->isCaught()) {
                // Rotate and offset to make it hang
                Rod rod = logicPtr->getRod();
                int rodX = rod.getPosition()[0];
                int rodY = rod.getPosition()[1];
                int offsetX = w / 4;
                int adjX = rodX - w / 2 + offsetX;
                int adjY = rodY + h / 3;
                double angle = -90.0;
                textureManager.drawRotatedTexture(Resource::HAZARD_HEAVY_ID, adjX, adjY, w, h, angle, SDL_FLIP_NONE);
            } else {
                // Cannon sprite for heavy hazard (not caught)
                textureManager.drawTexture(Resource::HAZARD_HEAVY_ID, x, y, w, h, flip);
            }
        }
        else if (dynamic_cast<ShockHazard*>(hazards[i].get()))
        {
            // Skeleton sprite for shock hazard
            textureManager.drawTexture(Resource::HAZARD_SHOCK_ID, x, y, w, h, flip);
        }
        else
        {
            // Animated barracuda for normal hazard
            std::string baseId = hazards[i]->getSpriteID();
            std::string anim1Id = hazards[i]->getAnimFrame1ID();
            std::string anim2Id = hazards[i]->getAnimFrame2ID();
            
            textureManager.drawAnimatedTexture(baseId, anim1Id, anim2Id, x, y, w, h, Game::FISH_ANIMATION_DELAY, flip);
        }
    }
}

/**
 * @brief draws fishing rod
 */
void FishingView::drawFishingRod()
{
    Rod rod = logicPtr->getRod();
    int x = rod.getPosition()[0];
    int y = rod.getPosition()[1];

    // Scale
    int rx = Scale::scaleY(Game::ROD_RADIUSX_PERCENT);
    int ry = Scale::scaleH(Game::ROD_RADIUSY_PERCENT);
    float rodTopY = Scale::scaleH(0.04f);

    thickLineRGBA(renderer, x, rodTopY, x, y - ry,2,Colors::White::R, Colors::White::G, Colors::White::B, Colors::White::A);
    ellipseRGBA(renderer, x, y, rx, ry, Colors::Black::R, Colors::Black::G, Colors::Black::B, Colors::Black::A);
}

/**
 * @brief draws hitboxes for collision debugging
 */
void FishingView::drawHitboxes() {
    if (!showHitboxes) return;

    // Rod hitbox
    Rod rod = logicPtr->getRod();
    int rodX = rod.getPosition()[0];
    int rodY = rod.getPosition()[1];
    int rodRX = Scale::scaleW(Game::ROD_RADIUSX_PERCENT) * 1.2f;
    int rodRY = Scale::scaleH(Game::ROD_RADIUSY_PERCENT) * 1.2f;
    ellipseRGBA(renderer, rodX, rodY, rodRX, rodRY, Colors::DarkBlue::R, Colors::DarkBlue::G, Colors::DarkBlue::B, 128);

    // Fish hitboxes
    std::vector<std::shared_ptr<Fish>>& allFish = logicPtr->getAllFishRef();
    for (int i = 0; i < allFish.size(); i++) {
        if (allFish[i]->isActive()) {
            int x = allFish[i]->getPosition()[0];
            int y = allFish[i]->getPosition()[1];
            int w = allFish[i]->getDimens()[0];
            int h = allFish[i]->getDimens()[1];

            int hitboxSize = h / 2;
            int hitboxX, hitboxY;

            if (allFish[i]->getDirection()[0] > 0) {
                hitboxX = x + w - hitboxSize - 5;
            } else {
                hitboxX = x + 5;
            }

            hitboxY = y + (h - hitboxSize) / 2;

            // Bounding box
            boxRGBA(renderer, x, y, x + w, y + h, Colors::DarkGreen::R, Colors::DarkGreen::G, Colors::DarkGreen::B, 128);
            // Hitbox
            boxRGBA(renderer, hitboxX, hitboxY, hitboxX + hitboxSize, hitboxY + hitboxSize, Colors::DarkGreen::R, Colors::DarkGreen::G, Colors::DarkGreen::B, 200);
        }
    }

    // Hazard hitboxes
    std::vector<std::shared_ptr<Hazard>>& hazards = logicPtr->getHazardsRef();
    for (int i = 0; i < hazards.size(); i++) {
        if (hazards[i]->isActive()) {
            int x = hazards[i]->getPosition()[0];
            int y = hazards[i]->getPosition()[1];
            int w = hazards[i]->getDimens()[0];
            int h = hazards[i]->getDimens()[1];

            boxRGBA(renderer, x, y, x + w, y + h, Colors::Red::R, Colors::Red::G, Colors::Red::B, 128);
        }
    }
}

/**
 * @brief Toggle showing hitboxes
 */
void FishingView::toggleHitboxes() {
    showHitboxes = !showHitboxes;
}

/**
 * @brief Helper method to draw a styled text box like the one in store_view
 * @param x X position of the box
 * @param y Y position of the box
 * @param width Width of the box
 * @param height Height of the box
 * @param text Text to display in the box
 */
void FishingView::drawStyledBox(int x, int y, int width, int height, const std::string& text) {
    // Box dimensions
    SDL_Rect boxRect = {x, y, width, height};
    
    // Draw background - tan color
    SDL_SetRenderDrawColor(renderer, Colors::Tan::R, Colors::Tan::G, Colors::Tan::B, Colors::Tan::A);
    SDL_RenderFillRect(renderer, &boxRect);
    
    // Draw outer border - darker brown
    SDL_SetRenderDrawColor(renderer, Colors::Brown::R, Colors::Brown::G, Colors::Brown::B, Colors::Brown::A);
    SDL_RenderDrawRect(renderer, &boxRect);
    
    // Draw inner border - lighter highlight
    SDL_Rect innerRect = {x + 2, y + 2, width - 4, height - 4};
    SDL_SetRenderDrawColor(renderer, Colors::Wheat::R, Colors::Wheat::G, Colors::Wheat::B, Colors::Wheat::A);
    SDL_RenderDrawRect(renderer, &innerRect);
    
    // Render text
    int scaledFontSize = Scale::scaleFontSize(Visuals::FONT_SIZE);
    TTF_SetFontSize(font, scaledFontSize);
    
    // Darker brown text color
    SDL_Color textColor = {Colors::DarkBrown::R, Colors::DarkBrown::G, Colors::DarkBrown::B, Colors::DarkBrown::A};
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        std::cerr << "Failed to create texture from text surface: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Center text in box
    SDL_Rect textRect = {
        x + (width - textSurface->w) / 2,
        y + (height - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    
    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}



/**
 * @brief draws background sprite with animation
 */
void FishingView::drawBackground()
{
    // Render animated background instead of static one
    textureManager.drawAnimatedTexture(
        Resource::BG_FISHING_ID, 
        Resource::BG_FISHING_ANIM1_ID, 
        Resource::BG_FISHING_ANIM2_ID, 
        0, 0, Dimens::SCREEN_W, Dimens::SCREEN_H,
        Game::BACKGROUND_ANIMATION_DELAY
    );

    float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);

    /*
    // DEBUG: draw waterline
    int lineThicc = std::max(2, (int)(4 * Scale::getHeightScaleFactor()));
    boxRGBA(renderer, screenW, surfacePond, 0, surfacePond + lineThicc,
        Colors::Gray::R, Colors::Gray::G, Colors::Gray::B, Colors::Gray::A);
    */
}

/**
 * @brief view updater, calls each of the draw methods
 */
void FishingView::drawView()
{
    SDL_RenderClear(renderer);
    drawBackground();
    // display view to user
    drawFishingRod();
    drawFish();
    drawHazard();
    drawTimer(); // Draw timer first
    drawScore(); // Then draw score directly beneath it
    drawHitboxes();
    
    // If the game is paused, render the pause menu
    if (logicPtr->isPaused()) {
        pauseMenu.render();
    }
}

/**
 * @brief handles user keyboard events to pass to logic
 * @param SDL input event
 * @return action event
 */
UserInput::Action FishingView::handleKeyboardInput(SDL_Event &e)
{
    // If the game is paused, handle pause menu input instead
    if (logicPtr->isPaused()) {
        handlePauseMenuInput(e);
        return UserInput::Action::NONE;
    }
    
    if (e.type == SDL_KEYDOWN)
    {
        // select action based on key press
        switch (e.key.keysym.sym)
        {
            case SDLK_q:
                return UserInput::Action::QUIT;
                break;
                
            case SDLK_ESCAPE:
            case SDLK_p:
                logicPtr->pauseGame();
                return UserInput::Action::NONE;
                break;
                
            case SDLK_RSHIFT:
            case SDLK_LSHIFT:
                return UserInput::Action::RELEASE;
                break;
                
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
                return UserInput::Action::START;
                break;
                
            case SDLK_h:
                toggleHitboxes();
                break;
        }
    }
    return handleContinuousInput();
}


/**
 * @brief wrapper for pause menu input handling, called by ProcessManager
 * @param SDL input event
 */
void FishingView::handlePauseMenuInput(SDL_Event &e)
{
    if (pauseMenu.handleEvent(e)) {
        // Event fully handled by the pause menu
        return;
    }
}

/**
 * @brief handles continues input for smooth movement
 * @return action event based on keyboard state
 */
UserInput::Action FishingView::handleContinuousInput() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);

    if (keyState[SDL_SCANCODE_LEFT] && keyState[SDL_SCANCODE_RIGHT] ||
        keyState[SDL_SCANCODE_UP] && keyState[SDL_SCANCODE_DOWN]) {
        return UserInput::Action::NONE;
    } else if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_DOWN]) {
        return UserInput::Action::MOVEDOWN;
    } else if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_UP]) {
        return UserInput::Action::MOVEUP;
    }

    return UserInput::Action::NONE;
}

/**
 * @brief draws timer with styled box
 */
void FishingView::drawTimer() {
    // Calculate remaining time
    int remainingTime = logicPtr->getRoundDuration() - logicPtr->getElapsedTime();
    if (remainingTime < 0) remainingTime = 0;
    int secs = remainingTime/1000;
    
    // Format time with padding for consistent display
    std::ostringstream timerStream;
    timerStream << "Time: " << std::setfill('0') << std::setw(2) << secs;
    std::string timerText = timerStream.str();
    
    // Scale and position the box at top-left
    float boxWidthPercent = 0.15f;
    float boxHeightPercent = 0.05f;
    float boxXPercent = 0.02f;
    float boxYPercent = 0.02f;
    
    int boxWidth = Scale::scaleW(boxWidthPercent);
    int boxHeight = Scale::scaleH(boxHeightPercent);
    int boxX = Scale::scaleX(boxXPercent);
    int boxY = Scale::scaleY(boxYPercent);
    
    // Draw the styled box with timer text
    drawStyledBox(boxX, boxY, boxWidth, boxHeight, timerText);
}

/** Moved down so I could compare with Timer
 * @brief draws scoreboard with styled box
 */
void FishingView::drawScore()
{
    // Get the score as string
    std::string scoreText = "Score: " + std::to_string(logicPtr->getCatchScore());
    
    // Scale and position the box
    float boxWidthPercent = 0.15f;
    float boxHeightPercent = 0.05f;
    float boxXPercent = 0.02f;
    float boxYPercent = 0.09f;
    
    int boxWidth = Scale::scaleW(boxWidthPercent);
    int boxHeight = Scale::scaleH(boxHeightPercent);
    int boxX = Scale::scaleX(boxXPercent);
    int boxY = Scale::scaleY(boxYPercent);

    // Draw the styled box with score text
    drawStyledBox(boxX, boxY, boxWidth, boxHeight, scoreText);
}

/**
 * DEPRECATED: Same as the other two, may not end up using this one
 */
void FishingView::updateResolution(float newWidth, float newHeight) {
    std::cout << "FishingView: Updating for resolution " << newWidth << "x" << newHeight << std::endl;
    // Update screen dimensions
    screenW = newWidth;
    screenH = newHeight;
    pauseMenu.setScreenDimensions(newWidth, newHeight); // Pause menu scaling    
    loadSprites(); // Reload texturesif needed
}