#include "store_view.hpp"

StoreView::StoreView(const shared_ptr<GameNotifier>& notifier, SDL_Window* window, 
                    SDL_Renderer* renderer, PlayerData& playerData) :
    
    // Constructor
    BaseView(notifier, window, renderer),
    playerData(playerData),
    exitButton(0.02f, 0.03f, 0.12f, 0.05f, "Main Menu"),
    upgrade1(0.73f, 0.43f, 0.1f, 0.05f, "Rod"),
    upgrade2(0.54f, 0.105f, 0.1f, 0.05f, "Clock"),
    upgrade3(0.76f, 0.62f, 0.1f, 0.05f, "Register"),
    upgrade4(0.56f, 0.7f, 0.1f, 0.05f, "Ledger"),
    payLoanButton(0.165f, 0.83f, 0.12f, 0.05f, "Pay Loan"),
    fishingButton(0.165f, 0.883f, 0.12f, 0.05f, "Go Fishing"),
    saveGameButton(0.165f, 0.938f, 0.12f, 0.05f, "Save Game"),
    resetTutorial(0.02f, 0.09f, 0.12f, 0.05f, "Tutorial")
    {
        color = {Colors::Black::R, Colors::Black::G, Colors::Black::B, Colors::Black::A};

        // Set font sizes
        int buttonFontSize = Scale::scaleFontSize(Visuals::STORE_BUTTON_FONT_SIZE);
        int hoverFontSize = Scale::scaleFontSize(Visuals::HOVER_FONT_SIZE);
        if (font) {
            TTF_SetFontSize(font, buttonFontSize);
        }
        if (hoverFont) {
            TTF_SetFontSize(hoverFont, hoverFontSize);
        }
        
        // Scale button positions/set hover text
        updateResolution();
        fishingButton.setHoverText("Start day " + std::to_string(playerData.getDayCount() + 1));
        saveGameButton.setHoverText("Save your current progress");
        exitButton.setHoverText("Return to the main menu (Esc)");
        resetTutorial.setHoverText("View the tutorial again");
        resetTutorial.setFonts(font, hoverFont);

        // Set fonts
        upgrade1.setFonts(font, hoverFont);
        upgrade2.setFonts(font, hoverFont);
        upgrade3.setFonts(font, hoverFont);
        upgrade4.setFonts(font, hoverFont);
        payLoanButton.setFonts(font, hoverFont);
        fishingButton.setFonts(font, hoverFont);
        saveGameButton.setFonts(font, hoverFont);
        exitButton.setFonts(font, hoverFont);

        // Load static background DEPRECATED
        if (!textureManager.loadTexture(Resource::BG_STORE_ID, Resource::BG_STORE)) {
            std::cerr << "Failed to load store background" << std::endl;
        }
        
        // Load animated background frames
        if (!textureManager.loadTexture(Resource::BG_STORE_ANIM1_ID, Resource::BG_STORE_ANIM1)) {
            std::cerr << "Failed to load store background animation frame 1" << std::endl;
        }
        
        if (!textureManager.loadTexture(Resource::BG_STORE_ANIM2_ID, Resource::BG_STORE_ANIM2)) {
            std::cerr << "Failed to load store background animation frame 2" << std::endl;
        }
    }

StoreView::~StoreView() {
    // Destructor
}

void StoreView::input(SDL_Event &e) {
    switch (e.type) {
        case SDL_QUIT:
            notifier->exitGame();
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                focusedWindow = false;
            } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                focusedWindow = true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                mouseClick(e.button.x, e.button.y);
            }
            break;
        case SDL_KEYDOWN:
            if ((e.key.keysym.sym == SDLK_ESCAPE) || (e.key.keysym.sym == SDLK_q)) {
                notifier->returnMenu();
            } else if (e.key.keysym.sym == SDLK_s) {
                // Shortcut for save game
                notifier->saveGame();
            } else if (e.key.keysym.sym == SDLK_p) {
                // Shortcut for pay loan
                if (playerData.getCash() >= loan_payment_amount && playerData.getLoanAmount() > 0) {
                    playerData.payLoan(loan_payment_amount);
                    payLoanButton.setHoverText(getPayLoanHoverText());
                    
                    // Check if loan is fully paid
                    if (playerData.isLoanPaid()) {
                        // Notify process manager to show congratulations popup
                        notifier->notifyLoanPaid();
                    }
                }
            }
            break;
        case SDL_MOUSEMOTION:
            upgrade1.setHovered(upgrade1.intersects(e.motion.x, e.motion.y));
            upgrade2.setHovered(upgrade2.intersects(e.motion.x, e.motion.y));
            upgrade3.setHovered(upgrade3.intersects(e.motion.x, e.motion.y));
            upgrade4.setHovered(upgrade4.intersects(e.motion.x, e.motion.y));
            payLoanButton.setHovered(payLoanButton.intersects(e.motion.x, e.motion.y));
            fishingButton.setHovered(fishingButton.intersects(e.motion.x, e.motion.y));
            saveGameButton.setHovered(saveGameButton.intersects(e.motion.x, e.motion.y));
            exitButton.setHovered(exitButton.intersects(e.motion.x, e.motion.y));
            resetTutorial.setHovered(resetTutorial.intersects(e.motion.x, e.motion.y));
            break;
    }
}

void StoreView::mouseClick(float x, float y) {
    if (upgrade1.intersects(x, y)) {
        if (playerData.getCash() >= upgrade1_cost) {
            playerData.spendCash(upgrade1_cost);
            playerData.upgradeRod();
            upgrade1.setHoverText(getRodHoverText());
        }
    } else if (upgrade2.intersects(x, y)) {
        if (playerData.getCash() >= upgrade2_cost) {
            playerData.spendCash(upgrade2_cost);
            playerData.upgradeClock();
            upgrade2.setHoverText(getClockHoverText());
        }
    } else if (upgrade3.intersects(x, y)) {
        if (playerData.getCash() >= upgrade3_cost) {
            playerData.spendCash(upgrade3_cost);
            playerData.upgradeRegister();
            upgrade3.setHoverText(getRegisterHoverText());
        }
    } else if (upgrade4.intersects(x, y)) {
        if (playerData.getCash() >= upgrade4_cost) {
            playerData.spendCash(upgrade4_cost);
            playerData.upgradeLedger();
            upgrade4.setHoverText(getLedgerHoverText());
        }
    } else if (payLoanButton.intersects(x, y)) {
        if (playerData.getCash() >= loan_payment_amount && playerData.getLoanAmount() > 0) {
            playerData.payLoan(loan_payment_amount);
            payLoanButton.setHoverText(getPayLoanHoverText());
            
            // Check if loan is fully paid
            if (playerData.isLoanPaid()) {
                // Notify process manager to show congratulations popup
                notifier->notifyLoanPaid();
            }
        }
    } else if (fishingButton.intersects(x, y)) {
        notifier->goFishing();
    } else if (saveGameButton.intersects(x, y)) {
        notifier->saveGame();
    } else if (exitButton.intersects(x, y)) {
        notifier->returnMenu(); 
    } else if (resetTutorial.intersects(x, y)) {
        playerData.resetTutorialSeen();
        notifier->showTutorial();
    }
}

void StoreView::renderStore() {        
    // Draw upgrade buttons
    upgrade1.renderBox(renderer, color);
    upgrade2.renderBox(renderer, color);
    upgrade3.renderBox(renderer, color);
    upgrade4.renderBox(renderer, color);
    
    // Draw pay loan button (disabled if loan is paid off or insufficient cash)
    if (playerData.getLoanAmount() <= 0) {
        // Gray out the button if loan is paid off
        SDL_Color grayColor = {150, 150, 150, 255};
        payLoanButton.renderBox(renderer, grayColor);
    } else if (playerData.getCash() < loan_payment_amount) {
        // Gray out the button if insufficient cash
        SDL_Color grayColor = {150, 150, 150, 255};
        payLoanButton.renderBox(renderer, grayColor);
    } else {
        payLoanButton.renderBox(renderer, color);
    }
    
    fishingButton.renderBox(renderer, color);
    saveGameButton.renderBox(renderer, color);
    exitButton.renderBox(renderer, color);
    resetTutorial.renderBox(renderer, color);

    // Draw player statistics in bottom left
    drawPlayerStats();
}

void StoreView::drawPlayerStats() {
    // Position stats box
    float boxWidthPercent = 0.15f;
    float boxHeightPercent = 0.16f;
    float boxXPercent = 0.01f;
    float boxYPercent = 0.83f;
    
    int width = Scale::scaleW(boxWidthPercent);
    int height = Scale::scaleH(boxHeightPercent);
    int baseX = Scale::scaleX(boxXPercent);
    int baseY = Scale::scaleY(boxYPercent);
    
    // Draw background for stats (tan colored box with border)
    SDL_Rect statsRect = {baseX, baseY, width, height};
    
    SDL_SetRenderDrawColor(renderer, Colors::Tan::R, Colors::Tan::G, Colors::Tan::B, Colors::Tan::ATrans); // Background
    SDL_RenderFillRect(renderer, &statsRect);
    SDL_SetRenderDrawColor(renderer, Colors::Brown::R, Colors::Brown::G, Colors::Brown::B, Colors::Brown::A); // Outer border 
    SDL_RenderDrawRect(renderer, &statsRect);
    SDL_Rect innerRect = {baseX + 2, baseY + 2, width - 4, height - 4}; // Inner border
    SDL_SetRenderDrawColor(renderer, Colors::Wheat::R, Colors::Wheat::G, Colors::Wheat::B, Colors::Wheat::A);
    SDL_RenderDrawRect(renderer, &innerRect);
    
    // Use scaled font sizes for headers and text
    int headerFontSize = Scale::scaleFontSize(Visuals::STATS_HEADER_FONT_SIZE);
    int textFontSize = Scale::scaleFontSize(Visuals::STATS_TEXT_FONT_SIZE);
    TTF_SetFontSize(font, headerFontSize);
    int textXOffset = static_cast<int>(width * 0.05f);
    int textX = baseX + textXOffset;
    SDL_Color textColor = {Colors::Black::R, Colors::Black::G, Colors::Black::B, Colors::Black::A};
    
    // Render player stats header
    SDL_Surface* headerSurface = TTF_RenderText_Blended(font, "Player Stats:", textColor);
    SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
    SDL_Rect headerRect = {textX, baseY, headerSurface->w, headerSurface->h};
    SDL_RenderCopy(renderer, headerTexture, NULL, &headerRect);
    SDL_FreeSurface(headerSurface);
    SDL_DestroyTexture(headerTexture);
    
    // Change font size for regular text
    TTF_SetFontSize(font, textFontSize);
    
    // Draw each line with even spacing
    float lineSpacingPercent = 0.12f;
    int lineSpacing = static_cast<int>(height * lineSpacingPercent);
    int yOffset = static_cast<int>(height * 0.20f);
    
    // Day count
    std::string dayText = "Day: " + std::to_string(playerData.getDayCount());
    SDL_Surface* daySurface = TTF_RenderText_Blended(font, dayText.c_str(), textColor);
    SDL_Texture* dayTexture = SDL_CreateTextureFromSurface(renderer, daySurface);
    SDL_Rect dayRect = {textX, baseY + yOffset, daySurface->w, daySurface->h};
    SDL_RenderCopy(renderer, dayTexture, NULL, &dayRect);
    SDL_FreeSurface(daySurface);
    SDL_DestroyTexture(dayTexture);
    yOffset += lineSpacing;
        
    // Cash
    std::string cashText = "Cash: $" + std::to_string(playerData.getCash());
    SDL_Surface* cashSurface = TTF_RenderText_Blended(font, cashText.c_str(), textColor);
    SDL_Texture* cashTexture = SDL_CreateTextureFromSurface(renderer, cashSurface);
    SDL_Rect cashRect = {textX, baseY + yOffset, cashSurface->w, cashSurface->h};
    SDL_RenderCopy(renderer, cashTexture, NULL, &cashRect);
    SDL_FreeSurface(cashSurface);
    SDL_DestroyTexture(cashTexture);
    yOffset += lineSpacing;
    
    // Loan amount
    SDL_Color loanColor = textColor;
    if (playerData.getLoanAmount() <= 0) {
        loanColor = {0, 128, 0, 255}; // Green for paid off
    } else {
        loanColor = {180, 0, 0, 255}; // Red for not
    }
    
    std::string loanText = "Loan: $" + std::to_string(playerData.getLoanAmount());
    if (playerData.getLoanAmount() <= 0) {
        loanText = "Loan: PAID OFF!";
    }
    
    SDL_Surface* loanSurface = TTF_RenderText_Blended(font, loanText.c_str(), loanColor);
    SDL_Texture* loanTexture = SDL_CreateTextureFromSurface(renderer, loanSurface);
    SDL_Rect loanRect = {textX, baseY + yOffset, loanSurface->w, loanSurface->h};
    SDL_RenderCopy(renderer, loanTexture, NULL, &loanRect);
    SDL_FreeSurface(loanSurface);
    SDL_DestroyTexture(loanTexture);
    yOffset += lineSpacing;
    
    // Ledger income (if any)
    if (playerData.getLedgerIncome() > 0) {
        std::string ledgerText = "Ledger Income: $" + std::to_string(playerData.getLedgerIncome()) + "/day";
        SDL_Surface* ledgerSurface = TTF_RenderText_Blended(font, ledgerText.c_str(), textColor);
        SDL_Texture* ledgerTexture = SDL_CreateTextureFromSurface(renderer, ledgerSurface);
        SDL_Rect ledgerRect = {textX, baseY + yOffset, ledgerSurface->w, ledgerSurface->h};
        SDL_RenderCopy(renderer, ledgerTexture, NULL, &ledgerRect);
        SDL_FreeSurface(ledgerSurface);
        SDL_DestroyTexture(ledgerTexture);
        yOffset += lineSpacing;
    }
    
    // Cash multiplier (if any)
    if (playerData.getRegisterLevel() > 0) {
        std::ostringstream multiplierStream;
        multiplierStream << std::fixed << std::setprecision(2) << playerData.getCashMultiplier();
        
        std::string multiplierText = "Cash Multiplier: x" + multiplierStream.str();
        SDL_Surface* multiplierSurface = TTF_RenderText_Blended(font, multiplierText.c_str(), textColor);
        SDL_Texture* multiplierTexture = SDL_CreateTextureFromSurface(renderer, multiplierSurface);
        SDL_Rect multiplierRect = {textX, baseY + yOffset, multiplierSurface->w, multiplierSurface->h};
        SDL_RenderCopy(renderer, multiplierTexture, NULL, &multiplierRect);
        SDL_FreeSurface(multiplierSurface);
        SDL_DestroyTexture(multiplierTexture);
        yOffset += lineSpacing;
    }

    // Total fish caught
    std::string fishText = "Total Fish Caught: " + std::to_string(playerData.getTotalFishCaught());
    SDL_Surface* fishSurface = TTF_RenderText_Blended(font, fishText.c_str(), textColor);
    SDL_Texture* fishTexture = SDL_CreateTextureFromSurface(renderer, fishSurface);
    SDL_Rect fishRect = {textX, baseY + yOffset, fishSurface->w, fishSurface->h};
    SDL_RenderCopy(renderer, fishTexture, NULL, &fishRect);
    SDL_FreeSurface(fishSurface);
    SDL_DestroyTexture(fishTexture);

    // Reset font size back to default
    int defaultFontSize = Scale::scaleFontSize(Visuals::FONT_SIZE);
    TTF_SetFontSize(font, defaultFontSize);
}

void StoreView::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Use animated background instead of static
    textureManager.drawAnimatedTexture(
        Resource::BG_STORE_ID, 
        Resource::BG_STORE_ANIM1_ID, 
        Resource::BG_STORE_ANIM2_ID, 
        0, 0, Dimens::SCREEN_W, Dimens::SCREEN_H,
        Game::BACKGROUND_ANIMATION_DELAY
    );
    
    renderStore();
}

std::string StoreView::getRodHoverText() const {
    std::ostringstream oss;
    oss << "Rod Level: " << playerData.getRodLevel()
        << " | Max Fish: " << playerData.getMaxFishLevel()
        << " | Speed: +" << static_cast<int>(playerData.getRodSpeedMultiplier() * 100 - 100) << "%"
        << " | Cost: $" << upgrade1_cost;
    return oss.str();
}

std::string StoreView::getClockHoverText() const {
    std::ostringstream oss;
    oss << "Clock Level: " << playerData.getClockLevel()
        << " | Cost: $" << upgrade2_cost;
    return oss.str();
}

std::string StoreView::getRegisterHoverText() const {
    std::ostringstream oss;
    oss << "Register Level: " << playerData.getRegisterLevel()
        << " | Multiplier: x" << std::setprecision(2) << std::fixed << playerData.getCashMultiplier()
        << " | Cost: $" << upgrade3_cost;
    return oss.str();
}

std::string StoreView::getLedgerHoverText() const {
    std::ostringstream oss;
    oss << "Ledger Level: " << playerData.getLedgerLevel()
        << " | Income: $" << playerData.getLedgerIncome() << "/day"
        << " | Cost: $" << upgrade4_cost;
    return oss.str();
}

std::string StoreView::getPayLoanHoverText() const {
    std::ostringstream oss;
    if (playerData.getLoanAmount() <= 0) {
        oss << "Your loan is already paid off!";
    } else if (playerData.getCash() < loan_payment_amount) {
        oss << "You need $" << loan_payment_amount << " to make a payment";
    } else {
        oss << "Make a $" << loan_payment_amount << " payment on your loan"
            << " | Remaining: $" << playerData.getLoanAmount();
    }
    return oss.str();
}

/**
 * DEPRECATED: Used in constructor, but pretty sure it's dead code
 */
void StoreView::updateResolution() {
    std::cout << "StoreView: Updating for resolution " << Dimens::SCREEN_W << "x" << Dimens::SCREEN_H << std::endl;
    
    // Scale positions
    exitButton.updateScreenPosition();
    upgrade1.updateScreenPosition();
    upgrade2.updateScreenPosition();
    upgrade3.updateScreenPosition();
    upgrade4.updateScreenPosition();
    fishingButton.updateScreenPosition();
    saveGameButton.updateScreenPosition();
    payLoanButton.updateScreenPosition();
    
    // Scale text if necessary
    upgrade1.setHoverText(getRodHoverText());
    upgrade2.setHoverText(getClockHoverText());
    upgrade3.setHoverText(getRegisterHoverText());
    upgrade4.setHoverText(getLedgerHoverText());
    payLoanButton.setHoverText(getPayLoanHoverText());
}