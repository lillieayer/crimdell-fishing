#ifndef TEXT_BOX_HPP
#define TEXT_BOX_HPP

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "game_constants.hpp"
#include "scaling.hpp"

class TextBox {
    public:
        /* THIS CRAP SHOULD FINALLY GET MOVED OUT OF HEADER */

        // Constructor takes font pointers and color
        TextBox(float xPercent, float yPercent, float widthPercent, float heightPercent, 
                const std::string& text, TTF_Font* font = nullptr, 
                TTF_Font* hoverFont = nullptr, int cost = 0)
            : xPercent(xPercent), yPercent(yPercent), 
              widthPercent(widthPercent), heightPercent(heightPercent), 
              text(text), cost(cost),
              showHoverBox(false), hovered(false), 
              hoverBoxWidth(180), hoverBoxHeight(75),
              font(font), hoverFont(hoverFont) {
            updateScreenPosition();
        }

        // Update screen position based on scaling
        void updateScreenPosition() {
            x = Scale::scaleX(xPercent);
            y = Scale::scaleY(yPercent);
            width = Scale::scaleW(widthPercent);
            height = Scale::scaleH(heightPercent);
            
            // Scale hover box size
            hoverBoxWidth = Scale::scaleW(0.15f);
            hoverBoxHeight = Scale::scaleH(0.08f);
        }

        bool intersects(int pointX, int pointY) const {
            return pointX >= x && pointX <= x + width &&
                   pointY >= y && pointY <= y + height;
        }
        
        void setHoverText(const std::string& text) {
            hoverText = text;
            showHoverBox = !text.empty();
        }
        
        // Getters/Setters
        int getX() const { return x; }
        void setX(int newX) { x = newX; }
        int getY() const { return y; }
        void setY(int newY) { y = newY; }
        int getWidth() const { return width; }
        void setWidth(int newWidth) { width = newWidth; }
        int getHeight() const { return height; }
        void setHeight(int newHeight) { height = newHeight; }
        const std::string& getText() const { return text; }
        void setText(const std::string& newText) { text = newText; }
        int getCost() const { return cost; }
        void setCost(int newCost) { cost = newCost; }
        bool isHovered() const { return hovered; }
        void setHovered(bool isHovered) { hovered = isHovered; }

        // Set fonts after construction if needed
        void setFonts(TTF_Font* newFont, TTF_Font* newHoverFont) {
            font = newFont;
            hoverFont = newHoverFont;
        }
        
        // Render method takes renderer and color
        void renderBox(SDL_Renderer* renderer, const SDL_Color& color);

    
    private:
        float xPercent;
        float yPercent;
        float widthPercent;
        float heightPercent;
        
        int x;
        int y;
        int width;
        int height;
        
        std::string text;
        int cost;

        bool showHoverBox;
        bool hovered;
        std::string hoverText;
        int hoverBoxWidth;
        int hoverBoxHeight;

        TTF_Font* font;
        TTF_Font* hoverFont;
        
        // Helper methods for writing text
        void writeText(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color);
        void writeHoverText(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color);
        void writeTextBlended(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color);
        void writeHoverTextBlended(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color);
};

#endif