#include "text_box.hpp"
#include <stdexcept>

void TextBox::renderBox(SDL_Renderer* renderer, const SDL_Color& color) {
    if (!font || !hoverFont) {
        throw std::runtime_error("TextBox fonts not set before rendering");
    }

    // Scale screen position
    updateScreenPosition();

    SDL_Rect rect = {x, y, width, height};

    // Background color - tan
    if (hovered) {
        // Lighter tan for hover
        SDL_SetRenderDrawColor(renderer, Colors::TanHover::R, Colors::TanHover::G, Colors::TanHover::B, Colors::TanHover::A);
    } else {
        // Base tan color
        SDL_SetRenderDrawColor(renderer, Colors::Tan::R, Colors::Tan::G, Colors::Tan::B, Colors::Tan::A);
    }

    // Fill rectangle with the background color
    SDL_RenderFillRect(renderer, &rect);

    // Outer border - darker brown
    SDL_SetRenderDrawColor(renderer, Colors::Brown::R, Colors::Brown::G, Colors::Brown::B, Colors::Brown::A);
    SDL_RenderDrawRect(renderer, &rect);

    // Inner border - lighter highlight
    SDL_Rect innerRect = {x + 2, y + 2, width - 4, height - 4};
    SDL_SetRenderDrawColor(renderer, Colors::Wheat::R, Colors::Wheat::G, Colors::Wheat::B, Colors::Wheat::A);
    SDL_RenderDrawRect(renderer, &innerRect);

    // Brown text
    SDL_Color textColor = {70, 35, 0, 255};

    // Get text dimensions for proper centering
    int textWidth, textHeight;
    TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);
    
    // Calculate centered position
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - textHeight) / 2;
    
    // Render with high quality text
    writeTextBlended(renderer, text, textX, textY, textColor);

    if (hovered && showHoverBox) {
        int hoverX = x + width + 10;
        int hoverY = y;

        // Background for hover box - semi-transparent tan
        SDL_Rect hoverRect = {hoverX, hoverY, hoverBoxWidth, hoverBoxHeight};
        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 220); // Tan with transparency
        SDL_RenderFillRect(renderer, &hoverRect);
        
        // Hover box border - darker brown
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Medium brown
        SDL_RenderDrawRect(renderer, &hoverRect);
        
        // Render hover text with smaller font and scale
        int lineHeight = Visuals::HOVER_FONT_SIZE + 2;
        lineHeight = int(lineHeight * Scale::getHeightScaleFactor());
        
        // Calculate max chars per line based on hover box width
        int maxCharsPerLine = hoverBoxWidth / (TTF_FontHeight(hoverFont) / 2);
        std::string textRemaining = hoverText;
        int currentLine = 0;
        
        while (!textRemaining.empty() && currentLine < 3) {
            std::string lineText;
            
            if (textRemaining.length() <= maxCharsPerLine) {
                lineText = textRemaining;
                textRemaining = "";
            } else {
                // Find space to break the line
                int breakPos = maxCharsPerLine;
                while (breakPos > 0 && textRemaining[breakPos] != ' ') {
                    breakPos--;
                }
                
                if (breakPos == 0) {
                    // No space found, just break at max chars
                    breakPos = maxCharsPerLine;
                }
                
                lineText = textRemaining.substr(0, breakPos);
                textRemaining = textRemaining.substr(breakPos + 1);
            }
            
            // Brown hover text
            SDL_Color hoverTextColor = {70, 35, 0, 255};
            writeHoverTextBlended(renderer, lineText, hoverX + 5, hoverY + 5 + (currentLine * lineHeight), hoverTextColor);
            currentLine++;
        }
    }
}

// High-quality text rendering
void TextBox::writeTextBlended(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color) {
    if (x < 0 || y < 0 || x >= Dimens::SCREEN_W || y >= Dimens::SCREEN_H) {
        throw std::out_of_range("Text out of bounds");
        return;
    }

    // Create surface with font using blended mode for high quality
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        throw std::runtime_error("Failed to create text surface.");
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        throw std::runtime_error("Failed to create texture from text surface.");
        return;
    }

    int text_width, text_height;
    SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);
    SDL_Rect location = {x, y, text_width, text_height};
    SDL_RenderCopy(renderer, texture, nullptr, &location);
    SDL_DestroyTexture(texture);
}

// Keep old method for compatibility
void TextBox::writeText(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color) {
    writeTextBlended(renderer, text, x, y, color);
}

// New high-quality hover text rendering
void TextBox::writeHoverTextBlended(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color) {
    if (x < 0 || y < 0 || x >= Dimens::SCREEN_W || y >= Dimens::SCREEN_H) {
        throw std::out_of_range("Text out of bounds");
        return;
    }

    // Create surface with hover font using blended mode for high quality
    SDL_Surface* surface = TTF_RenderText_Blended(hoverFont, text.c_str(), color);
    if (!surface) {
        throw std::runtime_error("Failed to create hover text surface.");
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        throw std::runtime_error("Failed to create texture from hover text surface.");
        return;
    }

    int text_width, text_height;
    SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);
    SDL_Rect location = {x, y, text_width, text_height};
    SDL_RenderCopy(renderer, texture, nullptr, &location);
    SDL_DestroyTexture(texture);
}

// Keep old method just in case
void TextBox::writeHoverText(SDL_Renderer* renderer, const std::string& text, int x, int y, const SDL_Color& color) {
    writeHoverTextBlended(renderer, text, x, y, color);
}