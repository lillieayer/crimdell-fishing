#ifndef STORE_VIEW_HPP
#define STORE_VIEW_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "text_box.hpp"
#include "player_data.hpp"
#include "base_view.hpp"
#include <stdexcept>
#include <iostream>
#include "game_notifier.hpp"
#include "game_constants.hpp"
#include <sstream>
#include <iomanip>
#include "scaling.hpp"
using namespace std;

class StoreView : public BaseView {
public:
    StoreView(const shared_ptr<GameNotifier>& notifier, SDL_Window* window, SDL_Renderer* renderer, PlayerData& playerData);
    ~StoreView();

    // Core loop methods
    void input(SDL_Event &e) override;
    void render() override;
    
    void updateResolution() override;

private:
    PlayerData& playerData;

    // Upgrade costs, should be moved to game constants
    int upgrade1_cost = Game::ROD_PRICE;
    int upgrade2_cost = Game::CLOCK_PRICE;
    int upgrade3_cost = Game::REGISTER_PRICE;
    int upgrade4_cost = Game::LEDGER_PRICE;
    int loan_payment_amount = Game::LOAN_PAYMENT;

    // Upgrade buttons
    TextBox upgrade1;
    TextBox upgrade2;
    TextBox upgrade3;
    TextBox upgrade4;
    TextBox payLoanButton;
    TextBox fishingButton;
    TextBox saveGameButton;
    TextBox exitButton;
    TextBox resetTutorial;

    void renderStore();
    void drawPlayerStats(); // draw player stats in a box
    void renderBox(const TextBox& box);
    void mouseClick(float x, float y);

    // String formatting
    std::string getRodHoverText() const;
    std::string getClockHoverText() const;
    std::string getRegisterHoverText() const;
    std::string getLedgerHoverText() const;
    std::string getPayLoanHoverText() const;
};

#endif