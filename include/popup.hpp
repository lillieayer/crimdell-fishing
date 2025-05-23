#ifndef POPUP_HPP
#define POPUP_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Popup {
public:
    Popup(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight);
    ~Popup() = default;
    
    // Show popup with title/message
    void show(const std::string& title, const std::vector<std::string>& lines);
    
    // Handle events for closing the popup
    bool handleEvent(SDL_Event& e);

    void render();
    bool isVisible() const { return visible; }
    void updateScreenDimensions(int newWidth, int newHeight);
    void updateRenderer(SDL_Renderer* newRenderer) { renderer = newRenderer; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    bool visible;
    
    SDL_Rect popupRect;
    std::string title;
    std::vector<std::string> messageLines;
    
    // Kept for compatibility with existing code
    bool closeButtonHovered;
};

#endif