#include <iostream> // For debug text
#include <random> // For random spawn generation
#include <memory> // For shared pointer
#include <vector> // For storing fish/hazard pointers
#include <algorithm>
#include <SDL.h>

#include "fishing_logic.hpp" // Class Header
#include "fishing_view.hpp" // For view
#include "Vector2D.hpp" // For location vector
#include "floater.hpp" // For floater superclass
#include "fish.hpp" // For small fish class
#include "medium_fish.hpp" // For medium fish class
#include "big_fish.hpp" // For big fish class
#include "rod.hpp" // For rod class
#include "hazard.hpp" // For hazard class
#include "heavy_hazard.hpp" // For heavy hazard class
#include "shock_hazard.hpp" // For shock hazard class
#include "game_constants.hpp" // For game values
#include "scaling.hpp" // For scaling helper methods

using namespace std;

FishingLogic::FishingLogic(int dayCount, int roundDuration, int rodLevel) : 
    rod(Rod(1.0f + (rodLevel * 0.1f))), 
    roundStartTime(0), // Initialize to 0 instead of current time
    roundDuration(roundDuration), 
    playerRodLevel(rodLevel)
{
    // Initialize scaled spawn locations
    float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);
    float maxRodLength = Scale::scaleH(Game::ROD_MAX_LENGTH_PERCENT);
    float heightScale = Scale::getHeightScaleFactor();
    spawnLocations = {
        surfacePond + (50 * heightScale),
        surfacePond + (130 * heightScale),
        surfacePond + (200 * heightScale),
        surfacePond + (280 * heightScale),
        surfacePond + (330 * heightScale),
        surfacePond + (400 * heightScale),
        surfacePond + (480 * heightScale),
        maxRodLength
    };

    // This is a terrible, hacky solution to implement victory mode without changing things other methods :D
    bool isHazardFree = false;    
    std::ifstream loanPaidFile("victory_flag.txt");
    if (loanPaidFile.good()) {
        isHazardFree = true;
        loanPaidFile.close();
    }
    
    // Initialize fish and hazards based on day count
    initFishHazards(dayCount, isHazardFree);
    
    spawnInterval = 2000;
    lastSpawnTime = 0; // Initialize to 0 instead of current time
    
    // Actual start time is set later
    resetRoundStartTime();
}

/**
 * @brief Initializes fish and hazards based on day-specific level configurations
 * @param dayCount Current day number (1-10+)
 * @param isHazardFree Whether hazards should be disabled (victory mode)
 */
void FishingLogic::initFishHazards(int dayCount, bool isHazardFree) {
    // Clamp to 1-10, 10 is max difficulty
    int effectiveDay = std::max(1, std::min(dayCount, 10));
    
    cout << "Initializing fishing for day " << dayCount << " (clamped to " << effectiveDay << ")" << endl;
    
    int numSmallFish = 0;
    int numMediumFish = 0;
    int numBigFish = 0;
    
    int numNormalHazards = 0;
    int numHeavyHazards = 0;
    int numShockHazards = 0;
    
    float hazardSpeedMultiplier = 1.0f;
    
    // Determine the fish and hazard counts based on day
    switch (effectiveDay) {
        case 1:
            numSmallFish = Level::Day1::SMALL_FISH;
            numMediumFish = Level::Day1::MEDIUM_FISH;
            numBigFish = Level::Day1::BIG_FISH;
            
            numNormalHazards = Level::Day1::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day1::HEAVY_HAZARDS;
            numShockHazards = Level::Day1::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day1::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 2:
            numSmallFish = Level::Day2::SMALL_FISH;
            numMediumFish = Level::Day2::MEDIUM_FISH;
            numBigFish = Level::Day2::BIG_FISH;
            
            numNormalHazards = Level::Day2::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day2::HEAVY_HAZARDS;
            numShockHazards = Level::Day2::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day2::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 3:
            numSmallFish = Level::Day3::SMALL_FISH;
            numMediumFish = Level::Day3::MEDIUM_FISH;
            numBigFish = Level::Day3::BIG_FISH;
            
            numNormalHazards = Level::Day3::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day3::HEAVY_HAZARDS;
            numShockHazards = Level::Day3::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day3::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 4:
            numSmallFish = Level::Day4::SMALL_FISH;
            numMediumFish = Level::Day4::MEDIUM_FISH;
            numBigFish = Level::Day4::BIG_FISH;
            
            numNormalHazards = Level::Day4::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day4::HEAVY_HAZARDS;
            numShockHazards = Level::Day4::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day4::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 5:
            numSmallFish = Level::Day5::SMALL_FISH;
            numMediumFish = Level::Day5::MEDIUM_FISH;
            numBigFish = Level::Day5::BIG_FISH;
            
            numNormalHazards = Level::Day5::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day5::HEAVY_HAZARDS;
            numShockHazards = Level::Day5::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day5::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 6:
            numSmallFish = Level::Day6::SMALL_FISH;
            numMediumFish = Level::Day6::MEDIUM_FISH;
            numBigFish = Level::Day6::BIG_FISH;
            
            numNormalHazards = Level::Day6::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day6::HEAVY_HAZARDS;
            numShockHazards = Level::Day6::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day6::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 7:
            numSmallFish = Level::Day7::SMALL_FISH;
            numMediumFish = Level::Day7::MEDIUM_FISH;
            numBigFish = Level::Day7::BIG_FISH;
            
            numNormalHazards = Level::Day7::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day7::HEAVY_HAZARDS;
            numShockHazards = Level::Day7::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day7::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 8:
            numSmallFish = Level::Day8::SMALL_FISH;
            numMediumFish = Level::Day8::MEDIUM_FISH;
            numBigFish = Level::Day8::BIG_FISH;
            
            numNormalHazards = Level::Day8::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day8::HEAVY_HAZARDS;
            numShockHazards = Level::Day8::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day8::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 9:
            numSmallFish = Level::Day9::SMALL_FISH;
            numMediumFish = Level::Day9::MEDIUM_FISH;
            numBigFish = Level::Day9::BIG_FISH;
            
            numNormalHazards = Level::Day9::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day9::HEAVY_HAZARDS;
            numShockHazards = Level::Day9::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day9::HAZARD_SPEED_MULTIPLIER;
            break;
            
        case 10:
        default:
            numSmallFish = Level::Day10::SMALL_FISH;
            numMediumFish = Level::Day10::MEDIUM_FISH;
            numBigFish = Level::Day10::BIG_FISH;
            
            numNormalHazards = Level::Day10::NORMAL_HAZARDS;
            numHeavyHazards = Level::Day10::HEAVY_HAZARDS;
            numShockHazards = Level::Day10::SHOCK_HAZARDS;
            
            hazardSpeedMultiplier = Level::Day10::HAZARD_SPEED_MULTIPLIER;
            break;
    }
    
    initFish(numSmallFish, numMediumFish, numBigFish); // Initialize hazards with the determined counts
    if (isHazardFree) { // Skip hazard spawning if in victory mode
        cout << "Creating hazard-free fishing round!" << endl;
        return;
    }
    initHazards(numNormalHazards, numHeavyHazards, numShockHazards, hazardSpeedMultiplier); // Initialize hazards with the determined counts

}

/**
 * @brief Calculates a random number between an inclusive range
 * @param lower Lower bound for rand generation
 * @param upper Upper bound for rand generation
 * @return int randomized value
 */
int FishingLogic::calculateRandNum(int lower, int upper)
{
    // create random seed
    std::random_device random_seed;
    // create random engine/generator
    std::default_random_engine gen(random_seed());
    // use distribution to simulate choice for angle (0 to 45)
    std::uniform_int_distribution<int> distribution(lower,upper);
    int randNum = distribution(gen);
    return randNum;
}

/**
 * @brief Randomizes the speed of a fish within bounds based on size and window size
 * @param fishPtr fish reference
 * @return float randomized speed value
 */
float FishingLogic::calculateRandSpeed(std::shared_ptr<Fish>& fishPtr)
{
    float speedScale = Scale::getWidthScaleFactor();

    if(dynamic_cast<MediumFish*>(fishPtr.get()) != nullptr)
    {
        return speedScale * (float)calculateRandNum(Game::MAX_MEDIUM_FISH_SPEED - 2, Game::MAX_MEDIUM_FISH_SPEED);
    } else if(dynamic_cast<BigFish*>(fishPtr.get()) != nullptr)
    {
        return speedScale * (float)calculateRandNum(Game::MAX_BIG_FISH_SPEED - 2, Game::MAX_BIG_FISH_SPEED);
    } else
    {
        return speedScale * (float)calculateRandNum(Game::MAX_FISH_SPEED - 2, Game::MAX_FISH_SPEED);
    }
}

/**
 * @brief getter for fish vector
 * @return reference to fish vector
 * @details Fish vector will get more expensive to store/send as more fish added so return ref
 */
std::vector<std::shared_ptr<Fish>>& FishingLogic::getAllFishRef()
{
    return allFish;
}

/**
 * @brief getter for hazard vector
 * @return reference to hazard vector
 * @details Hazard vector will get more expensive to store/send as more fish added so return ref
 */
std::vector<std::shared_ptr<Hazard>>& FishingLogic::getHazardsRef()
{
    return hazards;
}

/**
 * @brief Handles user input to control rod
 * @param User input event
 */
void FishingLogic::handleUserInput(UserInput::Action userInput)
{
    // Handle pause/resume input
    if (userInput == UserInput::Action::START) {
        if (!paused) {
            pauseGame();
        }
        return;
    }
    
    // Don't process other inputs if paused
    if (paused) {
        return;
    }
    
    // Check for released fish
    if (userInput == UserInput::Action::RELEASE) 
    {   
        if (caughtFishPtr) {
            releaseFish();
        }
        else if (caughtHeavyHazardPtr) {
            releaseHeavyHazard();
        }
    }

    // Block all non-quit actions if stunned
    if (stunned)
    {
        if (userInput == UserInput::Action::QUIT) {
            running = false;
        }
        // Ignore all other inputs while stunned
        return;
    }
    
    // Get the current rod speed w/ heavy hazard multiplier
    float currentRodSpeed = rod.getMaxVelocity();
    if (caughtHeavyHazardPtr) {
        // Apply the heavy hazard's speed reduction when it's caught
        auto heavyHazard = dynamic_cast<HeavyHazard*>(caughtHeavyHazardPtr.get());
        if (heavyHazard) {
            currentRodSpeed *= heavyHazard->getSlowFactor();
        }
    }
    
    switch (userInput)
    {
        case UserInput::Action::MOVEDOWN:
            // If heavy hazard is caught, reduce rod speed
            if (caughtHeavyHazardPtr) {
                rod.castDownSpeed(currentRodSpeed);
            } else {
                rod.castDown();
            }
            
            // If reeling a fish or heavy hazard, match its motion to the rod
            if (caughtFishPtr) {
                // Get the current speed magnitude and direction from the rod
                float speedMagnitude = rod.getSpeed()[1];
                float sameX = caughtFishPtr->getDirection()[0];
                caughtFishPtr->setDirection(sameX, rod.getDirection()[1]);
                caughtFishPtr->setSpeed(0, speedMagnitude);
            }
            else if (caughtHeavyHazardPtr) {
                // Get the current speed magnitude and direction from the rod
                float speedMagnitude = rod.getSpeed()[1];
                float sameX = caughtHeavyHazardPtr->getDirection()[0];
                caughtHeavyHazardPtr->setDirection(sameX, rod.getDirection()[1]);
                caughtHeavyHazardPtr->setSpeed(0, speedMagnitude);
            }
            break;

        case UserInput::Action::MOVEUP:
            // If heavy hazard is caught, reduce rod speed
            if (caughtHeavyHazardPtr) {
                rod.castUpSpeed(currentRodSpeed);
            } else {
                rod.castUp();
            }
            
            if (caughtFishPtr) {
                // Get the current speed magnitude and direction from the rod
                float speedMagnitude = rod.getSpeed()[1];
                float sameX = caughtFishPtr->getDirection()[0];
                caughtFishPtr->setDirection(sameX, rod.getDirection()[1]);
                caughtFishPtr->setSpeed(0, speedMagnitude);
            }
            else if (caughtHeavyHazardPtr) {
                // Get the current speed magnitude and direction from the rod
                float speedMagnitude = rod.getSpeed()[1];
                float sameX = caughtHeavyHazardPtr->getDirection()[0];
                caughtHeavyHazardPtr->setDirection(sameX, rod.getDirection()[1]);
                caughtHeavyHazardPtr->setSpeed(0, speedMagnitude);
            }
            break;

        case UserInput::Action::NONE:
            rod.stopMovement();
            break;

        case UserInput::Action::QUIT:
            running = false;
            break;
            
        default:
            break;
    }
}

/**
 * @brief Calculates a random direction factor (left or right) to be used to multiply w the speed of the fish
 * @return float Direction for the fish to swim (+/- 1.0)
 */
float FishingLogic::randomizeDir()
{

    int randDir = calculateRandNum(0,1);
    if (randDir == 0 ) return -1.0f;
    else return 1.0f;
}

/**
 * @brief Tracks and spawns new fish if necessary
 */
void FishingLogic::spawnNewFloaters()
{
    // Determine how many total fish can be activated (allowed on screen) this cycle
    int numFishActivated = calculateRandNum(1,allFish.size());
    // Debug
    cout << "num fish total: " << allFish.size() << endl;
    cout << "num fish activated: " << numFishActivated << endl;
    while (numFishActivated > 0) {
        int randi = calculateRandNum(0, allFish.size() - 1);
        cout << "random fish spawned: " << randi << endl;
        // if randomly selected fish is not already active, spawn it
        if (!allFish[randi]->isActive()) {
            float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);
            float maxLength = Scale::scaleW(Game::ROD_MAX_LENGTH_PERCENT);

            int randY = calculateRandNum(surfacePond + 10, maxLength - (int)allFish[randi]->getDimens()[1]);
            float randDir = randomizeDir();
            allFish[randi]->setDirection(randDir, 1);
            // set start x pos based on dir of fish
            randDir > 0 ? allFish[randi]->setPosition(0,randY) : allFish[randi]->setPosition(Dimens::SCREEN_W - (int)allFish[randi]->getDimens()[0],randY);
            allFish[randi]->setSpeed(calculateRandSpeed(allFish[randi]),0);
            allFish[randi]->setActive(true);
        }
        // either way activated fish has been spawned
        numFishActivated--;
    }
}

/**
 * @brief Tracks and spawns new hazards if necessary
 */
void FishingLogic::spawnHazards()
{
    if (hazards.empty()) {
        return;
    }

    // randomly determine how many hazards can be activated (on screen) this cycle (every 2 sec)
    int numHazardsActivated = calculateRandNum(1,hazards.size());
    while (numHazardsActivated > 0) {
        // randomly select a hazard to spawn
        int randi = calculateRandNum(0, hazards.size() - 1);
        // if randomly selected hazard is not already active, spawn it
        if (!hazards[randi]->isActive()) {
            // Scale spawn position and size
        
            float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);
            float maxLength = Scale::scaleH(Game::ROD_MAX_LENGTH_PERCENT);

            float hazardH, hazardW;
            if (dynamic_cast<ShockHazard*>(hazards[randi].get())) {
                hazardH = Scale::scaleH(Visuals::SHOCK_HAZARD_HEIGHT_PERCENT);
                hazardW = Scale::scaleW(Visuals::SHOCK_HAZARD_WIDTH_PERCENT);
            } else if (dynamic_cast<HeavyHazard*>(hazards[randi].get())) {
                hazardH = Scale::scaleH(Visuals::HEAVY_HAZARD_HEIGHT_PERCENT);
                hazardW = Scale::scaleW(Visuals::HEAVY_HAZARD_WIDTH_PERCENT);
            } else {
                hazardH = Scale::scaleH(Visuals::HAZARD_HEIGHT_PERCENT);
                hazardW = Scale::scaleW(Visuals::HAZARD_WIDTH_PERCENT);
            }

            int randY = calculateRandNum(surfacePond + 10, maxLength - (int)hazardH);
            float randDir = randomizeDir();

            // Check direction of hazard movement to adjust spawn x position
            if (randDir < 0) {  // Spawn off right side
                hazards[randi]->setPosition(Dimens::SCREEN_W, randY);
                hazards[randi]->setDirection(randDir, 0); // Move left
            } else {
                hazards[randi]->setPosition(-hazardW, randY);   // Spawn off left side
                hazards[randi]->setDirection(randDir, 0); // Move right
            }

            hazards[randi]->setSpeed(Game::MAX_HAZARD_SPEED * Scale::getWidthScaleFactor(),0);
            // Activate hazard
            hazards[randi]->setActive(true);
        }
        // either way random hazard has either been activated or already was
        numHazardsActivated--;
    }
}

/**
 * @brief initialize new hazards and add pointer to vector
 * @param numNormalHazards Number of normal hazards to add
 * @param numHeavyHazards Number of heavy hazards to add
 * @param numShockHazards Number of shock hazards to add
 * @param speedMultiplier Speed multiplier to apply to hazards
 */
void FishingLogic::initHazards(int numNormalHazards, int numHeavyHazards, int numShockHazards, float speedMultiplier)
{
    // Base hazard speed with multiplier applied
    float hazardSpeed = Game::MAX_HAZARD_SPEED * Scale::getWidthScaleFactor() * speedMultiplier;
    
    cout << "Initializing hazards: " << numNormalHazards << " normal, " 
         << numHeavyHazards << " heavy, " << numShockHazards 
         << " shock with speed multiplier " << speedMultiplier << endl;
    
    // Create normal hazards
    for (int i = 0; i < numNormalHazards; i++) {
        float hazardW = Scale::scaleW(Visuals::HAZARD_WIDTH_PERCENT);
        float hazardH = Scale::scaleH(Visuals::HAZARD_HEIGHT_PERCENT);
        auto hazard = std::make_shared<Hazard>(hazardW, hazardH, hazardSpeed);
        hazards.push_back(hazard);
    }
    
    // Create heavy hazards
    for (int i = 0; i < numHeavyHazards; i++) {
        float heavyHazardW = Scale::scaleW(Visuals::HEAVY_HAZARD_WIDTH_PERCENT);
        float heavyHazardH = Scale::scaleH(Visuals::HEAVY_HAZARD_HEIGHT_PERCENT);
        auto hazard = std::make_shared<HeavyHazard>(heavyHazardW, heavyHazardH, hazardSpeed);
        hazards.push_back(hazard);
    }
    
    // Create shock hazards
    for (int i = 0; i < numShockHazards; i++) {
        float shockHazardW = Scale::scaleW(Visuals::SHOCK_HAZARD_WIDTH_PERCENT);
        float shockHazardH = Scale::scaleH(Visuals::SHOCK_HAZARD_HEIGHT_PERCENT);
        auto hazard = std::make_shared<ShockHazard>(shockHazardW, shockHazardH, hazardSpeed);
        hazards.push_back(hazard);
    }
}

/**
 * @brief initialize new fish of each type and add pointer to vector
 * @param numSmallFish number of small fish
 * @param numMedFish number of medium fish
 * @param numBigFish number of large fish
 */
void FishingLogic::initFish(int numSmallFish, int numMedFish, int numBigFish)
{
    // Scale fish dimensions
    float smallFishW = Scale::scaleW(Visuals::FISH_WIDTH_PERCENT);
    float smallFishH = Scale::scaleH(Visuals::FISH_HEIGHT_PERCENT);
    float smallFishSpeed = Game::MAX_FISH_SPEED * Scale::getWidthScaleFactor();

    float medFishW = Scale::scaleW(Visuals::MEDIUM_FISH_WIDTH_PERCENT);
    float medFishH = Scale::scaleH(Visuals::MEDIUM_FISH_HEIGHT_PERCENT);
    float medFishSpeed = Game::MAX_MEDIUM_FISH_SPEED * Scale::getWidthScaleFactor();

    float bigFishW = Scale::scaleW(Visuals::BIG_FISH_WIDTH_PERCENT);
    float bigFishH = Scale::scaleH(Visuals::BIG_FISH_HEIGHT_PERCENT);
    float bigFishSpeed = Game::MAX_BIG_FISH_SPEED * Scale::getWidthScaleFactor();

    cout << "Initializing fish: " << numSmallFish << " small, " 
         << numMedFish << " medium, " << numBigFish << " big" << endl;

    int totalFish = numSmallFish + numMedFish + numBigFish;
    while (totalFish > 0)
    {
        if (numSmallFish > 0)
        {
            allFish.push_back(std::make_shared<Fish>(smallFishW, smallFishH, smallFishSpeed));
            numSmallFish--;
        }
        else if (numMedFish > 0)
        {
            allFish.push_back(std::make_shared<MediumFish>(medFishW, medFishH, medFishSpeed));
            numMedFish--;
        }
        else if (numBigFish > 0)
        {
            allFish.push_back(std::make_shared<BigFish>(bigFishW, bigFishH, bigFishSpeed));
            numBigFish--;
        }
        // recalculate total fish
        totalFish = numSmallFish + numMedFish + numBigFish;
    }
}

/**
 * @brief tests if a floater has intersected the rod
 * @param floaterDetails Vector with floater location and size
 * @return bool true if intersection, false if not
 */
bool FishingLogic::checkFloaterCaught(const std::vector<Vector2D>& floaterDetails) {
    //std::cout << "Checked floater caught" << std::endl;
    int floaterX = floaterDetails[0][0];
    int floaterY = floaterDetails[0][1];
    int floaterW = floaterDetails[3][0];
    int floaterH = floaterDetails[3][1];

    // Create hitbox at fish's mouth
    int hitboxSize = floaterH / 2;
    int hitboxX, hitboxY;
    if (floaterDetails[1][0] > 0) {
        hitboxX = floaterX + floaterW - hitboxSize - 5;
    } else {
        hitboxX = floaterX + 5;
    }
    hitboxY = floaterY + (floaterH - hitboxSize) / 2;

    int rodX = rod.getPosition()[0];
    int rodY = rod.getPosition()[1];

    int rodRX = Scale::scaleW(Game::ROD_RADIUSX_PERCENT) * 1.2f; // slightly bigger rod hitbox than visual
    int rodRY = Scale::scaleH(Game::ROD_RADIUSY_PERCENT) * 1.2f;

    if ((rodY + rodRY > hitboxY) && (rodY - rodRY < hitboxY + hitboxSize)) {
        if ((hitboxX + hitboxSize > rodX - rodRX) && (hitboxX < rodX + rodRX)) {
            // Check if this is a fish (not a hazard)
            for (auto& fish : allFish) {
                if (fish->isActive() && 
                    fish->getPosition()[0] == floaterDetails[0][0] && 
                    fish->getPosition()[1] == floaterDetails[0][1]) {
                    // Check fish type against rod level
                    if (dynamic_cast<MediumFish*>(fish.get()) && playerRodLevel < 2) {
                        return false;
                    }
                    if (dynamic_cast<BigFish*>(fish.get()) && playerRodLevel < 4) {
                        return false;
                    }
                    return true;
                }
            }
            return true; // For hazards or untyped floaters
        }
    }
    return false;
}

/**
 * @brief handles logic when a rod intersects fish or hazard
 */
void FishingLogic::handleCatch()
{
    //std::cout << "Handling catch" << std::endl;
    for (int i = 0; i < allFish.size(); i++)
    {
        if ((!caughtFishPtr) && (allFish[i]->isActive()))
        {
            std::vector<Vector2D> fishDetails = {allFish[i]->getPosition(), allFish[i]->getDirection(), allFish[i]->getSpeed(), allFish[i]->getDimens()};
            // check if catch target on fish collides with rod
            if (checkFloaterCaught(fishDetails)){
                caughtFishPtr = allFish[i];
                caughtFishPtr->setCaught(true);
                // rotate fish by inverting height and width
                float oldH = caughtFishPtr->getDimens()[1];
                float oldW = caughtFishPtr->getDimens()[0];
                caughtFishPtr->setDimens(oldH, oldW);
                // set fish x (top right corner) to rod x depending on direction of swimming
                float middleOfFish = caughtFishPtr->getDimens()[0]/2;
                caughtFishPtr->setPosition(rod.getPosition()[0] + middleOfFish, rod.getPosition()[1]);
            }
        }
    }

    //Handles the behavior of hazards when intersected by the rod
    for (int i = 0; i < hazards.size(); i++)
    {
        if (hazards[i]->isActive())
        {
            std::vector<Vector2D> hazardDetails = {hazards[i]->getPosition(), hazards[i]->getDirection(), hazards[i]->getSpeed(), hazards[i]->getDimens()};
            if (checkFloaterCaught(hazardDetails))
            {
                //Behavior for knocking fish off of hook when hitting a hazard
                if (caughtFishPtr)
                {
                    releaseFish();
                }
                catchScore = std::max(0, catchScore - 2);

                if (auto hh = dynamic_cast<HeavyHazard*>(hazards[i].get()))
                {
                    caughtHeavyHazardPtr = hazards[i];
                    caughtHeavyHazardPtr->setCaught(true);
                    
                    // Speed reduction
                    float reducedSpeed = rod.getMaxVelocity() * hh->getSlowFactor();
                    caughtHeavyHazardPtr->setSpeed(0, reducedSpeed * rod.getDirection()[1]);
                    caughtHeavyHazardPtr->setDirection(rod.getDirection());
                    
                    // Set rod velocity to match new speed if moving
                    if (rod.getDirection()[1] < 0) {
                        rod.castUpSpeed(reducedSpeed);
                    } else if (rod.getDirection()[1] > 0) {
                        rod.castDownSpeed(reducedSpeed);
                    }
                }
                else if (auto sh = dynamic_cast<ShockHazard*>(hazards[i].get()))
                {
                    hazards[i]->setActive(false);
                    hazards[i]->setPosition(-100, -100);
                    float minRod = Scale::scaleH(Game::ROD_MIN_LENGTH_PERCENT);
                    rod.setPosition(rod.getPosition()[0], minRod);
                    stunned = true;
                    stunEndTime = SDL_GetTicks() + 2000;
                }
                else
                {
                    hazards[i]->setActive(false);
                    hazards[i]->setPosition(-100, -100);
                }
                
            }
        }
    }
}

/**
 * @brief Handles logic when a fish is released from the rod
 */
void FishingLogic::releaseFish()
{   // stop vertical speed
    caughtFishPtr->setSpeed(calculateRandSpeed(caughtFishPtr), 0);
    caughtFishPtr->setCaught(false);
    float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);
    float rodH = Scale::scaleH(Game::ROD_RADIUSY_PERCENT);

    if (rod.getPosition()[1] + rodH/2 < surfacePond)
    {   // set fish out of view
        caughtFishPtr->setPosition(-100,-100);
        // deactivate fish
        caughtFishPtr->setActive(false);
        int points = 1;
        if(dynamic_cast<MediumFish*>(caughtFishPtr.get()) != nullptr)
        {
            points = 2;
        } else if(dynamic_cast<BigFish*>(caughtFishPtr.get()) != nullptr)
        {
            points = 3;
        }
        catchScore += points;
    }  else
    { // continue swimming
        // set fish past rod
        caughtFishPtr->setPosition(rod.getPosition() + rod.getDimens());
        caughtFishPtr->setSpeed(calculateRandSpeed(caughtFishPtr), 0);
        caughtFishPtr->update();
    }
    // rotate fish back to original orientation
    float oldH = caughtFishPtr->getDimens()[1];
    float oldW = caughtFishPtr->getDimens()[0];
    // have to split old h and w cuz we're swapping them
    caughtFishPtr->setDimens(oldH, oldW);

    // reset caughtFish ptr
    caughtFishPtr = nullptr;
}

//Method for when a heavy hazard is caught
void FishingLogic::releaseHeavyHazard()
{
    float surfacePond = Scale::scaleH(Visuals::SURFACE_POND_PERCENT);

    if (rod.getPosition()[1] <= surfacePond)
    {
        caughtHeavyHazardPtr->setActive(false);
        caughtHeavyHazardPtr->setCaught(false);
        caughtHeavyHazardPtr->setPosition(-100, -100);
        caughtHeavyHazardPtr.reset();
    }
}

/** 
 * @brief updater for fishing phase logic, updates logic state each tick
 */
void FishingLogic::update()
{
    if (!running) {
        return;
    }

    //Clear stun time if time is up
    Uint32 now = SDL_GetTicks();
    if (stunned && now >= stunEndTime) stunned = false;

    rod.updatePos();
    if (caughtFishPtr && caughtFishPtr->isCaught()) {
        caughtFishPtr->setPosition(rod.getPosition()[0], rod.getPosition()[1]);
        caughtFishPtr->update();
    }

    if (caughtHeavyHazardPtr && caughtHeavyHazardPtr->isCaught())
    {
        caughtHeavyHazardPtr->setPosition(rod.getPosition());
    }

    int currentTime = SDL_GetTicks();

    // Only check ever 5 frames since expensive
    if (currentTime - lastCheckTime > 80) {
        handleCatch();
        lastCheckTime = currentTime;
    }

    // Check if the round has actually started
    if (roundStartTime <= 0) { // Round not started yet, initialize start time
        resetRoundStartTime();
        return;
    }

    // Calculate time elapsed including paused time
    elapsedTime = currentTime - roundStartTime - totalPauseTime;
    
    // Debug output for timer issues
    static int lastDebugOutput = 0;
    if (currentTime - lastDebugOutput > 1000) { // Log once per second
        std::cout << "FishingLogic: Time remaining: " 
                  << (roundDuration - elapsedTime) / 1000 
                  << " seconds (elapsed: " << elapsedTime / 1000 
                  << ", total: " << roundDuration / 1000 
                  << ", pause: " << totalPauseTime / 1000 << ")" 
                  << std::endl;
        lastDebugOutput = currentTime;
    }
    
    if (elapsedTime >= roundDuration) {
        std::cout << "FishingLogic: Round time expired! elapsed=" 
                  << elapsedTime << ", duration=" << roundDuration << std::endl;
        running = false;
        return;
    }

    if (currentTime - lastSpawnTime > spawnInterval) { // Every 3ish seconds
        spawnNewFloaters();
        spawnHazards();
        lastSpawnTime = SDL_GetTicks();
    }

    for(int i = 0; i < allFish.size(); i++)
    { // update all floater movement w automation
        if (!(allFish[i]->isCaught()) && allFish[i]->isActive()) allFish[i]->update();

        // check if fish is no longer visible
        if (allFish[i]->isActive()) {
            // Check direction to determine correct oob
            if (allFish[i]->getDirection()[0] > 0) {
                // Fish moving right
                if (allFish[i]->getPosition()[0] >= Dimens::SCREEN_W) {
                    allFish[i]->setActive(false);
                }
            } else {
                // Fish moving left
                if (allFish[i]->getPosition()[0] + allFish[i]->getDimens()[0] <= 0) {
                    allFish[i]->setActive(false);
                }
            }
        }
    }

    for (int i = 0; i < hazards.size(); i++)
    { //Updates all hazard movement with automation
        if (hazards[i]->isActive())
        {
            hazards[i]->update();
        }

        //Checks to see if hazard is no longer visible
        if (hazards[i]->isActive()) {
            // Check direction to determine correct oob
            if (hazards[i]->getDirection()[0] > 0) {
                // Hazard moving right
                if (hazards[i]->getPosition()[0] >= Dimens::SCREEN_W) {
                    hazards[i]->setActive(false);
                }
            } else {
                // Hazard moving left
                if (hazards[i]->getPosition()[0] + hazards[i]->getDimens()[0] <= 0) {
                    hazards[i]->setActive(false);
                }
            }
        }
    }
}

void FishingLogic::setCallback(std::function<void(const std::string&)> callback) {
    notificationCallback = callback;
}

void FishingLogic::setPaused(bool newState) {
    if (newState == paused) {
        return; // No change needed
    }
    
    if (newState) {
        // Entering pause state
        pauseStartTime = SDL_GetTicks();
        paused = true;
    } else {
        // Exiting pause state
        if (pauseStartTime > 0) {
            totalPauseTime += (SDL_GetTicks() - pauseStartTime);
            pauseStartTime = 0;
        }
        paused = false;
    }
}

void FishingLogic::pauseGame() {
    setPaused(true);
    if (notificationCallback) {
        notificationCallback("PauseFishing");
    }
}

void FishingLogic::resumeGame() {
    setPaused(false);
    if (notificationCallback) {
        notificationCallback("ResumeFishing");
    }
}

void FishingLogic::quitToMenu() {
    if (notificationCallback) {
        notificationCallback("ReturnMenu");
    }
}

/**
 * @brief Resets the round start time to the current time
 * @details This should be called when the fishing phase actually begins,
 *          not when the FishingLogic object is created >:(
 */
void FishingLogic::resetRoundStartTime() {
    roundStartTime = SDL_GetTicks();
    lastSpawnTime = roundStartTime;
    elapsedTime = 0;
    totalPauseTime = 0;
    pauseStartTime = 0;
    
    std::cout << "FishingLogic: Round start time reset to " << roundStartTime << std::endl;
}