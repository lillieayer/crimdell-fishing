#include "base_view.hpp"
#include "game_constants.hpp"

#include "game_notifier.hpp"

BaseView::BaseView(const shared_ptr<GameNotifier> &notifier, SDL_Window *window, SDL_Renderer *renderer) :
    notifier(notifier), window(window),
    renderer(renderer),
    focusedWindow(true),  // Ensure this is true initially
    font(nullptr),
    hoverFont(nullptr),  // Explicitly initialize this
    textureManager(renderer)
{
    color = {70, 35, 0, 255}; // Dark brown color
    
    // Initialize fonts with better error reporting
    try {
        initializeFonts();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing fonts: " << e.what() << std::endl;
        // Don't rethrow - attempt to continue
    }
}

BaseView::~BaseView()
{
    for (auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();
    cleanupFonts();
}

void BaseView::initializeFonts()
{
    std::cout << "Loading fonts from: " << Resource::WATERLILY << " and " << Resource::ARIAL << std::endl;
    
    // Scale font height
    int scaledFontSize = Scale::scaleFontSize(Visuals::FONT_SIZE);
    int scaledHoverFontSize = Scale::scaleFontSize(Visuals::HOVER_FONT_SIZE);

    // Load scaled font
    font = TTF_OpenFont(Resource::WATERLILY.c_str(), scaledFontSize);
    if (!font) {
        std::cerr << "Failed to load main font Waterlily: " << TTF_GetError() << std::endl;
        
        // Try to load Arial as fallback
        font = TTF_OpenFont(Resource::ARIAL.c_str(), scaledFontSize);
        
        if (!font) {
            std::cerr << "Failed to load backup Arial font: " << TTF_GetError() << std::endl;
        }
    }
    
    // Try to load hover font
    hoverFont = TTF_OpenFont(Resource::ARIAL.c_str(), scaledHoverFontSize);
    if (!hoverFont) {
        std::cerr << "Failed to load hover font: " << TTF_GetError() << std::endl;
        }
    
    // Set font hinting for better quality
    if (font) {
        TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
    }
    
    if (hoverFont) {
        TTF_SetFontHinting(hoverFont, TTF_HINTING_LIGHT);
    }
}

void BaseView::cleanupFonts()
{
    // Main destructor implementation, destroys SDL objects in reverse order of initialization
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (hoverFont) {
        TTF_CloseFont(hoverFont);
        hoverFont = nullptr;
    }
}

void BaseView::write(const std::string &text, int x, int y)
{
    // Handle the case where font loading failed
    if (!font) {
        std::cerr << "Cannot render text '" << text << "': Font not loaded" << std::endl;
        return;
    }
    
    if (x < 0 || y < 0 || x >= Dimens::SCREEN_W || y >= Dimens::SCREEN_H) {
        std::cerr << "Text out of bounds: " << text << " at (" << x << ", " << y << ")" << std::endl;
        return;
    }

    try {
        SDL_Texture* texture = getCachedTexture(text);
        if (!texture) {
            std::cerr << "Failed to get cached texture for: " << text << std::endl;
            return;
        }

        int text_width, text_height;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height) < 0) {
            std::cerr << "Failed to query texture dimensions: " << SDL_GetError() << std::endl;
            return;
        }

        SDL_Rect location = {x, y, text_width, text_height};
        if (SDL_RenderCopy(renderer, texture, nullptr, &location) < 0) {
            std::cerr << "Failed to render texture: " << SDL_GetError() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error rendering text '" << text << "': " << e.what() << std::endl;
    }
}

SDL_Texture *BaseView::getCachedTexture(const std::string &text)
{
    if (text.empty()) {
        throw std::invalid_argument("Attempted to render empty text");
        return nullptr;
    }

    if (textureCache.size() > 100) {
        for (auto& pair : textureCache) {
            SDL_DestroyTexture(pair.second);
        }
        textureCache.clear();
    }

    auto it = textureCache.find(text);
    if (it != textureCache.end()) {
        return it->second;
    }

    // Use Blended mode instead of Solid for better quality antialiased text
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        throw std::runtime_error("Failed to create text surface." + std::string(SDL_GetError()));
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        throw std::runtime_error("Failed to create texture from text surface." + std::string(SDL_GetError()));
        return nullptr;
    }

    textureCache[text] = texture;
    return texture;
}

/**
 * DEPRECATED: Update the renderer on res change
 */
void BaseView::updateRenderer(SDL_Renderer* newRenderer) {
    renderer = newRenderer;
    textureManager.updateRenderer(newRenderer);
    for (auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();
}