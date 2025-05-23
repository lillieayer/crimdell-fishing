#ifndef BASE_VIEW_HPP
#define BASE_VIEW_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>
#include "game_constants.hpp"
#include "texture_manager.hpp"
#include "scaling.hpp"
using namespace std;
#include "game_notifier.hpp"

/**
 * @class BaseView 
 * @brief Abstract base class for all views in the game to allow dif views to inherit
 * the same commonmethods (initialize, cleanup,  etc)
 */

class BaseView{

    protected: 
        bool focusedWindow;
        SDL_Window* window;
        const shared_ptr<GameNotifier> notifier;
        SDL_Renderer* renderer;
        TTF_Font* font;
        TTF_Font* hoverFont;
        SDL_Color color;
        std::unordered_map<std::string, SDL_Texture*> textureCache; 
        TextureManager textureManager;
    
        void initializeFonts();
        void cleanupFonts();
        void write(const std::string& text, int x, int y);
        SDL_Texture* getCachedTexture(const std::string& text);

    public:
        BaseView(const shared_ptr<GameNotifier>& notifier, SDL_Window* window, SDL_Renderer* renderer);
        virtual ~BaseView();
        virtual void render() = 0;
        virtual void input(SDL_Event &e) = 0;
        virtual void updateResolution() {}
        void updateRenderer(SDL_Renderer* newRenderer);
};

#endif //BASE_VIEW_HPP