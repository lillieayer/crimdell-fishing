#ifndef FISHING_LOGIC_HPP
#define FISHING_LOGIC_HPP

#include <vector> // For fish/hazard vectors
#include <memory> // For fish/hazard pointers
#include <SDL.h> // For Uint
#include <fstream> // For victory mode ez flag
#include <functional>

#include "rod.hpp"
#include "fish.hpp"
#include "medium_fish.hpp"
#include "big_fish.hpp"
#include "hazard.hpp"
#include "heavy_hazard.hpp"
#include "shock_hazard.hpp"
#include "floater.hpp"
#include "game_constants.hpp"
#include "view_adapter.hpp"
#include "Vector2D.hpp"


/**
 * @class FishingLogic
 * @brief Logic for the fishing phase; holds current gamestate
 */
class FishingLogic
{
    private:
        bool running = true;
        bool paused = false;
        // reserve vector capacity based on level chosen
        std::vector<std::shared_ptr<Fish>> allFish;
        Rod rod;
        std::vector<std::shared_ptr<Hazard>> hazards;
        std::shared_ptr<Hazard> caughtHeavyHazardPtr;
        bool stunned = false;
        Uint32 stunEndTime = 0;
        int catchScore = 0; // initialize
        std::shared_ptr<Fish> caughtFishPtr;
        std::vector<float> spawnLocations;
        int roundStartTime;
        int elapsedTime = 0;
        int pauseStartTime = 0;  // For tracking pause time
        int totalPauseTime = 0;  // Total pause duration to adjust timing
        int spawnInterval;
        int lastSpawnTime;
        void handleCatch();
        float randomizeDir();
        void spawnHazards();
        void initHazards(int numNormalHazards, int numHeavyHazards, int numShockHazards, float speedMultiplier);
        void initFish(int numSmallFish, int numMedFish, int numBigFish);
        void initFishHazards(int dayCount, bool isHazardFree);
        bool checkFloaterCaught(const std::vector<Vector2D>& floaterDetails);
        int lastCheckTime = 0;
        int roundDuration;
        int playerRodLevel;
        
        // Callback handler for pause menu
        std::function<void(const std::string&)> notificationCallback;

    public:
        FishingLogic(int dayCount, int roundDuration, int rodLevel);
        void setRunning(bool newState) { running = newState; }
        bool getRunning() const { return running; }
        void setPaused(bool newState);
        bool isPaused() const { return paused; }
        int getCatchScore() const { return catchScore; }
        int getStartTime() const { return roundStartTime; }
        int getElapsedTime() const { return elapsedTime; }
        Rod getRod() const { return rod; }
        std::vector<std::shared_ptr<Hazard>>& getHazardsRef();
        std::vector<std::shared_ptr<Fish>>& getAllFishRef();
        void releaseFish();
        void releaseHeavyHazard();
        void spawnNewFloaters();
        void handleUserInput(UserInput::Action userInput);
        void update();
        int calculateRandNum(int lower, int upper);
        float calculateRandSpeed(std::shared_ptr<Fish>& fishPtr);
        int getRoundDuration() const { return roundDuration; }
        void resetRoundStartTime();
        
        // Callback handler for pause menu
        void setCallback(std::function<void(const std::string&)> callback);
        void pauseGame();
        void resumeGame();
        void quitToMenu();
};

#endif // FISHING_LOGIC_HPP