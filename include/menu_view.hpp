#ifndef MENU_VIEW_HPP
#define MENU_VIEW_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "game_notifier.hpp"
#include "text_box.hpp"
#include "base_view.hpp"
#include <iostream>
#include "scaling.hpp"
using namespace std;

class MenuView : public BaseView{
public:
    MenuView(const shared_ptr<GameNotifier>& notifier, SDL_Window* window, SDL_Renderer* renderer);
    ~MenuView();

    // Core loop methods
    bool isDone() const {return done;}
    void input(SDL_Event &e) override;
    void render() override;

    void updateResolution() override;

private:
    bool done;

//    TextBox fishingButton;
    TextBox storeButton;
    TextBox loadGameButton;
    TextBox exitButton;

    // Resolution change buttons
    TextBox smallResButton;
    TextBox medResButton;
    TextBox largeResButton;
    void handleResolutionChange(int width, int height);

    void renderMenu();
    void mouseClick(float x, float y);
};

#endif