#include "pause_menu.hpp"
#include "game_constants.hpp"
#include <iostream>

PauseMenu::PauseMenu(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* hoverFont, int screenWidth, int screenHeight)
    : renderer(renderer), 
      font(font), 
      hoverFont(hoverFont),
      screenWidth(screenWidth), 
      screenHeight(screenHeight),
      resumeButton(0.42f, 0.42f, 0.16f, 0.06f, "Resume"),
      quitButton(0.42f, 0.51f, 0.16f, 0.06f, "Quit"),
      fishingLogic(nullptr) // Initialize fishingLogic to nullptr
{
    resumeButton.setHoverText("Continue playing the current fishing round");
    quitButton.setHoverText("Return to the main menu (progress will be lost)");
    
    // Only set fonts if not null
    if (font != nullptr && hoverFont != nullptr) {
        resumeButton.setFonts(font, hoverFont);
        quitButton.setFonts(font, hoverFont);
    }
    
    // Update screen positions based on current dimens
    resumeButton.updateScreenPosition();
    quitButton.updateScreenPosition();
}

void PauseMenu::render() {
    // Check if valid
    if (!renderer) {
        std::cerr << "PauseMenu::render - Renderer is null!" << std::endl;
        return;
    }
    if (!font) {
        std::cerr << "PauseMenu::render - Font is null!" << std::endl;
        return;
    }
    
    // Draw semi-transparent overlay
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect fullScreen = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &fullScreen);
    
    // Draw pause menu title
    float titleWidthPercent = 0.24f;
    float titleHeightPercent = 0.09f;
    float titleXPercent = 0.38f;
    float titleYPercent = 0.31f;
    int titleWidth = Scale::scaleW(titleWidthPercent);
    int titleHeight = Scale::scaleH(titleHeightPercent);
    int titleX = Scale::scaleX(titleXPercent);
    int titleY = Scale::scaleY(titleYPercent);  
    TextBox titleBox(titleXPercent, titleYPercent, titleWidthPercent, titleHeightPercent, "Game Paused");
    titleBox.setFonts(font, hoverFont);
    SDL_Color textColor = {70, 35, 0, 255}; // Color for text

    // Render title and buttons
    titleBox.renderBox(renderer, textColor);
    resumeButton.renderBox(renderer, textColor);
    quitButton.renderBox(renderer, textColor);
}

bool PauseMenu::handleEvent(SDL_Event& e) {
    // Check if fishingLogic is valid
    if (!fishingLogic) {
        std::cerr << "PauseMenu: No fishing logic reference set" << std::endl;
        return false;
    }
    
    switch (e.type) {
        case SDL_MOUSEMOTION:
            resumeButton.setHovered(resumeButton.intersects(e.motion.x, e.motion.y));
            quitButton.setHovered(quitButton.intersects(e.motion.x, e.motion.y));
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                mouseClick(e.button.x, e.button.y);
                return true;
            }
            break;
            
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_p:
                case SDLK_RETURN:
                    // Resume game on Escape, P, or Enter key
                    fishingLogic->resumeGame();
                    return true;
                    
                case SDLK_q:
                    // Quit to menu on Q key
                    fishingLogic->quitToMenu();
                    return true;
                    
                case SDLK_UP:
                case SDLK_DOWN:
                    // Toggle between buttons with up/down keys
                    if (resumeButton.isHovered()) {
                        resumeButton.setHovered(false);
                        quitButton.setHovered(true);
                    } else {
                        resumeButton.setHovered(true);
                        quitButton.setHovered(false);
                    }
                    break;
                    
                default:
                    break;
            }
            break;
    }
    
    return false;
}

void PauseMenu::mouseClick(int x, int y) {
    // Check if fishingLogic is valid before using it
    if (!fishingLogic) {
        std::cerr << "PauseMenu::mouseClick - fishingLogic is null!" << std::endl;
        return;
    }
    
    if (resumeButton.intersects(x, y)) {
        std::cout << "PauseMenu: Resume game clicked" << std::endl;
        fishingLogic->resumeGame();
    } else if (quitButton.intersects(x, y)) {
        std::cout << "PauseMenu: Quit to menu clicked" << std::endl;
        fishingLogic->quitToMenu();
    }
}

void PauseMenu::setFonts(TTF_Font* newFont, TTF_Font* newHoverFont) {
    // Check if the new fonts are valid before setting them
    if (!newFont || !newHoverFont) {
        std::cerr << "PauseMenu::setFonts - One or both fonts are null!" << std::endl;
        return;
    }
    
    font = newFont;
    hoverFont = newHoverFont;

    resumeButton.setFonts(newFont, newHoverFont);
    quitButton.setFonts(newFont, newHoverFont);
}

/**
 * @brief Update local screen size vars and button positions when resolution changes
 */
void PauseMenu::setScreenDimensions(int newWidth, int newHeight) {
    // Check if the dimensions are valid
    if (newWidth <= 0 || newHeight <= 0) {
        std::cerr << "PauseMenu::setScreenDimensions - Invalid dimensions: " 
                  << newWidth << "x" << newHeight << std::endl;
        return;
    }
    
    screenWidth = newWidth;
    screenHeight = newHeight;
    
    // Update button positions
    resumeButton.updateScreenPosition();
    quitButton.updateScreenPosition();
}