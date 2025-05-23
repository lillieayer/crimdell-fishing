#include <stdexcept>
#include <string>
#include <iostream>
#include "menu_view.hpp"
#include "game_constants.hpp"
#include "text_box.hpp"

MenuView::MenuView(const shared_ptr<GameNotifier>& notifier, SDL_Window* window, SDL_Renderer* renderer) :
    // Constructor
    BaseView(notifier, window, renderer),
    done(false),

    storeButton(0.35f, 0.45f, 0.3f, 0.05f, "New Game"),
    loadGameButton(0.35f, 0.55f, 0.3f, 0.05f, "Load Game"),
    exitButton(0.4f, 0.65f, 0.2f, 0.05f, "Exit Game"),
    // Resolution buttons
    smallResButton(0.05f, 0.05f, 0.1f, 0.04f, "Small"),
    medResButton(0.05f, 0.1f, 0.1f, 0.04f, "Medium"),
    largeResButton(0.05f, 0.15f, 0.1f, 0.04f, "Large") {

        // Note: color is set in BaseView's constructor
        storeButton.setHoverText("Start a new game and go to the store");
        loadGameButton.setHoverText("Load a previously saved game");  // New hover text
        exitButton.setHoverText("Exit the game");
        storeButton.setFonts(font, hoverFont);
        loadGameButton.setFonts(font, hoverFont);
        exitButton.setFonts(font, hoverFont);

        // Hover text/fonts for resolution buttons
        smallResButton.setHoverText("Change to small resolution (" + std::to_string(static_cast<int>(Dimens::SCREEN_W_SMALL)) + "x" + std::to_string(static_cast<int>(Dimens::SCREEN_H_SMALL)) + ")");
        medResButton.setHoverText("Change to medium resolution (" + std::to_string(static_cast<int>(Dimens::SCREEN_W_MED)) + "x" + std::to_string(static_cast<int>(Dimens::SCREEN_H_MED)) + ")");
        largeResButton.setHoverText("Change to large resolution (" + std::to_string(static_cast<int>(Dimens::SCREEN_W_LARGE)) + "x" + std::to_string(static_cast<int>(Dimens::SCREEN_H_LARGE)) + ")");
        smallResButton.setFonts(font, hoverFont);
        medResButton.setFonts(font, hoverFont);
        largeResButton.setFonts(font, hoverFont);

        // Load static background
        if (!textureManager.loadTexture(Resource::BG_MENU_ID, Resource::BG_MENU)) {
            std::cerr << "Failed to load menu background" << std::endl;
        }
        
        // Load animated background frames
        if (!textureManager.loadTexture(Resource::BG_MENU_ANIM1_ID, Resource::BG_MENU_ANIM1)) {
            std::cerr << "Failed to load menu background animation frame 1" << std::endl;
        }
        
        if (!textureManager.loadTexture(Resource::BG_MENU_ANIM2_ID, Resource::BG_MENU_ANIM2)) {
            std::cerr << "Failed to load menu background animation frame 2" << std::endl;
        }
    }

MenuView::~MenuView() {
    // Destructor
}

void MenuView::input(SDL_Event &e) {
    // Debug output
    std::cout << "MenuView received event type: " << e.type << std::endl;
    
    switch (e.type) {
        case SDL_QUIT:
            std::cout << "MenuView: Quit event received" << std::endl;
            notifier->exitGame();
            break;
            
        case SDL_WINDOWEVENT:
            std::cout << "MenuView: Window event received: " << e.window.event << std::endl;
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                focusedWindow = false;
            } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                focusedWindow = true;
            }
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            std::cout << "MenuView: Mouse button down at (" << e.button.x << ", " << e.button.y << ")" << std::endl;
            if (e.button.button == SDL_BUTTON_LEFT) {
                mouseClick(e.button.x, e.button.y);
            }
            break;
            
        case SDL_MOUSEMOTION:
            {   
                // Update button hover states
                bool storeHovered = storeButton.intersects(e.motion.x, e.motion.y);
                bool loadGameHovered = loadGameButton.intersects(e.motion.x, e.motion.y);  // New hover check
                bool exitHovered = exitButton.intersects(e.motion.x, e.motion.y);
                
                // Only log changes in hover state                
                if (storeHovered != storeButton.isHovered()) {
                    std::cout << "MenuView: Store button hover changed to " << storeHovered << std::endl;
                    storeButton.setHovered(storeHovered);
                }
                
                if (loadGameHovered != loadGameButton.isHovered()) {  // New hover handler
                    std::cout << "MenuView: Load Game button hover changed to " << loadGameHovered << std::endl;
                    loadGameButton.setHovered(loadGameHovered);
                }
                
                if (exitHovered != exitButton.isHovered()) {
                    std::cout << "MenuView: Exit button hover changed to " << exitHovered << std::endl;
                    exitButton.setHovered(exitHovered);
                }

                // Res buttons
                bool smallResHovered = smallResButton.intersects(e.motion.x, e.motion.y);
                bool medResHovered = medResButton.intersects(e.motion.x, e.motion.y);
                bool largeResHovered = largeResButton.intersects(e.motion.x, e.motion.y);
                
                if (smallResHovered != smallResButton.isHovered()) {
                    smallResButton.setHovered(smallResHovered);
                }
                if (medResHovered != medResButton.isHovered()) {
                    medResButton.setHovered(medResHovered);
                }
                if (largeResHovered != largeResButton.isHovered()) {
                    largeResButton.setHovered(largeResHovered);
                }
            }
            break;

        case SDL_KEYDOWN:
            std::cout << "MenuView: Key pressed: " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
            // Add keyboard navigation for menu
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    std::cout << "MenuView: Exit key pressed" << std::endl;
                    notifier->exitGame();
                    break;
                    
                case SDLK_RETURN:
                case SDLK_SPACE:
                    // Select currently hovered button
                    if (storeButton.isHovered()) {
                        std::cout << "MenuView: Selecting store button via keyboard" << std::endl;
                        notifier->startStore();
                    } else if (loadGameButton.isHovered()) {  // New keyboard handler
                        std::cout << "MenuView: Selecting load game button via keyboard" << std::endl;
                        notifier->loadGame();
                    } else if (exitButton.isHovered()) {
                        std::cout << "MenuView: Selecting exit button via keyboard" << std::endl;
                        notifier->exitGame();
                    }
                    break;
                    
                case SDLK_UP:
                case SDLK_w:
                    // Navigate up in menu
                    if (exitButton.isHovered()) {
                        exitButton.setHovered(false);
                        loadGameButton.setHovered(true);
                    } else if (loadGameButton.isHovered()) {
                        loadGameButton.setHovered(false);
                        storeButton.setHovered(true);
                    } else if (storeButton.isHovered()) {
                        storeButton.setHovered(false);
                        exitButton.setHovered(true);  // Wrap around to bottom
                    }
                    break;                    
                case SDLK_f:
                    std::cout << "MenuView: Direct fishing shortcut" << std::endl;
                    notifier->goFishing();
                    break;
                    
                case SDLK_l:
                    std::cout << "MenuView: Direct load game shortcut" << std::endl;
                    notifier->loadGame();
                    break;
            }
            break;
    }
}

void MenuView::mouseClick(float x, float y) {
    std::cout << "MenuView::mouseClick at (" << x << ", " << y << ")" << std::endl;
    
    if (storeButton.intersects(x, y)) {
        std::cout << "MenuView: Store button clicked, calling notifier->startStore()" << std::endl;
        notifier->startStore();
    } else if (loadGameButton.intersects(x, y)) {  // New click handler
        std::cout << "MenuView: Load Game button clicked, calling notifier->loadGame()" << std::endl;
        notifier->loadGame();
    } else if (exitButton.intersects(x, y)) {
        std::cout << "MenuView: Exit button clicked, calling notifier->exitGame()" << std::endl;
        notifier->exitGame();
    } else if (smallResButton.intersects(x, y)) {
        std::cout << "MenuView: Small resolution button clicked" << std::endl;
        handleResolutionChange(Dimens::SCREEN_W_SMALL, Dimens::SCREEN_H_SMALL);
    } else if (medResButton.intersects(x, y)) {
        std::cout << "MenuView: Medium resolution button clicked" << std::endl;
        handleResolutionChange(Dimens::SCREEN_W_MED, Dimens::SCREEN_H_MED);
    } else if (largeResButton.intersects(x, y)) {
        std::cout << "MenuView: Large resolution button clicked" << std::endl;
        handleResolutionChange(Dimens::SCREEN_W_LARGE, Dimens::SCREEN_H_LARGE);
    } else {
        std::cout << "MenuView: Click outside any button" << std::endl;
    }
}

void MenuView::renderMenu() {    
    // Render buttons
    storeButton.renderBox(renderer, color);
    loadGameButton.renderBox(renderer, color);
    exitButton.renderBox(renderer, color);

    smallResButton.renderBox(renderer, color);
    medResButton.renderBox(renderer, color);
    largeResButton.renderBox(renderer, color);
}

void MenuView::render() {
    SDL_SetRenderDrawColor(renderer, 0, 40, 100, 255);
    SDL_RenderClear(renderer);

    // Use animated background instead of static
    textureManager.drawAnimatedTexture(
        Resource::BG_MENU_ID,
        Resource::BG_MENU_ANIM1_ID,
        Resource::BG_MENU_ANIM2_ID,
        0, 0, Dimens::SCREEN_W, Dimens::SCREEN_H,
        Game::BACKGROUND_ANIMATION_DELAY
    );
    
    renderMenu();
}


/**
 * @brief Notify processmanager to perform resolution change 
 */
void MenuView::handleResolutionChange(int width, int height) {
    notifier->changeResolution(width, height);
}

/**
 * DEPRECATED: I think
 * @brief Reloads UI elements with new scaling
 */
void MenuView::updateResolution() {
    std::cout << "MenuView: Updating for resolution " << Dimens::SCREEN_W << "x" << Dimens::SCREEN_H << std::endl;
    for (auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();
    
    // Update textbox positions
    storeButton.updateScreenPosition();
    loadGameButton.updateScreenPosition();
    exitButton.updateScreenPosition();
    smallResButton.updateScreenPosition();
    medResButton.updateScreenPosition();
    largeResButton.updateScreenPosition();
    
    // Update hover text for resolution buttons (otherwise issue)
    smallResButton.setHoverText("Change to small resolution (" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_W_SMALL)) + "x" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_H_SMALL)) + ")");
    
    medResButton.setHoverText("Change to medium resolution (" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_W_MED)) + "x" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_H_MED)) + ")");
    
    largeResButton.setHoverText("Change to large resolution (" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_W_LARGE)) + "x" + 
        std::to_string(static_cast<int>(Dimens::SCREEN_H_LARGE)) + ")");
}