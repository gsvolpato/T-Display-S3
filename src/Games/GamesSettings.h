#ifndef GAMES_SETTINGS_H
#define GAMES_SETTINGS_H

#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* GamesMenuItems[];
void handleGamesMenuSelection(int selectedOption);
void GamesMenu();

#endif // GAMES_SETTINGS_H
