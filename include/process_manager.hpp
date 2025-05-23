#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <memory>
#include "game_notifier.hpp"
#include "menu_view.hpp"
#include "store_view.hpp"
#include "fishing_logic.hpp"
#include "fishing_view.hpp"
#include "game_constants.hpp"
#include "view_adapter.hpp"
#include "player_data.hpp"
#include "popup.hpp"
#include <iostream>
#include <fstream> // For victory mode ez flag
#include <SDL.h>
#include <SDL_ttf.h>
using namespace std;

// Enumerator for gamestate control
enum class GameState {
    MENU,
    STORE,
    PAUSED_STORE,
    FISHING,
    PAUSED_FISHING,
    GAMEOVER
};

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();
    void run();

private:
    // Moved window creation to here
    SDL_Window* window;
    SDL_Renderer* renderer;
    void initializeSDL();

    // Game state management
    GameState currentState;
    bool isRunning;

    // Game modules
    const std::shared_ptr<GameNotifier> gameNotifier = make_shared<GameNotifier>();
    std::unique_ptr<MenuView> menuView;

    std::shared_ptr<FishingLogic> fishingLogic;
    std::unique_ptr<FishingView> fishingView;

    std::unique_ptr<StoreView> storeView;

    PlayerData playerData;

    // Game loop helpers
    void handleStateTransition(GameState newState);
    void handleNotification(const std::string& notification);
    void updateCurrentModule();
    void renderCurrentModule();
    int calculateFishingLevel();

    // Player data management
    void resetPlayerData();

    // Save/Load game functionality
    bool saveGame();
    bool loadGame();
    void showSaveGamePopup(bool success);
    void showLoadGamePopup(bool success);
    const std::string SAVE_FILE_PATH = "fishing_game_save.txt";

    // Move to game constants
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;
    Uint32 frameStart;
    int frameTime;

    std::string lastNote; // previous notification

    UserInput::Action userInput;

    void logSDLError(const std::string& message);

    void changeResolution(int width, int height);
    void recreateRenderer();

    // Popup related
    std::unique_ptr<Popup> popup;
    bool fishPopup;
    bool loanPaidPopup; // Victory screen
    bool transitionWait;
    bool transitionDone;
    GameState pendingState;
    void showFishPopup();
    void showLoanPaidPopup();
    bool createFishingPhaseWithNoHazards();
    void showGameOverPopup();
    bool isGameOver = false;
    void showStoreTutorial1();
    void showStoreTutorial2();
    void showFishingTutorial1();
    void showFishingTutorial2();
    int currentTutorialStep = 0;
    void showSecretEndingPopup();

};

#endif