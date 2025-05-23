#include "player_data.hpp"
#include <iostream>
#include <fstream>
#include <string>

PlayerData::PlayerData() {
    // Default initialization is in the header
}

bool PlayerData::saveGame(const std::string& filename) const {
    try {
        std::ofstream saveFile(filename);
        if (!saveFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        // Save player stats
        saveFile << cash << std::endl;
        saveFile << dayCount << std::endl;
        saveFile << totalFishCaught << std::endl;
        saveFile << rent << std::endl;
        saveFile << loan << std::endl;
        
        // Save upgrade levels
        saveFile << rodLevel << std::endl;
        saveFile << rodSpeedMultiplier << std::endl;
        saveFile << clockLevel << std::endl;
        saveFile << registerLevel << std::endl;
        saveFile << ledgerLevel << std::endl;
        saveFile << ledgerIncome << std::endl;
        
        // Save tutorial flag
        saveFile << (seenTutorial ? 1 : 0) << std::endl;
        
        saveFile.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
        return false;
    }
}

bool PlayerData::loadGame(const std::string& filename) {
    try {
        std::ifstream loadFile(filename);
        if (!loadFile.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << std::endl;
            return false;
        }

        // Load player stats
        loadFile >> cash;
        loadFile >> dayCount;
        loadFile >> totalFishCaught;
        loadFile >> rent;
        loadFile >> loan;
        
        // Load upgrade levels
        loadFile >> rodLevel;
        loadFile >> rodSpeedMultiplier;
        loadFile >> clockLevel;
        loadFile >> registerLevel;
        loadFile >> ledgerLevel;
        loadFile >> ledgerIncome;
        
        // Load tutorial flag
        int tutorialSeen = 0;
        loadFile >> tutorialSeen;
        seenTutorial = (tutorialSeen == 1);
        
        loadFile.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        return false;
    }
}