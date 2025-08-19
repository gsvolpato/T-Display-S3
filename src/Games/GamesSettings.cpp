#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "GamesSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"

extern TFT_eSPI tft;

const char* GamesMenuItems[] = {
    "20 Questions",
    "Snake",
    "Pong",
    "Tetris",
    "Memory Game",
    "Simon Says",
    nullptr
};

void handleGamesMenuSelection(int selectedOption) {
    uint8_t rotation = tft.getRotation();
    int textX = (rotation == 1 || rotation == 3) ? SCREEN_WIDTH/2 : SCREEN_WIDTH/4;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("20 Questions", textX, SCREEN_HEIGHT/2);
            Serial.println("20 Questions selected");
            delay(1500);
            break;
        case 1:
            staticItems();
            tft.drawString("Snake", textX, SCREEN_HEIGHT/2);
            Serial.println("Snake selected");
            delay(1500);
            break;
        case 2:
            staticItems();
            tft.drawString("Pong", textX, SCREEN_HEIGHT/2);
            Serial.println("Pong selected");
            delay(1500);
            break;
        case 3:
            staticItems();
            tft.drawString("Tetris", textX, SCREEN_HEIGHT/2);
            Serial.println("Tetris selected");
            delay(1500);
            break;
        case 4:
            staticItems();
            tft.drawString("Memory Game", textX, SCREEN_HEIGHT/2);
            Serial.println("Memory Game selected");
            delay(1500);
            break;
        case 5:
            staticItems();
            tft.drawString("Simon Says", textX, SCREEN_HEIGHT/2);
            Serial.println("Simon Says selected");
            delay(1500);
            break;
        default:
            staticItems();
            tft.drawString("Invalid selection", textX, SCREEN_HEIGHT/2);
            Serial.println("Invalid Games menu selection");
            delay(1500);
            break;
    }
}

void GamesMenu() {
    Serial.println("Parsed Games Menu Items:");
    for (int i = 0; GamesMenuItems[i] != nullptr; i++) {
        Serial.println(GamesMenuItems[i]);
    }
    
    uint8_t rotation = tft.getRotation();
    const char* headerText = (rotation == 1 || rotation == 3) ? "Games" : "Games";
    
    userInterface(headerText, GamesMenuItems, handleGamesMenuSelection);
}
