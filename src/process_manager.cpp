#if defined(_WIN32) || defined(_WIN64)
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
    #include <unistd.h>     // For fork, execl, getcwd, _exit, dup2, etc
    #include <sys/wait.h>   // For waitpid
    #include <fcntl.h>      // For open and O_RDWR    
#endif

#include "process_manager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <errno.h>      // For errno and strerror
#include <string.h>     // For strerror
#include <limits.h>     // For PATH_MAX

#ifdef __APPLE__
#include <mach-o/dyld.h> // For _NSGetExecutablePath
#endif

ProcessManager::ProcessManager() :
    currentState(GameState::MENU),
    isRunning(true),
    transitionDone(false),
    fishPopup(false),          // Explicitly initialize to false
    loanPaidPopup(false),      // Initialize loan paid popup flag
    transitionWait(false),     // Explicitly initialize to false
    pendingState(GameState::MENU)  // Initialize with a default value
{
    std::cout << "ProcessManager: Initializing..." << std::endl;
    initializeSDL();
    // Init Menu module
    try {
        menuView = std::make_unique<MenuView>(gameNotifier, window, renderer);
        std::cout << "ProcessManager: MenuView created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to create MenuView: " << e.what() << std::endl;
    }

    // Notifier handling with error checking
    std::cout << "ProcessManager: Setting up callbacks..." << std::endl;
    try {
        gameNotifier->setCallback([this](const std::string& notification) {
            std::cout << "ProcessManager: Received notification from gameNotifier: " << notification << std::endl;
            this->handleNotification(notification);
        });
        std::cout << "ProcessManager: gameNotifier callback set successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to set gameNotifier callback: " << e.what() << std::endl;
    }
    /* Removed since no more StoreLogic
    try {
        storeLogic.setCallback([this](const std::string& notification) {
            std::cout << "ProcessManager: Received notification from StoreLogic: " << notification << std::endl;
            this->handleNotification(notification);
        });
        std::cout << "ProcessManager: StoreLogic callback set successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to set StoreLogic callback: " << e.what() << std::endl;
    }
    */

    // Init popup stuff
    try {
        TTF_Font* font = TTF_OpenFont("../resource/Arial.ttf", 16); // These font opens are redundant w/ baseview, but I'm too scared to remove them at this point
        if (!font) {
            std::cerr << "WARNING: Failed to load popup font: " << TTF_GetError() << std::endl;
            // Try fallback fonts
            font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 16);
            if (!font) {
                font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 16);
            }
            if (!font) {
                std::cerr << "ERROR: Failed to load any fonts for popup" << std::endl;
            }
        }
        popup = std::make_unique<Popup>(renderer, font, Dimens::SCREEN_W, Dimens::SCREEN_H);
        std::cout << "ProcessManager: Popup created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to create popup: " << e.what() << std::endl;
    }

    // Initialize other modules as null pointers
    fishingLogic = nullptr;
    fishingView = nullptr;
    std::cout << "ProcessManager: Initialization complete" << std::endl;

    // Double-check that popup flags are properly initialized
    std::cout << "ProcessManager: Popup state check - fishPopup: " << (fishPopup ? "true" : "false")
              << ", transitionWait: " << (transitionWait ? "true" : "false") << std::endl;
}

ProcessManager::~ProcessManager() {
    // Pure aesthetics - flush output and print message to look professional.
    std::cout << std::endl << "Process complete. Press enter to return to terminal prompt." << std::endl;
    std::cout.flush();
    std::cerr.flush();
    fflush(stdout);
    fflush(stderr);

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief Initializes SDL with acceleration if possible
 * @details Includes self-defeating debugging attempt from a while back, kept in case a similar error shows up again 
 */
void ProcessManager::initializeSDL() {
    std::cout << "ProcessManager: Initializing SDL..." << std::endl;

    // Initialize SDL with all required subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        throw std::runtime_error("SDL could not initialize! Error: " + std::string(SDL_GetError()));
    }

    std::cout << "ProcessManager: SDL initialized successfully" << std::endl;

    // Set SDL hints for better cross-platform behavior
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");  // Linear texture filtering
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "0");  // Disable screensaver

    // Try different SDL render drivers if specified
#ifdef _WIN32
    // On Windows, try Direct3D first, then OpenGL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
#else
    // On other platforms, explicitly use OpenGL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif

    std::cout << "ProcessManager: Creating window: " + std::to_string(Dimens::SCREEN_W) + "x" + std::to_string(Dimens::SCREEN_H) << std::endl;

    // Create window
    window = SDL_CreateWindow("Crim Dell Fishing",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             Dimens::SCREEN_W,
                             Dimens::SCREEN_H,
                             SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        SDL_Quit();
        throw std::runtime_error("Window could not be created! Error: " + std::string(SDL_GetError()));
    }

    std::cout << "ProcessManager: Window created successfully" << std::endl;
    std::cout << "ProcessManager: Creating renderer..." << std::endl;

    // Try with and without vsync depending on platform
#ifdef _WIN32
    // On Windows, try without vsync first
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#else
    // On other platforms, try with vsync first
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#endif

    // If that failed, try the alternative
    if (renderer == nullptr) {
        std::cerr << "First renderer creation attempt failed: " << SDL_GetError() << std::endl;

#ifdef _WIN32
        // On Windows, fall back to vsync
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#else
        // On other platforms, fall back to no vsync
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#endif

        // If still failed, try software renderer as last resort
        if (renderer == nullptr) {
            std::cerr << "Second renderer creation attempt failed: " << SDL_GetError() << std::endl;
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        }
    }

    // If all renderer creation attempts failed, throw error
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Renderer could not be created! Error: " + std::string(SDL_GetError()));
    }

    std::cout << "ProcessManager: Renderer created successfully" << std::endl;

    // Set blend mode for transparency support
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Initialize SDL_image for texture loading
    std::cout << "ProcessManager: Initializing SDL_image..." << std::endl;
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("SDL_image could not initialize! Error: " + std::string(IMG_GetError()));
    }

    std::cout << "ProcessManager: SDL_image initialized successfully" << std::endl;
    std::cout << "ProcessManager: Initializing SDL_ttf..." << std::endl;

    // Initialize TTF
    if (TTF_Init() < 0) {
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("TTF could not initialize! Error: " + std::string(TTF_GetError()));
    }

    std::cout << "ProcessManager: SDL_ttf initialized successfully" << std::endl;
    std::cout << "ProcessManager: SDL initialization complete" << std::endl;

    // Clear any non-fatal SDL errors that might have occurred during initialization
    const char* sdlError = SDL_GetError();
    if (sdlError && sdlError[0] != '\0') {
        std::cerr << "WARNING: Non-fatal SDL errors during initialization: " << sdlError << std::endl;
        SDL_ClearError();
    }
}

void ProcessManager::handleStateTransition(GameState newState) {
    std::cout << "ProcessManager::handleStateTransition from " << static_cast<int>(currentState)
              << " to " << static_cast<int>(newState) << std::endl;

    // Debug popup state
    std::cout << "ProcessManager: Popup state before transition - fishPopup: " << (fishPopup ? "true" : "false")
              << ", transitionWait: " << (transitionWait ? "true" : "false") << std::endl;

    // Safety check: if transitioning from menu, force popups to be inactive
    // This ensures we can always leave the menu state
    if (currentState == GameState::MENU) {
        if (fishPopup) {
            std::cout << "ProcessManager: WARNING - Popup incorrectly active in menu state, forcing to inactive" << std::endl;
            fishPopup = false;
        }
        if (transitionWait) {
            std::cout << "ProcessManager: WARNING - TransitionWait incorrectly active in menu state, forcing to inactive" << std::endl;
            transitionWait = false;
        }
    }

    // Handle special transitions for pause
    if (newState == GameState::PAUSED_FISHING && currentState == GameState::FISHING) {
        std::cout << "ProcessManager: Pausing fishing game" << std::endl;
        if (fishingLogic) {
            fishingLogic->setPaused(true);
        }
        currentState = GameState::PAUSED_FISHING;
        return;
    } else if (newState == GameState::FISHING && currentState == GameState::PAUSED_FISHING) {
        std::cout << "ProcessManager: Resuming fishing game" << std::endl;
        if (fishingLogic) {
            fishingLogic->setPaused(false);
        }
        currentState = GameState::FISHING;
        return;
    }

    // Now check popup state again
    if (fishPopup || loanPaidPopup) {
        std::cout << "ProcessManager: Popup active, queueing transition to " << static_cast<int>(newState) << std::endl;
        transitionWait = true;
        pendingState = newState;
        return;
    }

    // Proceed with normal state transition
    if (newState == currentState) {
        std::cout << "ProcessManager: Already in state " << static_cast<int>(currentState) << ", no transition needed" << std::endl;
        return; // no transition needed
    }

    // Clean up previous state
    std::cout << "ProcessManager: Cleaning up state " << static_cast<int>(currentState) << std::endl;

    // When leaving fishing or paused fishing, clean up fishing resources
    if (currentState == GameState::FISHING || currentState == GameState::PAUSED_FISHING) {
        std::cout << "ProcessManager: Cleaning fishing" << std::endl;
        if (fishingLogic) {
            fishingLogic->setRunning(false);
        }
        fishingView = nullptr;
        fishingLogic = nullptr;
        
        // Remove the loan paid flag file if it exists
        std::remove("victory_flag.txt");
    } else if (currentState == GameState::STORE) {
        std::cout << "ProcessManager: Cleaning store" << std::endl;
        storeView = nullptr;
    } else if (currentState == GameState::MENU) {
        std::cout << "ProcessManager: Cleaning menu (keeping menuView)" << std::endl;
        // We keep menuView alive throughout the game
    }

    // Initialize new state
    std::cout << "ProcessManager: Initializing state " << static_cast<int>(newState) << std::endl;

    try {
        if (newState == GameState::FISHING) {
            std::cout << "ProcessManager: Creating new fishing state" << std::endl;
            int level = calculateFishingLevel();
            int roundDuration = Game::ROUND_DURATION + (playerData.getClockLevel() * 10 * 1000);
            fishingLogic = std::make_shared<FishingLogic>(level, roundDuration, playerData.getRodLevel());
            
            // Set callback for fishing logic to handle notifications
            fishingLogic->setCallback([this](const std::string& notification) {
                this->handleNotification(notification);
            });
            
            fishingView = std::make_unique<FishingView>(fishingLogic, window, renderer, Dimens::SCREEN_H, Dimens::SCREEN_W);
            fishingLogic->setRunning(true);
            fishingLogic->spawnNewFloaters();
            fishingLogic->resetRoundStartTime();
            transitionDone = false;
            std::cout << "ProcessManager: Fishing state created successfully" << std::endl;
        } else if (newState == GameState::STORE) {
            std::cout << "ProcessManager: Creating new store state" << std::endl;
            storeView = std::make_unique<StoreView>(gameNotifier, window, renderer, playerData);
            std::cout << "ProcessManager: Store state created successfully" << std::endl;
        } else if (newState == GameState::MENU) {
            std::cout << "ProcessManager: Returning to menu state" << std::endl;
            // MenuView should already exist
            if (!menuView) {
                std::cout << "ProcessManager: Recreating MenuView" << std::endl;
                menuView = std::make_unique<MenuView>(gameNotifier, window, renderer);
            }
        }

        currentState = newState;
        std::cout << "ProcessManager: State transition complete. Current state: " << static_cast<int>(currentState) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR during state transition: " << e.what() << std::endl;
        logSDLError("State transition error");

        // Fallback to menu if transition fails
        if (newState != GameState::MENU) {
            std::cout << "ProcessManager: Falling back to menu state after transition failure" << std::endl;
            currentState = GameState::MENU;
        }
    }
}

void ProcessManager::run() {
    // Main game loop
    frameStart = SDL_GetTicks();

    // Clear any pending events at startup
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // Discard any events that might be in the queue
    }

    std::cout << "Starting main game loop" << std::endl;

    while (isRunning) {
        // Process all events first
        updateCurrentModule();

        // Render current frame
        renderCurrentModule();

        // Calculate and apply frame delay
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }

        frameStart = SDL_GetTicks();
    }
}

void ProcessManager::updateCurrentModule() {
    SDL_Event e;
    userInput = UserInput::Action::NONE;
    bool quitToMenu = false;

    // Debug output to verify event processing
    bool eventProcessed = false;

    while (SDL_PollEvent(&e) != 0) {
        eventProcessed = true;

        // Debug output to see what events are being processed
        if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION) {
            //std::cout << "Event processed: Type=" << e.type;
            if (e.type == SDL_KEYDOWN) {
                std::cout << ", Key=" << SDL_GetKeyName(e.key.keysym.sym);
            }
            std::cout << std::endl;
        }

        // Global event handling - always process quit events first
        if (e.type == SDL_QUIT) {
            std::cout << "Quit event received" << std::endl;
            isRunning = false;
            return;
        }

        // Handle popup events
        if ((fishPopup || loanPaidPopup) && popup->handleEvent(e)) {
            std::cout << "Popup closed" << std::endl;
            
            if (fishPopup) {
                fishPopup = false;

                // Handle tutorial progression
                if (!playerData.hasSeenTutorial()) {
                    if (currentTutorialStep == 1) {
                        showStoreTutorial2();
                        return;
                    } else if (currentTutorialStep == 2) {
                        return;
                    } else if (currentTutorialStep == 3) {
                        showFishingTutorial2();
                        return;
                    }
                } if (isGameOver) {
                    // Reset flag and player data
                    isGameOver = false;
                    resetPlayerData();
                    
                    if (transitionWait) {
                        transitionWait = false;
                        handleStateTransition(GameState::MENU);
                    }
                    return;
                }

                if (transitionWait) {
                    if (currentState == GameState::FISHING) {
                        std::cout << "Cleaning up fishing after popup" << std::endl;
                        fishingView = nullptr;
                        fishingLogic = nullptr;
                    }
    
                    GameState newState = pendingState;
                    transitionWait = false;
    
                    if (newState == GameState::STORE) {
                        std::cout << "Transitioning to store" << std::endl;
                        storeView = std::make_unique<StoreView>(gameNotifier, window, renderer, playerData);
                        currentState = newState;
                    }
                }
            } else if (loanPaidPopup) {
                loanPaidPopup = false;
                
                if (transitionWait) {
                    transitionWait = false;
                    std::cout << "Creating hazard-free fishing after loan paid popup" << std::endl;
                    createFishingPhaseWithNoHazards();
                }
            }

            continue;
        }

        // Module-specific event handling
        switch (currentState) {
            case GameState::MENU:
                std::cout << "Processing menu event: " << e.type << std::endl;
                if (menuView) {
                    menuView->input(e);
                } else {
                    std::cout << "Warning: menuView is null!" << std::endl;
                }
                break;

            case GameState::STORE:
                if (storeView) storeView->input(e);
                break;

            case GameState::FISHING:
                if (fishingView) {
                    UserInput::Action event = fishingView->handleKeyboardInput(e);

                    if (event == UserInput::Action::QUIT) {
                        quitToMenu = true;
                    } else if (event != UserInput::Action::NONE && fishingLogic) {
                        fishingLogic->handleUserInput(event);
                    }
                }
                break;
                
            case GameState::PAUSED_FISHING:
                // Handle events while paused
                if (fishingView) {
                    fishingView->handlePauseMenuInput(e);
                }
                break;

            default:
                break;
        }
    }

    if (!eventProcessed) {
        // If no events were processed, add a small delay to prevent CPU usage when idle
        SDL_Delay(1);
    }

    if (quitToMenu) {
        handleStateTransition(GameState::MENU);
        return;
    }

    // Update game state for active modules
    if (currentState == GameState::FISHING && !fishPopup && !loanPaidPopup && fishingLogic && fishingView) {
        UserInput::Action continuousInput = fishingView->handleContinuousInput();
        fishingLogic->handleUserInput(continuousInput);

        fishingLogic->update();

        // Check if game state needs to change
        if (!transitionDone && (!fishingLogic->getRunning() || fishingLogic->getElapsedTime() >= fishingLogic->getRoundDuration())) {
            showFishPopup();
            return;
        }
    }
    // Check for any pending state transitions
    if (transitionWait && !fishPopup && !loanPaidPopup) {
        std::cout << "ProcessManager: Processing pending transition to state " << static_cast<int>(pendingState) << std::endl;
        transitionWait = false;
        handleStateTransition(pendingState);
    }
}

void ProcessManager::renderCurrentModule() {
    // Set default background color in case rendering fails
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    bool renderSuccessful = false;

    try {
        // Call module's render method
        //std::cout << "Rendering state " << static_cast<int>(currentState) << std::endl;

        switch (currentState) {
            case GameState::MENU:
                if (menuView) {
                    menuView->render();
                    renderSuccessful = true;
                } else {
                    std::cerr << "ERROR: menuView is null during render!" << std::endl;
                }
                break;

            case GameState::STORE:
                if (storeView) {
                    storeView->render();
                    renderSuccessful = true;
                } else {
                    std::cerr << "ERROR: storeView is null during render!" << std::endl;
                }
                break;

            case GameState::FISHING:
            case GameState::PAUSED_FISHING:  // Handle paused state the same way for rendering
                if (fishingView) {
                    fishingView->drawView();
                    renderSuccessful = true;
                } else {
                    std::cerr << "ERROR: fishingView is null during render!" << std::endl;
                }
                break;

            default:
                std::cerr << "ERROR: Unknown state during render: " << static_cast<int>(currentState) << std::endl;
                break;
        }

        // Render popup on top of background
        if ((fishPopup || loanPaidPopup) && popup) {
            popup->render();
        }

    } catch (const std::exception& e) {
        std::cerr << "ERROR during rendering: " << e.what() << std::endl;
        logSDLError("Render error");

        // If rendering fails completely, draw a simple fallback screen
        if (!renderSuccessful) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
            SDL_RenderClear(renderer);

            // We would render some error text here, but we can't rely on text rendering if it's failing
            SDL_Rect errorBox = {
                static_cast<int>(Dimens::SCREEN_W/4),
                static_cast<int>(Dimens::SCREEN_H/4),
                static_cast<int>(Dimens::SCREEN_W/2),
                static_cast<int>(Dimens::SCREEN_H/2)
            };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &errorBox);
        }
    }

    // Always attempt to present the renderer
    try {
        SDL_RenderPresent(renderer);
    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: Failed to present renderer: " << e.what() << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
    }
}

int ProcessManager::calculateFishingLevel() {
    return playerData.getDayCount();;
}

void ProcessManager::handleNotification(const std::string& notification) {
    std::cout << "ProcessManager::handleNotification('" << notification << "') called" << std::endl;

    // Store last notification for reference
    lastNote = notification;

    if (notification == "StartStore") {
        std::cout << "ProcessManager: Handling StartStore notification..." << std::endl;
        // Reset player data when starting a new game from the menu
        if (currentState == GameState::MENU) {
            resetPlayerData();
        }
        std::remove("victory_flag.txt"); // In case of game crash
        handleStateTransition(GameState::STORE);
        // Show tutorial if new
        if (!playerData.hasSeenTutorial()) {
            showStoreTutorial1();
        }
    } else if (notification == "ReturnMenu") {
        std::cout << "ProcessManager: Handling ReturnMenu notification..." << std::endl;
        handleStateTransition(GameState::MENU);
    } else if (notification == "GoFishing") {
        std::cout << "ProcessManager: Handling GoFishing notification..." << std::endl;
        handleStateTransition(GameState::FISHING);
        // Show tutorial if new
        if (!playerData.hasSeenTutorial() && currentTutorialStep == 2) {
            showFishingTutorial1();
        }
    } else if (notification == "ExitGame") {
        std::cout << "ProcessManager: Handling ExitGame notification, setting isRunning to false" << std::endl;
        isRunning = false;
    } else if (notification == "SaveGame") {
        std::cout << "ProcessManager: Handling SaveGame notification..." << std::endl;
        bool saveSuccess = saveGame();
        showSaveGamePopup(saveSuccess);
    } else if (notification == "LoadGame") {
        std::cout << "ProcessManager: Handling LoadGame notification..." << std::endl;
        bool loadSuccess = loadGame();
        showLoadGamePopup(loadSuccess);
    } else if (notification == "PauseFishing") {
        std::cout << "ProcessManager: Handling PauseFishing notification..." << std::endl;
        currentState = GameState::PAUSED_FISHING;
    } else if (notification == "ResumeFishing") {
        std::cout << "ProcessManager: Handling ResumeFishing notification..." << std::endl;
        currentState = GameState::FISHING;
    } else if (notification == "LoanPaid") {
        std::cout << "ProcessManager: Handling LoanPaid notification..." << std::endl;
        showLoanPaidPopup();
    } else if (notification.find("ChangeResolution:") == 0) {
        std::cout << "ProcessManager: Handling ChangeResolution notification..." << std::endl;
        // Parse the notification string for the resolution (could just pass small/med/large as notif, but I already wrote other methods)
        size_t firstColon = notification.find(':');
        size_t secondColon = notification.find(':', firstColon + 1);
        
        if (firstColon != std::string::npos && secondColon != std::string::npos) {
            std::string widthStr = notification.substr(firstColon + 1, secondColon - firstColon - 1);
            std::string heightStr = notification.substr(secondColon + 1);
            
            try {
                int width = std::stoi(widthStr);
                int height = std::stoi(heightStr);
                changeResolution(width, height);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid resolution values: " << e.what() << std::endl;
            }
        }
    } else if (notification == "ShowTutorial") {
        std::cout << "ProcessManager: Handling ShowTutorial notification..." << std::endl;
        currentTutorialStep = 0;
        showStoreTutorial1();
    } else {
        std::cerr << "ProcessManager: WARNING - Unknown notification: " << notification << std::endl;
    }
}

void ProcessManager::showFishPopup() {
    if (fishingLogic) {
        playerData.applyLedgerIncome();
        int catchScore = fishingLogic->getCatchScore();
        playerData.addFishCaught(catchScore);
        
        // Calculate earnings with cash register multiplier
        float multiplier = playerData.getCashMultiplier();
        int baseValue = catchScore * Game::BASE_SCORE_MULTIPLIER;
        int gross = static_cast<int>(baseValue * multiplier);
        int profit = gross - playerData.getRent();
        playerData.addCash(profit);

        if (playerData.getCash() < 0) {
            if (playerData.isLoanPaid()) {
                showSecretEndingPopup();
            } else {
            showGameOverPopup();
            return; // L bozo!
            }

        }

        std::vector<std::string> lines;
        lines.push_back("");
        lines.push_back("Fish Caught: $" + std::to_string(catchScore));
        lines.push_back("Money Earned: $" + std::to_string(gross) + 
                      (multiplier > 1.0f ? 
                       " (x" + (std::ostringstream() << std::setprecision(2) << std::defaultfloat << multiplier).str() + " multiplier)" : 
                       ""));
        lines.push_back("Rent Paid: $" + std::to_string(playerData.getRent()));
        lines.push_back("Profit: $" + std::to_string(profit));
        if (playerData.getLedgerIncome() > 0) {
            lines.push_back("Ledger Income: $" + std::to_string(playerData.getLedgerIncome()));
        }
        lines.push_back("");
        lines.push_back("Press SPACE to continue");

        popup->show("Fishing Results, Day " + std::to_string(playerData.getDayCount()), lines);
        fishPopup = true;

        playerData.setRent(playerData.getRent() * Game::RENT_INCREASE);
        playerData.incrementDay();

        fishingLogic->setRunning(false);

        pendingState = GameState::STORE;
        transitionWait = true; // queue gamestate transition
        transitionDone = true;
    }
}

/**
 * @brief Display popup when loan is fully paid off
 */
void ProcessManager::showLoanPaidPopup() {
    std::vector<std::string> lines;
    lines.push_back("");
    lines.push_back("You have fully paid off your loan!");
    lines.push_back("Enjoy the tranquility of fishing...");
    lines.push_back("");
    lines.push_back("Press SPACE to go fishing w/o hazards!");

    if (popup) {
        popup->show("Congratulations!", lines);
        loanPaidPopup = true;
        
        // Set up transition to hazard-free fishing when popup is closed
        pendingState = GameState::FISHING;
        transitionWait = true;
    } else {
        std::cerr << "Warning: Popup is not initialized!" << std::endl;
        // Fallback if popup doesn't work - go directly to fishing
        createFishingPhaseWithNoHazards();
    }
}

/**
 * @brief Creates a special fishing phase with no hazards as a reward for paying off the loan
 * Once again, a terrible, hacky solution to communicate b/w ProcessManager and FishingLogic w/o modifying parameters
 * @return bool Success indicator
 */
bool ProcessManager::createFishingPhaseWithNoHazards() {
    try {
        std::cout << "ProcessManager: Creating hazard-free fishing phase" << std::endl;
        if (fishingLogic) {
            fishingLogic->setRunning(false);
        }
        fishingView = nullptr;
        fishingLogic = nullptr;
        
        // Create a flag file to indicate loan is paid
        std::ofstream loanPaidFlag("victory_flag.txt");
        if (loanPaidFlag.is_open()) {
            loanPaidFlag << "Loan is paid off" << std::endl;
            loanPaidFlag.close();
        } else {
            std::cerr << "Warning: Could not create victory flag file" << std::endl;
        }
        
        // Init victory fishing phase
        int level = calculateFishingLevel();
        int roundDuration = Game::ROUND_DURATION + (playerData.getClockLevel() * 10 * 1000);
        fishingLogic = std::make_shared<FishingLogic>(level, roundDuration, playerData.getRodLevel());
        fishingLogic->setCallback([this](const std::string& notification) {
            this->handleNotification(notification);
        });
        
        // Create the fishing view
        fishingView = std::make_unique<FishingView>(fishingLogic, window, renderer, Dimens::SCREEN_H, Dimens::SCREEN_W);
        
        // Set up the fishing phase
        fishingLogic->setRunning(true);
        fishingLogic->spawnNewFloaters();
        fishingLogic->resetRoundStartTime();
        transitionDone = false;
        currentState = GameState::FISHING;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR creating hazard-free fishing phase: " << e.what() << std::endl;
        logSDLError("Hazard-free fishing creation error");
        return false;
    }
}

void ProcessManager::logSDLError(const std::string& message) {
    std::cerr << "SDL ERROR: " << message << ": " << SDL_GetError() << std::endl;

    // Only try to show a popup if we're fully initialized and not in an error state
    if (renderer && popup && isRunning) {
        try {
            std::vector<std::string> errorLines;
            errorLines.push_back(message);
            errorLines.push_back(SDL_GetError());
            errorLines.push_back("Press SPACE to continue");

            // Debug popup state before showing
            std::cout << "Creating error popup. Current popup state - fishPopup: "
                      << (fishPopup ? "true" : "false") << std::endl;

            // Store previous popup state
            bool wasFishPopup = fishPopup;

            // Show the error popup
            popup->show("Error", errorLines);

            // Set our error popup flag instead of using fishPopup
            bool errorPopupActive = true;

            // Process events until popup is closed
            SDL_Event e;
            while (errorPopupActive && SDL_WaitEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    isRunning = false;
                    errorPopupActive = false;
                }

                if (popup->handleEvent(e)) {
                    errorPopupActive = false;
                }
            }

            // Restore previous popup state
            fishPopup = wasFishPopup;

            std::cout << "Error popup closed. Restored popup state - fishPopup: "
                      << (fishPopup ? "true" : "false") << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to show error popup: " << e.what() << std::endl;
            // Make sure no popup flags are set if we failed
            fishPopup = false;
            transitionWait = false;
        }
    } else {
        // No popup possible, just log to console
        std::cerr << "Cannot show error popup - system not fully initialized" << std::endl;
    }
}

bool ProcessManager::saveGame() {
    std::cout << "ProcessManager: Saving game to " << SAVE_FILE_PATH << std::endl;
    return playerData.saveGame(SAVE_FILE_PATH);
}

bool ProcessManager::loadGame() {
    std::cout << "ProcessManager: Loading game from " << SAVE_FILE_PATH << std::endl;
    bool success = playerData.loadGame(SAVE_FILE_PATH);
    
    // If load was successful, transition to store mode
    if (success) {
        std::cout << "ProcessManager: Game loaded successfully, transitioning to store" << std::endl;
        // Queue transition to store after the popup is closed
        pendingState = GameState::STORE;
        transitionWait = true;
    }
    
    return success;
}

void ProcessManager::showSaveGamePopup(bool success) {
    std::vector<std::string> lines;
    if (success) {
        lines.push_back("Game saved successfully.");
    } else {
        lines.push_back("Failed to save the game.");
        lines.push_back("Please check file permissions.");
    }
    lines.push_back("");
    lines.push_back("Press SPACE to continue.");

    if (popup) {
        popup->show(success ? "Save Game" : "Save Failed", lines);
        fishPopup = true; // Reuse existing popup flag
    } else {
        std::cerr << "Warning: Popup is not initialized!" << std::endl;
    }
}

void ProcessManager::showLoadGamePopup(bool success) {
    std::vector<std::string> lines;
    if (success) {
        lines.push_back("Game loaded successfully.");
        lines.push_back("Cash: $" + std::to_string(playerData.getCash()));
        lines.push_back("Day: " + std::to_string(playerData.getDayCount()));
    } else {
        lines.push_back("Failed to load the game.");
        lines.push_back("No save file found or file is corrupted.");
    }
    lines.push_back("");
    lines.push_back("Press SPACE to continue.");

    if (popup) {
        popup->show(success ? "Load Game" : "Load Failed", lines);
        fishPopup = true; // Reuse existing popup flag
    } else {
        std::cerr << "Warning: Popup is not initialized!" << std::endl;
    }
}

void ProcessManager::resetPlayerData() {
    std::cout << "ProcessManager: Resetting player data for new game" << std::endl;
    playerData = PlayerData(); // Create a new PlayerData with default values
}

/* THESE SHOULD BE MOVED OUT OF PROCESS MANAGER */

void ProcessManager::showGameOverPopup() {
    std::vector<std::string> lines;
    lines.push_back("");
    lines.push_back("You've run out of money!");
    lines.push_back("Your business is officially bankrupt.");
    lines.push_back("");
    lines.push_back("Press SPACE to return to main menu");

    if (popup) {
        popup->show("Game Over", lines);
        fishPopup = true; // Reuse fishPopup for game over display
        
        // Set up transition to menu when popup is closed
        pendingState = GameState::MENU;
        transitionWait = true;
        isGameOver = true; // Set game over flag
    } else {
        std::cerr << "Warning: Popup is not initialized!" << std::endl;
        // If popup doesn't work kick directly to menu
        handleStateTransition(GameState::MENU);
    }
}

void ProcessManager::showStoreTutorial1() {
    std::vector<std::string> lines;
    lines.push_back("Welcome to your new fishing business!");
    lines.push_back("");
    lines.push_back("UPGRADES:");
    lines.push_back("- Rod: Catch larger fish and reel in faster");
    lines.push_back("- Clock: Increases fishing round duration");
    lines.push_back("- Cash Register: Increases earnings per fish");
    lines.push_back("- Ledger: Provides daily passive income");
    lines.push_back("");
    lines.push_back("Press SPACE to continue...");

    if (popup) {
        popup->show("Store Tutorial (1/2)", lines);
        fishPopup = true;
        currentTutorialStep = 1;
    }
}

void ProcessManager::showStoreTutorial2() {
    std::vector<std::string> lines;
    lines.push_back("ECONOMICS:");
    lines.push_back("- Cash: Your current money");
    lines.push_back("- Rent: Due after each day of fishing");
    lines.push_back("- Loan: You must pay this off to win the game");
    lines.push_back("- BANKRUPTCY: If your cash goes below 0,");
    lines.push_back("  you'll lose the game!");
    lines.push_back("");
    lines.push_back("Go fishing by clicking the button!");
    lines.push_back("Press SPACE to continue...");

    if (popup) {
        popup->show("Store Tutorial (2/2)", lines);
        fishPopup = true;
        currentTutorialStep = 2;
    }
}

void ProcessManager::showFishingTutorial1() {
    std::vector<std::string> lines;
    lines.push_back("FISHING CONTROLS:");
    lines.push_back("- Arrow keys: Control rod");
    lines.push_back("- SHIFT key: Release caught fish/hazard");
    lines.push_back("- ESC or P: Pause the game");
    lines.push_back("");
    lines.push_back("Catch fish by touching them with your hook");
    lines.push_back("and reel them to the surface to score points!");
    lines.push_back("");
    lines.push_back("Press SPACE to continue...");

    if (popup) {
        popup->show("Fishing Tutorial (1/2)", lines);
        fishPopup = true;
        currentTutorialStep = 3;
    }
}

void ProcessManager::showFishingTutorial2() {
    std::vector<std::string> lines;
    lines.push_back("HAZARDS:");
    lines.push_back("- Barracuda: Eats your fish");
    lines.push_back("- Skeleton: Stuns your rod in fear!");
    lines.push_back("- Cannon: Must be slowly reeled to surface");
    lines.push_back("");
    lines.push_back("Watch out for hazards as they will reduce");
    lines.push_back("your score or slow you down!");
    lines.push_back("");
    lines.push_back("Press SPACE to begin fishing!");

    if (popup) {
        popup->show("Fishing Tutorial (2/2)", lines);
        fishPopup = true;
        currentTutorialStep = 4;
        
        // Mark tutorials as seen
        playerData.setTutorialSeen();
        if (fishingLogic) {
            fishingLogic->resetRoundStartTime();
        }
    }
}

// Special popup for when the player goes bankrupt after paying off the loan
void ProcessManager::showSecretEndingPopup() {
    std::vector<std::string> lines;
    lines.push_back("");
    lines.push_back("Congratulations on paying off your loan!");
    lines.push_back("But your business still went bankrupt...");
    lines.push_back("");
    lines.push_back("You've discovered the secret ending!");
    lines.push_back("Freedom from debt, but still broke.");
    lines.push_back("");
    lines.push_back("Press SPACE to return to main menu");

    if (popup) {
        popup->show("Secret Ending: Bittersweet Victory", lines);
        fishPopup = true; // Reuse fishPopup for secret ending display
        
        // Set up transition to menu when popup is closed
        pendingState = GameState::MENU;
        transitionWait = true;
        isGameOver = true; // Set game over flag
    } else {
        std::cerr << "Warning: Popup is not initialized!" << std::endl;
        // If popup doesn't work kick directly to menu
        handleStateTransition(GameState::MENU);
    }
}

/**
 * @brief Allows the user to change resolution
 * @details Yo dawg, I heard you liked process management so I put real process management
 * in your process manager.  This is probably my jank magnum opus - it was easy to set up and
 * should work consistently with POSIX, but is almost definitely worst way to do this.
 * @details for real: This sets a new config file, forks a child process, then kills
 * and waits for the parent zombie so you don't get a chain of game windows hanging open/
 * have to launch additional shells.
 */
void ProcessManager::changeResolution(int width, int height) {
#if IS_WINDOWS
    std::cout << "Resolution change is not supported on Windows." << std::endl;
    std::cout << "Please modify game_config.txt manually and restart the game." << std::endl;

    // Still write the configuration file so it takes effect on next launch
    std::ofstream configFile("game_config.txt");
    if (configFile.is_open()) {
        configFile << "resolution_width=" << width << std::endl;
        configFile << "resolution_height=" << height << std::endl;
        configFile.close();
        
        std::cout << "Wrote resolution to config file: " << width << "x" << height << std::endl;
        std::cout << "The new resolution will take effect when you restart the game." << std::endl;
    } else {
        std::cerr << "Failed to write config file!" << std::endl;
    }
    return;
#else // Non-Windows
    // Write res to config
    std::ofstream configFile("game_config.txt");
    if (configFile.is_open()) {
        configFile << "resolution_width=" << width << std::endl;
        configFile << "resolution_height=" << height << std::endl;
        configFile.close();
        
        std::cout << "Wrote resolution to config file: " << width << "x" << height << std::endl;
    } else {
        std::cerr << "Failed to write config file!" << std::endl;
        return;
    }
    
    // Get the absolute path
    char exePath[PATH_MAX] = {0};
    
    #ifdef __APPLE__ // Mac specific
        uint32_t size = sizeof(exePath);
        if (_NSGetExecutablePath(exePath, &size) != 0) {
            std::cerr << "Failed to get exe path" << std::endl;
            strcpy(exePath, "./fish"); // Fallback to relative
        }
    #elif defined(__linux__) // Linux specific
        ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
        if (len == -1) {
            std::cerr << "Failed to get exe path: " << strerror(errno) << std::endl;
            strcpy(exePath, "./fish"); // Fallback to relative
        } else {
            exePath[len] = '\0'; // Null terminate
        }
    #else
        // Default fallback for other platforms
        strcpy(exePath, "./fish");
    #endif
        // L Windows, no POSIX no resolution change

        // Get current dir
        char cwd[PATH_MAX] = {0};
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            std::cerr << "Failed to get current working dir: " << strerror(errno) << std::endl;
            // Fallback to relative path
        }
        
        std::cout << "Starting new process from: " << cwd << std::endl;
        std::cout << "Executable path: " << exePath << std::endl;
        
        // Fork
        pid_t pid = fork();
        if (pid == 0) {
            // Child - double fork to prevent zombie
            pid_t pid2 = fork();
            
            if (pid2 == 0) {
                // Grandchild - detach from terminal control
                setsid();
                
                // Redirect stdin - this doesn't do what I wanted but it doesn't hurt
                int nullfd = open("/dev/null", O_RDWR);
                if (nullfd >= 0) {
                    dup2(nullfd, STDIN_FILENO);
                    close(nullfd);
                }
                if (chdir(cwd) != 0) { // Change dir
                    std::cerr << "Failed to change directory to " << cwd << ": " << strerror(errno) << std::endl;
                }
                
                // Execute
                execl(exePath, exePath, NULL);
                std::cerr << "Failed to execute " << exePath << ": " << strerror(errno) << std::endl;
                _exit(1);
            } 
            else if (pid2 > 0) {
                // Child - exit
                _exit(0);
            }
            else {
                // Fork failed
                std::cerr << "Second fork failed: " << strerror(errno) << std::endl;
                _exit(1);
            }
        } 
        else if (pid > 0) {
            // Parent - wait for child
            int status;
            waitpid(pid, &status, 0);

            // Clean up SDL
            TTF_Quit();
            IMG_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            
            // Flush streams for posterity
            std::cout.flush();
            std::cerr.flush();
            fflush(stdout);
            fflush(stderr);
            
            // Exit parent
            _exit(0);
        }
        else {
            // Fork failed
            std::cerr << "First fork failed: " << strerror(errno) << std::endl;
        }
    #endif
}
