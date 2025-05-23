// Holds game constants
#ifndef GAME_CONSTANTS_HPP
#define GAME_CONSTANTS_HPP

#include <string>  // For resource filepaths

// Window Dimensions
namespace Dimens 
{
    extern float SCREEN_H; // External so can be changed persistently by main
    extern float SCREEN_W;
    extern float CENTER_X;
    extern float CENTER_Y;

    static const float SCREEN_H_LARGE = 1232.0f;
    static const float SCREEN_W_LARGE = 1685.0f;
    static const float SCREEN_H_MED = 924.0f;
    static const float SCREEN_W_MED = 1263.0f;
    static const float SCREEN_H_SMALL = 616.0f;
    static const float SCREEN_W_SMALL = 842.0f;
}

// Mechanics-Related Values
namespace Game
{
    const int ROUND_DURATION = 30000; // 30 seconds in ms

    // Rod Values
    const float ROD_MAX_LENGTH_PERCENT = 0.99f;
    const float ROD_MIN_LENGTH_PERCENT = 0.195f;
    const float ROD_RADIUSX_PERCENT = 0.005f;
    const float ROD_RADIUSY_PERCENT = 0.02f;
    const float ROD_SPEED = 2;

    // Floater Values
    const float MAX_FISH_SPEED = 4;
    const float MAX_MEDIUM_FISH_SPEED = 5;
    const float MAX_BIG_FISH_SPEED = 6;
    const float MAX_HAZARD_SPEED = 2;

    // Animation Values
    const int FISH_ANIMATION_DELAY = 400; // milliseconds between fish animation frames
    const int BACKGROUND_ANIMATION_DELAY = 1600; // milliseconds between background animation frames

    // Store Balance Values
    const int ROD_PRICE = 20;
    const int CLOCK_PRICE = 10;
    const int LEDGER_PRICE = 15;
    const int REGISTER_PRICE = 10;

    const int LOAN_PAYMENT = 25;
    const int BASE_SCORE_MULTIPLIER = 2;
    const float RENT_INCREASE = 1.05;
    const float REGISTER_UPGRADE_VALUE = 0.1;
    const float ROD_SPEED_UPGRADE_VALUE = 0.2;
    const int LEDGER_INCOME = 2;

    const int STARTING_CASH = 100;
    const int STARTING_LOAN = 400;
    const int STARTING_RENT = 10;

    const float HEAVY_HAZARD_SLOW_FACTOR = 0.3f;
}

// Level spawn values
namespace Level {
    namespace Day1 {
        const int SMALL_FISH = 4;
        const int MEDIUM_FISH = 0;
        const int BIG_FISH = 0;

        const int NORMAL_HAZARDS = 2;
        const int HEAVY_HAZARDS = 0;
        const int SHOCK_HAZARDS = 0;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.0f;
    }
    
    namespace Day2 {
        const int SMALL_FISH = 5;
        const int MEDIUM_FISH = 0;
        const int BIG_FISH = 0;

        const int NORMAL_HAZARDS = 2;
        const int HEAVY_HAZARDS = 0;
        const int SHOCK_HAZARDS = 0;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.1f;
    }
    
    namespace Day3 {
        const int SMALL_FISH = 5;
        const int MEDIUM_FISH = 2;
        const int BIG_FISH = 0;

        const int NORMAL_HAZARDS = 2;
        const int HEAVY_HAZARDS = 2;
        const int SHOCK_HAZARDS = 0;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.2f;
    }
    
    namespace Day4 {
        const int SMALL_FISH = 5;
        const int MEDIUM_FISH = 3;
        const int BIG_FISH = 0;

        const int NORMAL_HAZARDS = 2;
        const int HEAVY_HAZARDS = 2;
        const int SHOCK_HAZARDS = 0;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.3f;
    }
    
    namespace Day5 {
        const int SMALL_FISH = 6;
        const int MEDIUM_FISH = 4;
        const int BIG_FISH = 0;

        const int NORMAL_HAZARDS = 3;
        const int HEAVY_HAZARDS = 2;
        const int SHOCK_HAZARDS = 0;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.4f;
    }
    
    namespace Day6 {
        const int SMALL_FISH = 6;
        const int MEDIUM_FISH = 4;
        const int BIG_FISH = 2;

        const int NORMAL_HAZARDS = 3;
        const int HEAVY_HAZARDS = 2;
        const int SHOCK_HAZARDS = 2;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.5f;
    }
    
    namespace Day7 {
        const int SMALL_FISH = 7;
        const int MEDIUM_FISH = 4;
        const int BIG_FISH = 3;

        const int NORMAL_HAZARDS = 3;
        const int HEAVY_HAZARDS = 3;
        const int SHOCK_HAZARDS = 3;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.6f;
    }
    
    namespace Day8 {
        const int SMALL_FISH = 8;
        const int MEDIUM_FISH = 5;
        const int BIG_FISH = 4;

        const int NORMAL_HAZARDS = 4;
        const int HEAVY_HAZARDS = 3;
        const int SHOCK_HAZARDS = 3;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.7f;
    }
    
    namespace Day9 {
        const int SMALL_FISH = 9;
        const int MEDIUM_FISH = 6;
        const int BIG_FISH = 5;

        const int NORMAL_HAZARDS = 4;
        const int HEAVY_HAZARDS = 4;
        const int SHOCK_HAZARDS = 3;
        
        const float HAZARD_SPEED_MULTIPLIER = 1.8f;
    }
    
    namespace Day10 {
        const int SMALL_FISH = 10;
        const int MEDIUM_FISH = 7;
        const int BIG_FISH = 6;

        const int NORMAL_HAZARDS = 5;
        const int HEAVY_HAZARDS = 4;
        const int SHOCK_HAZARDS = 4;
        
        const float HAZARD_SPEED_MULTIPLIER = 2.0f;
    }
}

// Sprite Values
namespace Visuals
{
    // Fish Values
    const float FISH_HEIGHT_PERCENT = 0.035f;
    const float FISH_WIDTH_PERCENT = 0.065f;
    const float MEDIUM_FISH_HEIGHT_PERCENT = 0.05f;
    const float MEDIUM_FISH_WIDTH_PERCENT = 0.08f;
    const float BIG_FISH_HEIGHT_PERCENT = 0.06f;
    const float BIG_FISH_WIDTH_PERCENT = 0.1f;

    // Hazard Values
    const float HAZARD_HEIGHT_PERCENT = 0.04f;
    const float HAZARD_WIDTH_PERCENT = 0.1f;
    const float SHOCK_HAZARD_HEIGHT_PERCENT = 0.15f;
    const float SHOCK_HAZARD_WIDTH_PERCENT = 0.07f;
    const float HEAVY_HAZARD_HEIGHT_PERCENT = 0.1f;
    const float HEAVY_HAZARD_WIDTH_PERCENT = 0.11f;

    // Fonts - restored old values and added new for store/popups
    const float FONT_SIZE = 24;
    const float HOVER_FONT_SIZE = 16;
    const float TITLE_FONT_SIZE = 30; 
    const float STATS_HEADER_FONT_SIZE = 20;
    const float STATS_TEXT_FONT_SIZE = 15;
    const float STORE_BUTTON_FONT_SIZE = 20;
    const float POPUP_TITLE_FONT_SIZE = 26;
    const float POPUP_TEXT_FONT_SIZE = 14;

    // Misc
    const float SURFACE_POND_PERCENT = 0.54f; // Waterline
}

// Color Presets
namespace Colors 
{
    namespace DarkBlue
    {
        static const int R = 0;
        static const int G = 40;
        static const int B = 191;
        static const int A = 1;
    }

    namespace DarkGreen
    {
        static const int R = 0;
        static const int G = 100;
        static const int B = 0;
        static const int A = 255;
    }
        
    namespace Gray 
    {
        static const int R = 191;
        static const int G = 191;
        static const int B = 191;
        static const int A = 250;
    }

    namespace LightGray
    {
        static const int R = 211;
        static const int G = 211;
        static const int B = 211;
        static const int A = 255;
    }

    namespace Orange 
    {
        static const int R = 255;
        static const int G = 148;
        static const int B = 112;
        static const int A = 250;
    }

    namespace Red
    {
        static const int R = 255;
        static const int G = 0;
        static const int B = 0;
        static const int A = 250;
    }

    namespace White 
    {
        static const int R = 255;
        static const int G = 255;
        static const int B = 255;
        static const int A = 250;
    }

    namespace Black 
    {
        static const int R = 0;
        static const int G = 0;
        static const int B = 0;
        static const int A = 250;
        static const int ATrans = 200;
    }
    
    // UI Colors
    namespace Tan
    {
        static const int R = 210;
        static const int G = 180;
        static const int B = 140;
        static const int A = 255;
        static const int ATrans = 240; // Semi-transparent alpha val
    }

    namespace TanHover
    {
        static const int R = 222;
        static const int G = 184;
        static const int B = 135;
        static const int A = 255;
    }
    
    namespace Brown
    {
        static const int R = 139;
        static const int G = 69;
        static const int B = 19;
        static const int A = 255;
    }
    
    namespace Wheat
    {
        static const int R = 245;
        static const int G = 222;
        static const int B = 179;
        static const int A = 255;
    }

    namespace DarkBrown
    {
        static const int R = 70;
        static const int G = 35;
        static const int B = 0;
        static const int A = 255;
    }
}

// Sprite Loading
namespace Resource
{
    // Resource Paths
    const std::string RESOURCE_PATH = "../resource/";

    // Fish animation sprites
    const std::string FISH_SMALL = RESOURCE_PATH + "fish_small.png";
    const std::string FISH_SMALL_ANIM1 = RESOURCE_PATH + "fish_small_animated.png";
    const std::string FISH_SMALL_ANIM2 = RESOURCE_PATH + "fish_small_animated2.png";
    
    const std::string FISH_MEDIUM = RESOURCE_PATH + "fish_medium.png";
    const std::string FISH_MEDIUM_ANIM1 = RESOURCE_PATH + "fish_medium_animated.png";
    const std::string FISH_MEDIUM_ANIM2 = RESOURCE_PATH + "fish_medium_animated2.png";
    
    const std::string FISH_BIG = RESOURCE_PATH + "fish_big.png";
    const std::string FISH_BIG_ANIM1 = RESOURCE_PATH + "fish_big_animated.png";
    const std::string FISH_BIG_ANIM2 = RESOURCE_PATH + "fish_big_animated2.png";
    
    const std::string HAZARD_HEAVY = RESOURCE_PATH + "hazard_cannon.png";
    const std::string HAZARD_SHOCK = RESOURCE_PATH + "hazard_skelly.png";
    const std::string HAZARD_BARRACUDA = RESOURCE_PATH + "hazard_barracuda.png"; // unused, remove?
    const std::string HAZARD_BARRACUDA_ANIM1 = RESOURCE_PATH + "hazard_barracuda_animated.png";
    const std::string HAZARD_BARRACUDA_ANIM2 = RESOURCE_PATH + "hazard_barracuda_animated2.png";


    // Fonts - added alternative font paths
    const std::string ARIAL = RESOURCE_PATH + "Arial.ttf";
    const std::string WATERLILY = RESOURCE_PATH + "Waterlily Script.ttf";

    // Background animation sprites
    const std::string BG_MENU = RESOURCE_PATH + "bg_menu.png";
    const std::string BG_MENU_ANIM1 = RESOURCE_PATH + "bg_menu_animated.png";
    const std::string BG_MENU_ANIM2 = RESOURCE_PATH + "bg_menu_animated2.png";
    
    const std::string BG_FISHING = RESOURCE_PATH + "bg_fish.png";
    const std::string BG_FISHING_ANIM1 = RESOURCE_PATH + "bg_fish_animated.png";
    const std::string BG_FISHING_ANIM2 = RESOURCE_PATH + "bg_fish_animated2.png";
    
    const std::string BG_STORE = RESOURCE_PATH + "bg_store.png";
    const std::string BG_STORE_ANIM1 = RESOURCE_PATH + "bg_store_animated.png";
    const std::string BG_STORE_ANIM2 = RESOURCE_PATH + "bg_store_animated2.png";

    // Resource IDs
    // Floater static IDs - fish deprecated
    const std::string FISH_SMALL_ID = "fish_small";
    const std::string FISH_MEDIUM_ID = "fish_medium";
    const std::string FISH_BIG_ID = "fish_big";
    const std::string HAZARD_HEAVY_ID = "hazard_cannon";
    const std::string HAZARD_SHOCK_ID = "hazard_skelly";
    const std::string HAZARD_BARRACUDA_ID = "hazard_barracuda";

    // Fish animation IDs
    const std::string FISH_SMALL_ANIM1_ID = "fish_small_anim1";
    const std::string FISH_SMALL_ANIM2_ID = "fish_small_anim2";
    const std::string FISH_MEDIUM_ANIM1_ID = "fish_medium_anim1";
    const std::string FISH_MEDIUM_ANIM2_ID = "fish_medium_anim2";
    const std::string FISH_BIG_ANIM1_ID = "fish_big_anim1";
    const std::string FISH_BIG_ANIM2_ID = "fish_big_anim2";
    const std::string HAZARD_BARRACUDA_ANIM1_ID = "hazard_barracuda_anim1";
    const std::string HAZARD_BARRACUDA_ANIM2_ID = "hazard_barracuda_anim2";

    // Background static IDs - Deprecated
    const std::string BG_MENU_ID = "bg_menu";
    const std::string BG_FISHING_ID = "bg_fishing";
    const std::string BG_STORE_ID = "bg_store";

    // Background animation IDs
    const std::string BG_MENU_ANIM1_ID = "bg_menu_anim1";
    const std::string BG_MENU_ANIM2_ID = "bg_menu_anim2";
    const std::string BG_FISHING_ANIM1_ID = "bg_fishing_anim1";
    const std::string BG_FISHING_ANIM2_ID = "bg_fishing_anim2";
    const std::string BG_STORE_ANIM1_ID = "bg_store_anim1";
    const std::string BG_STORE_ANIM2_ID = "bg_store_anim2";
}

#endif