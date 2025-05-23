#include <SDL.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include "process_manager.hpp"
#include "game_constants.hpp"

int main(int argc, char* argv[]) {
    // Set default resolution
    float width = Dimens::SCREEN_W_MED;  
    float height = Dimens::SCREEN_H_MED;
    
    // First check for config file
    std::ifstream configFile("game_config.txt");
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            try {
                size_t equalsPos = line.find('=');
                if (equalsPos != std::string::npos && equalsPos + 1 < line.length()) {
                    std::string key = line.substr(0, equalsPos);
                    std::string value = line.substr(equalsPos + 1);
                    
                    // Debug output
                    std::cout << "Config: Key='" << key << "', Value='" << value << "'" << std::endl;
                    
                    if (key == "resolution_width") {
                        width = std::stof(value);
                        std::cout << "Loaded width: " << width << std::endl;
                    } else if (key == "resolution_height") {
                        height = std::stof(value);
                        std::cout << "Loaded height: " << height << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing config value: " << e.what() << std::endl;
                // Continue with default values
            }
        }
        configFile.close();
    }
    
    // Then check command line args which override config file
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--width" && i + 1 < argc) {
            width = std::stoi(argv[i + 1]);
            i++;
        }
        else if (std::string(argv[i]) == "--height" && i + 1 < argc) {
            height = std::stoi(argv[i + 1]);
            i++;
        }
    }

    std::cout << "Launching with resolution: " + std::to_string(width) + "x" + std::to_string(height) << std::endl;
    
    // Set resolution 
    Dimens::SCREEN_W = width;
    Dimens::SCREEN_H = height;
    Dimens::CENTER_X = width/2;
    Dimens::CENTER_Y = height/2;
    
    // Create and run the game
    try {
        ProcessManager gameManager;
        gameManager.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}