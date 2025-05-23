#include <iostream>
#include "game_notifier.hpp"

GameNotifier::GameNotifier() : notificationCallback(nullptr) {
    // Initialize without state tracking
}


void GameNotifier::setCallback(std::function<void(const std::string&)> callback) {
    notificationCallback = callback;
}

void GameNotifier::goFishing() {
    std::cout << "GameNotifier::goFishing() called" << std::endl;
    if (notificationCallback) {
        std::cout << "GameNotifier: Calling notification callback with 'GoFishing'" << std::endl;
        notificationCallback("GoFishing");
    } else {
        std::cerr << "ERROR: GameNotifier::goFishing - notificationCallback is null!" << std::endl;
    }
}

void GameNotifier::startStore() {
    std::cout << "GameNotifier::startStore() called" << std::endl;
    if (notificationCallback) {
        std::cout << "GameNotifier: Calling notification callback with 'StartStore'" << std::endl;
        notificationCallback("StartStore");
    } else {
        std::cerr << "ERROR: GameNotifier::startStore - notificationCallback is null!" << std::endl;
    }
}

void GameNotifier::exitGame() {
    std::cout << "GameNotifier::exitGame() called" << std::endl;
    if (notificationCallback) {
        std::cout << "GameNotifier: Calling notification callback with 'ExitGame'" << std::endl;
        notificationCallback("ExitGame");
    } else {
        std::cerr << "ERROR: GameNotifier::exitGame - notificationCallback is null!" << std::endl;
    }
}

void GameNotifier::loadGame() {
    std::cout << "GameNotifier::loadGame() called" << std::endl;
    if (notificationCallback) {
        std::cout << "GameNotifier: Calling notification callback with 'LoadGame'" << std::endl;
        notificationCallback("LoadGame");
    } else {
        std::cerr << "ERROR: GameNotifier::loadGame - notificationCallback is null!" << std::endl;
    }
}

void GameNotifier::returnMenu() {
    if (notificationCallback) {
        notificationCallback("ReturnMenu");
    }
}


void GameNotifier::saveGame() {
    if (notificationCallback) {
        notificationCallback("SaveGame");
    }
}

void GameNotifier::notifyLoanPaid() {
    if (notificationCallback) {
        notificationCallback("LoanPaid");
    }
}

void GameNotifier::changeResolution(int width, int height) {
    if (notificationCallback) {
        std::string notification = "ChangeResolution:" + std::to_string(width) + ":" + std::to_string(height);
        notificationCallback(notification);
    }
}

void GameNotifier::showTutorial() {
    std::cout << "GameNotifier::showTutorial() called" << std::endl;
    if (notificationCallback) {
        std::cout << "GameNotifier: Calling notification callback with 'ShowTutorial'" << std::endl;
        notificationCallback("ShowTutorial");
    } else {
        std::cerr << "ERROR: GameNotifier::showTutorial - notificationCallback is null!" << std::endl;
    }
}