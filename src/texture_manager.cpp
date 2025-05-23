#include "texture_manager.hpp"

TextureManager::TextureManager(SDL_Renderer* renderer) : renderer(renderer) {
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
    }
}

TextureManager::~TextureManager() {
    clearTextures();
    IMG_Quit();
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filePath) {
    SDL_Surface* tempSurface = IMG_Load(filePath.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load image " << filePath << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!texture) {
        std::cerr << "Failed to create texture from " << filePath << "! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    textureMap[id] = texture;
    return true;
}

void TextureManager::drawTexture(const std::string& id, int x, int y, int scaledW, int scaledH, SDL_RendererFlip flip) {
    auto it = textureMap.find(id);
    if (it == textureMap.end()) {
        std::cerr << "Texture with ID " << id << " not found!" << std::endl;
        return;
    }

    int spriteH, spriteW;
    SDL_QueryTexture(it->second, nullptr, nullptr, &spriteW, &spriteH);
    
    SDL_Rect srcRect = {0, 0, spriteW, spriteH}; // Origin image
    SDL_Rect destRect = {x, y, scaledW, scaledH}; // Scaled image

    SDL_RenderCopyEx(renderer, it->second, &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawRotatedTexture(const std::string& id, int x, int y, int scaledW, int scaledH, 
    double angle, SDL_RendererFlip flip) {
    auto it = textureMap.find(id);
    if (it == textureMap.end()) {
        std::cerr << "Texture with ID " << id << " not found!" << std::endl;
        return;
    }

    int spriteH, spriteW;
    SDL_QueryTexture(it->second, nullptr, nullptr, &spriteW, &spriteH);

    SDL_Rect srcRect = {0, 0, spriteW, spriteH}; // Origin image
    SDL_Rect destRect = {x, y, scaledW, scaledH}; // Scaled image

    // Calculate center point for rotation
    SDL_Point center = {scaledW / 2, scaledH / 2};

    SDL_RenderCopyEx(renderer, it->second, &srcRect, &destRect, angle, &center, flip);
}

std::string TextureManager::getCurrentAnimationFrame(const std::string& baseId, 
    const std::string& anim1Id, const std::string& anim2Id, int delay) {
    
    // Check if this is a caught fish (use static texture) - this case will be handled in the fishing_view.cpp
    if (baseId.find("fish") != std::string::npos) {
    }
    
    // Get current time
    Uint32 currentTime = SDL_GetTicks();
    
    // Initialize animation state if not yet tracked
    if (lastAnimationTime.find(baseId) == lastAnimationTime.end()) {
        lastAnimationTime[baseId] = currentTime;
        animationFrameState[baseId] = true; // Start with first frame
    }
    
    // Check if it's time to change animation frame
    if (currentTime - lastAnimationTime[baseId] > delay) {
        // Toggle animation state
        animationFrameState[baseId] = !animationFrameState[baseId];
        // Update last animation time
        lastAnimationTime[baseId] = currentTime;
    }
    
    // Return the appropriate frame ID based on current state
    return animationFrameState[baseId] ? anim1Id : anim2Id;
}

void TextureManager::drawAnimatedTexture(const std::string& baseId, const std::string& anim1Id, 
    const std::string& anim2Id, int x, int y, int scaledW, int scaledH, 
    int delay, SDL_RendererFlip flip) {
    
    // Get current animation frame ID
    std::string currentFrameId = getCurrentAnimationFrame(baseId, anim1Id, anim2Id, delay);
    
    // Draw the current animation frame
    drawTexture(currentFrameId, x, y, scaledW, scaledH, flip);
}

void TextureManager::drawAnimatedRotatedTexture(const std::string& baseId, const std::string& anim1Id, 
    const std::string& anim2Id, int x, int y, int scaledW, int scaledH, 
    double angle, int delay, SDL_RendererFlip flip) {
    
    // Get current animation frame ID
    std::string currentFrameId = getCurrentAnimationFrame(baseId, anim1Id, anim2Id, delay);
    
    // Draw the current animation frame with rotation
    drawRotatedTexture(currentFrameId, x, y, scaledW, scaledH, angle, flip);
}

void TextureManager::clearTextures() {
    for (auto& pair : textureMap) {
        SDL_DestroyTexture(pair.second);
    }
    textureMap.clear();
    
    // Clear animation tracking maps too
    animationFrameState.clear();
    lastAnimationTime.clear();
    animationDelays.clear();
}

// Used in BaseView but may be dead code
void TextureManager::updateRenderer(SDL_Renderer* newRenderer) {
    clearTextures();
    renderer = newRenderer;
}