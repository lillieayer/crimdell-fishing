// player_data.hpp
#ifndef PLAYER_DATA_HPP
#define PLAYER_DATA_HPP
#include <string>
#include <fstream>
#include "game_constants.hpp"

class PlayerData {
public:
    PlayerData();
    ~PlayerData() = default;
    
    // Cash and day tracking
    int getCash() const { return cash; }
    void setCash(int amount) { cash = amount; }
    void addCash(int amount) { cash += amount; }
    void spendCash(int amount) { if (cash >= amount) cash -= amount; }
    int getRent() const { return rent; }
    void setRent(int amount) { rent = amount; }
    
    int getDayCount() const { return dayCount; }
    void incrementDay() { dayCount++; }
    void resetDays() { dayCount = 1; }
    
    // Loan functions
    int getLoanAmount() const { return loan; }
    void setLoanAmount(int amount) { loan = amount; }
    void payLoan(int amount) {
        if (cash >= amount && loan >= amount) {
            loan -= amount;
            cash -= amount;
        }
    }
    bool isLoanPaid() const { return loan <= 0; }
    
    // Upgrades
    int getRodLevel() const { return rodLevel; }
    int getClockLevel() const { return clockLevel; }
    int getRegisterLevel() const { return registerLevel; }
    float getCashMultiplier() const {return 1.0f + (registerLevel * Game::REGISTER_UPGRADE_VALUE); }
    int getLedgerLevel() const { return ledgerLevel; }
    int getLedgerIncome() const { return ledgerIncome; }
    void applyLedgerIncome() { cash += ledgerIncome; }
    void upgradeRod() { rodLevel++;  rodSpeedMultiplier += Game::ROD_SPEED_UPGRADE_VALUE; }
    float getRodSpeedMultiplier() const { return rodSpeedMultiplier; }
    int getMaxFishLevel() const { 
        if (rodLevel >= 3) return 3;
        if (rodLevel >= 1) return 2;
        return 1;
    }
    void upgradeClock() { clockLevel++; }
    void upgradeRegister() { registerLevel++; }
    void upgradeLedger() { ledgerLevel++; ledgerIncome += Game::LEDGER_INCOME;}
    
    // Game stats
    int getTotalFishCaught() const { return totalFishCaught; }
    void addFishCaught(int count) { totalFishCaught += count; }
    bool hasSeenTutorial() const { return seenTutorial; }
    void setTutorialSeen() { seenTutorial = true; }
    void resetTutorialSeen() { seenTutorial = false; }
    
    // Save/Load methods
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    
private:
    // Basic player stats
    int cash = Game::STARTING_CASH;
    int dayCount = 1;
    int totalFishCaught = 0;
    int rent = Game::STARTING_RENT;
    int loan = Game::STARTING_LOAN;
    bool seenTutorial = false;
    
    // Upgrades
    int rodLevel = 0;
    float rodSpeedMultiplier = 1.0f;
    int clockLevel = 0;
    int registerLevel = 0;
    int ledgerLevel = 0;
    int ledgerIncome = 0;
};

#endif