#ifndef GAME_NOTIFIER_HPP
#define GAME_NOTIFIER_HPP

#include <functional>
#include <string>
// Menu class

class GameNotifier {
public:
    GameNotifier();
    ~GameNotifier() = default;

    void setCallback(std::function<void(const std::string&)> callback);

    void goFishing();
    void startStore();
    void loadGame();
    void returnMenu();
    void exitGame();
    void saveGame();
    void notifyLoanPaid();
    void changeResolution(int width, int height);
    void showTutorial();

private:
    std::function<void(const std::string&)> notificationCallback;
};

#endif