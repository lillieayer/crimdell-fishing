#include "popup.hpp"
#include "game_constants.hpp"
#include "scaling.hpp"

Popup::Popup(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight)
    : renderer(renderer), font(font), screenWidth(screenWidth), screenHeight(screenHeight),
      visible(false), closeButtonHovered(false) {
    
    float popupWidthPercent = 0.35f;
    float popupHeightPercent = 0.45f;
    int popupWidth = Scale::scaleW(popupWidthPercent);
    int popupHeight = Scale::scaleH(popupHeightPercent);
    
    popupRect = {
        (screenWidth - popupWidth) / 2,
        (screenHeight - popupHeight) / 2,
        popupWidth, 
        popupHeight
    };
}

void Popup::show(const std::string& title, const std::vector<std::string>& lines) {
    this->title = title;
    this->messageLines = lines;
    visible = true;
}

void Popup::render() {
    if (!visible) return;
    
    // Semi-transparent dark overlay behind popup
    SDL_SetRenderDrawColor(renderer, Colors::Black::R, Colors::Black::G, Colors::Black::B, Colors::Black::ATrans);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect fullScreen = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &fullScreen);
    
    SDL_SetRenderDrawColor(renderer, Colors::Tan::R, Colors::Tan::G, Colors::Tan::B, Colors::Tan::A); // Background
    SDL_RenderFillRect(renderer, &popupRect);
    SDL_SetRenderDrawColor(renderer, Colors::Brown::R, Colors::Brown::G, Colors::Brown::B, Colors::Brown::A); // Outer border
    SDL_RenderDrawRect(renderer, &popupRect);
    SDL_Rect innerRect = {popupRect.x + 2, popupRect.y + 2, popupRect.w - 4, popupRect.h - 4}; // Inner border
    SDL_SetRenderDrawColor(renderer, Colors::Wheat::R, Colors::Wheat::G, Colors::Wheat::B, Colors::Wheat::A);
    SDL_RenderDrawRect(renderer, &innerRect);
    SDL_Color textColor = {Colors::DarkBrown::R, Colors::DarkBrown::G, Colors::DarkBrown::B, Colors::DarkBrown::A}; // Text color
    
    // Scale font sizes for title and message
    int scaledTitleFontSize = Scale::scaleFontSize(Visuals::POPUP_TITLE_FONT_SIZE);
    int scaledTextFontSize = Scale::scaleFontSize(Visuals::POPUP_TEXT_FONT_SIZE);
    TTF_SetFontSize(font, scaledTitleFontSize);
    
    // Render title centered at top of popup
    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, title.c_str(), textColor);
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        if (titleTexture) {
            SDL_Rect titleRect = {
                popupRect.x + (popupRect.w - titleSurface->w) / 2,
                popupRect.y + static_cast<int>(popupRect.h * 0.06f),
                titleSurface->w,
                titleSurface->h
            };
            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }
    
    // Set font size for message lines and render
    TTF_SetFontSize(font, scaledTextFontSize);
    float contentStartPercent = 0.2f;
    float lineSpacingPercent = 0.08f;
    
    int lineY = popupRect.y + static_cast<int>(popupRect.h * contentStartPercent);
    int lineSpacing = static_cast<int>(popupRect.h * lineSpacingPercent);
    
    for (const auto& line : messageLines) {
        SDL_Surface* lineSurface = TTF_RenderText_Blended(font, line.c_str(), textColor);
        if (lineSurface) {
            SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
            if (lineTexture) {
                SDL_Rect lineRect = {
                    popupRect.x + static_cast<int>(popupRect.w * 0.07f),
                    lineY,
                    lineSurface->w,
                    lineSurface->h
                };
                SDL_RenderCopy(renderer, lineTexture, NULL, &lineRect);
                SDL_DestroyTexture(lineTexture);
            }
            SDL_FreeSurface(lineSurface);
        }
        lineY += lineSpacing;
    }
}

bool Popup::handleEvent(SDL_Event& e) {
    if (!visible) {
        return false;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;
        
        // Clicking anywhere outside the popup closes it
        if (x < popupRect.x || x > popupRect.x + popupRect.w ||
            y < popupRect.y || y > popupRect.y + popupRect.h) {
            visible = false;
            return true;
        }
    }
    else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE || 
                                       e.key.keysym.sym == SDLK_SPACE || 
                                       e.key.keysym.sym == SDLK_RETURN)) {
        visible = false;
        return true;
    }
    return false;
}

/**
 * DEPRECATED: Same as the one (removed from) text box
 */
void Popup::updateScreenDimensions(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
    
    float popupWidthPercent = 0.35f;
    float popupHeightPercent = 0.45f;
    
    int popupWidth = static_cast<int>(newWidth * popupWidthPercent);
    int popupHeight = static_cast<int>(newHeight * popupHeightPercent);
    
    popupRect = {
        (screenWidth - popupWidth) / 2,
        (screenHeight - popupHeight) / 2,
        popupWidth, 
        popupHeight
    };
}

