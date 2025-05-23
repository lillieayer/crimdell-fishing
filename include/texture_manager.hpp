#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include <iostream>
#include <unordered_map>

class TextureManager {
    private:
        std::map<std::string, SDL_Texture*> textureMap;
        SDL_Renderer* renderer;

        // Animation tracking
        std::unordered_map<std::string, bool> animationFrameState; // true = frame1, false = frame2
        std::unordered_map<std::string, Uint32> lastAnimationTime;
        std::unordered_map<std::string, int> animationDelays;

    public:
        TextureManager(SDL_Renderer* renderer);
        ~TextureManager();

        bool loadTexture(const std::string& id, const std::string& filePath);
        void drawTexture(const std::string& id, int x, int y, int scaledW, int scaledH, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void drawRotatedTexture(const std::string& id, int x, int y, int scaledW, int scaledH, 
            double angle, SDL_RendererFlip flip = SDL_FLIP_NONE);
        
        // Animation methods
        void drawAnimatedTexture(const std::string& baseId, const std::string& anim1Id, 
            const std::string& anim2Id, int x, int y, int scaledW, int scaledH, 
            int delay, SDL_RendererFlip flip = SDL_FLIP_NONE);
        
        void drawAnimatedRotatedTexture(const std::string& baseId, const std::string& anim1Id, 
            const std::string& anim2Id, int x, int y, int scaledW, int scaledH, 
            double angle, int delay, SDL_RendererFlip flip = SDL_FLIP_NONE);
        
        // Get current animation frame ID based on time and delay
        std::string getCurrentAnimationFrame(const std::string& baseId, 
            const std::string& anim1Id, const std::string& anim2Id, int delay);
        
        void clearTextures();
        void updateRenderer(SDL_Renderer* newRenderer);
};

#endif