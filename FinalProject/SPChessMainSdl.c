/*
 * SPChessMainSdl.c
 *
 *  Created on: 10 March 2018
 *      Author: user
 */
#include "SPChessMainSdl.h"
#include <stdio.h>
#include <string.h>
#include "Button.h"
#include "SPChessGameSdl.h"
#include "SPLoadSaveGameSdl.h"
#include "SPMainAux.h"

#define GET_MODE_WINDOW 2
#define GET_DIFFICULTY_WINDOW 3
#define GET_COLOR_WINDOW 4


#define MM_NEW_GAME 0
#define MM_LOAD_GAME 1
#define MM_QUIT 2
#define MAIN_MENU_N_WIDGETS 3

#define GM_MESSAGE 0
#define GM_1PLAYER 1
#define GM_2PLAYER 2
#define GM_BACK_TO_MM 3
#define GET_MODE_N_WIDGETS 4

#define GD_MESSAGE 0
#define GD_BACK 6
#define GD_BACK_TO_MM 7
#define GET_DIFFICULTY_N_WIDGETS 8

#define GC_BLACK 0
#define GC_WHITE 1
#define GC_MESSAGE 2
#define GC_BACK 3
#define GC_BACK_TO_MM 4
#define GET_COLOR_N_WIDGETS 5

/**
 *  Creates the main menu. this is the only window shown when initialized
 *  @return		NULL, if an error occurred.
 *  			Otherwise An instant of a screen with content corresponding to the main menu .
 */
Screen* spGameCreateMainMenuWindow() {
	// Allocate screen struct
	Screen* mainMenuWindow = createScreen(450, 400, "Main Menu", MAIN_MENU_N_WIDGETS, SHOWN, NONE, GET_MODE_WINDOW,
										  spDrawScreen);

	if (!mainMenuWindow) {
		return NULL;
	}

	SDL_Renderer* renderer = mainMenuWindow->renderer;

	// Allocate screen's widgets
	mainMenuWindow->widgets[MM_NEW_GAME] = createButton(renderer, "pics/new_game.bmp", spOpenNextWindow, 100, 100, 250,
														50, SHOWN);
	mainMenuWindow->widgets[MM_LOAD_GAME] = createButton(renderer, "pics/load_game.bmp", spOpenNextWindow, 100, 175,
														 250, 50, SHOWN);
	mainMenuWindow->widgets[MM_QUIT] = createButton(renderer, "pics/quit_mm.bmp", spQuit, 100, 250, 250, 50, SHOWN);

	// check if widgets were created successfully
	int success = spCheckWidgetsInit(mainMenuWindow);
	if (!success) {
		spDestroyScreen(mainMenuWindow);
		return NULL;
	}

	return mainMenuWindow;
}

/**
*  Creates Creates the get mode window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get mode window.
*/
Screen* spGameCreateGetModeWindow() {
	// Allocate screen struct
	Screen* getModeWindow = createScreen(600, 400, "Select Mode:", GET_MODE_N_WIDGETS, HIDE, MAIN_MENU_WINDOW,
										 GET_DIFFICULTY_WINDOW, spDrawScreen);
	if (!getModeWindow) {
		return NULL;
	}

	SDL_Renderer* renderer = getModeWindow->renderer;

	// Allocate screen's widgets
	getModeWindow->widgets[GM_MESSAGE] = createLabel(renderer, "pics/gm_select_game_mode.bmp", 100, 62, 400, 50, SHOWN);
	getModeWindow->widgets[GM_1PLAYER] = createButton(renderer, "pics/gm_1_player.bmp", spOpenNextWindow, 175, 137, 250,
													  50, SHOWN);
	getModeWindow->widgets[GM_2PLAYER] = createButton(renderer, "pics/gm_2_player.bmp", spOpenNextWindow, 175, 212, 250,
													  50, SHOWN);
	getModeWindow->widgets[GM_BACK_TO_MM] = createButton(renderer, "pics/back_to_main_menu.bmp", spOpenMainMenuWindow,
														 300, 287, 250, 50, SHOWN);

	// check if widgets were created successfully
	int success = spCheckWidgetsInit(getModeWindow);
	if (!success) {
		spDestroyScreen(getModeWindow);
		return NULL;
	}

	return getModeWindow;
}

/**
*  Creates Creates the get difficulty window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get difficulty window.
*/
Screen* spCreateGetDifficultyWindow() {
	// Allocate screen struct
	Screen* getDifficultyWindow = createScreen(600, 490, "Select Difficulty:", GET_DIFFICULTY_N_WIDGETS, HIDE,
											   GET_MODE_WINDOW, GET_COLOR_WINDOW, spDrawScreen);
	if (!getDifficultyWindow) {
		return NULL;
	}

	SDL_Renderer* renderer = getDifficultyWindow->renderer;

	// Allocate screen's widgets
	getDifficultyWindow->widgets[GD_MESSAGE] = createLabel(renderer, "pics/gd_select_game_difficulty.bmp", 100, 25, 400,
														   50, SHOWN);

	char * difficulties[5] = { "pics/gd_amateur.bmp",
							   "pics/gd_easy.bmp",
							   "pics/gd_moderate.bmp",
							   "pics/gd_hard.bmp",
							   "pics/gd_expert.bmp" };
	int i;
	for(i = 0; i < 5; i++) {
		getDifficultyWindow->widgets[i + 1] = createButton(renderer, difficulties[i], spOpenNextWindow,
														   175, 100 + i * 60, 250, 50, SHOWN);
	}
	getDifficultyWindow->widgets[GD_BACK] = createButton(renderer, "pics/back.bmp", spOpenPreviousWindow, 50, 415, 250,
														 50, SHOWN);
	getDifficultyWindow->widgets[GD_BACK_TO_MM] = createButton(renderer, "pics/back_to_main_menu.bmp",
															   spOpenMainMenuWindow, 300, 415, 250, 50, SHOWN);

	// check if widgets were created successfully
	int success = spCheckWidgetsInit(getDifficultyWindow);
	if (!success) {
		spDestroyScreen(getDifficultyWindow);
		return NULL;
	}

	return getDifficultyWindow;
}

/**
*  Creates Creates the get color window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get color window.
*/
Screen* spCreateGetColorWindow() {
	// Allocate screen struct
	Screen* getColorWindow = createScreen(600, 400, "Select Color:", GET_COLOR_N_WIDGETS, HIDE, GET_DIFFICULTY_WINDOW,
										  GAME_SCREEN, spDrawScreen);
	if (getColorWindow == NULL){
		return NULL;
	}

	SDL_Renderer* renderer = getColorWindow->renderer;

	// Allocate screen's widgets
	getColorWindow->widgets[GC_MESSAGE] = createLabel(renderer, "pics/gc_select_user_color.bmp", 100, 62, 400, 50,
													  SHOWN);
	getColorWindow->widgets[GC_WHITE] = createButton(renderer, "pics/gc_white.bmp", spOpenNextWindow, 175, 137, 250, 50,
													 SHOWN);
	getColorWindow->widgets[GC_BLACK] = createButton(renderer, "pics/gc_black.bmp", spOpenNextWindow, 175, 212, 250, 50,
													 SHOWN);
	getColorWindow->widgets[GC_BACK] = createButton(renderer, "pics/back.bmp", spOpenPreviousWindow, 50, 287, 250, 50,
													SHOWN);
	getColorWindow->widgets[GC_BACK_TO_MM] = createButton(renderer, "pics/back_to_main_menu.bmp", spOpenMainMenuWindow,
														  300, 287, 250, 50, SHOWN);

	// check if widgets were created successfully
	int success = spCheckWidgetsInit(getColorWindow);
	if (!success) {
		spDestroyScreen(getColorWindow);
		return NULL;
	}

	return getColorWindow;
}

/**
*  Draws the widgets contained in a screen (this function is used by the other drawing functions).
*  @param screen - pointer to a screen.
*/
void spDrawWidgets(Screen* screen) {
	if (!screen) {
		return;
	}

	Widget* widget;
	int i;
	for (i = 0; i < screen->widgetsSize; i++) {
		widget = screen->widgets[i];
		if (widget && widget->shown) {
			widget->draw(widget, screen->renderer);
		}
	}
}

int spGameCreateScreens(Screen** screens) {
	if (!screens) {
		return 0;
	}

	screens[GAME_SCREEN] = spGameCreateGameScreen();
	if (!screens[GAME_SCREEN]) {
		return 0;
	}

	screens[MAIN_MENU_WINDOW] = spGameCreateMainMenuWindow();
	if (!screens[MAIN_MENU_WINDOW]) {
		return 0;
	}

	screens[GET_MODE_WINDOW] = spGameCreateGetModeWindow();
	if (!screens[GET_MODE_WINDOW]) {
		return 0;
	}

	screens[GET_DIFFICULTY_WINDOW] = spCreateGetDifficultyWindow();
	if (!screens[GET_DIFFICULTY_WINDOW]) {
		return 0;
	}

	screens[GET_COLOR_WINDOW] = spCreateGetColorWindow();
	if (!screens[GET_COLOR_WINDOW]) {
		return 0;
	}

	screens[LOAD_GAME_WINDOW] = spCreateLoadSaveGameWindow(LOAD_GAME_WINDOW);
	if (!screens[LOAD_GAME_WINDOW]) {
		return 0;
	}

	screens[SAVE_GAME_WINDOW] = spCreateLoadSaveGameWindow(SAVE_GAME_WINDOW);
	if (!screens[SAVE_GAME_WINDOW]) {
		return 0;
	}

	spUpdateLoadSaveSlots(screens); // need to know which slots are used when initializing

	return 1;
}

int spCheckWidgetsInit(Screen* screen) {
	int i;
	for (i = 0; i < screen->widgetsSize; i++) {
		if (!screen->widgets[i]) {
			printf("ERROR: could not create widget. Quitting..\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									 "ERROR: could not create widget. Quitting..\n", NULL);
			return 0;
		}
	}

	return 1;
}

void spDrawScreen(Screen* screen, int screenIndex) {
	if (!screen) {
		printf("ERROR: unable to draw, screen resources lost\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
								 "ERROR: unable to draw, screen resources lost", NULL);
		return;
	}

	SDL_Renderer* renderer = screen->renderer;
	int success = SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR); //set the right color
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n", SDL_GetError());
	}

	success = SDL_RenderClear(renderer); //clear, this is for safety, no need to check,failure won't effect
	if (success == -1) {
		printf("ERROR: unable to draw screen: %s\n", SDL_GetError());
	}

	if (screenIndex == GAME_SCREEN) {
		spDrawBoard(renderer);
	}


	spDrawWidgets(screen);
	SDL_Delay(10);
	SDL_RenderPresent(renderer);
}

int spOpenWindow(Screen** screens, int window, SPChessGame* game) {
	if (!screens) {
		return NONE;
	}
	int i;
	for(i = 0; i < NUM_SCREENS; i++) {
		if (i == window) {
			screens[i]->shown = SHOWN; // show our window
			SDL_ShowWindow(screens[i]->window);
		}
		else {
			screens[i]->shown = HIDE; // hide all the other
			SDL_HideWindow(screens[i]->window);
		}
	}
	if(window == MAIN_MENU_WINDOW){
		spChessGameResetGame(game); //need to reset game
    	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE; // game will not be save anymore
	}
	return PRESSED;
}

int spOpenLoadSaveGameWindow(Screen** screens,SPChessGame* game, int screenIndex, int previousScreen) {

	screens[screenIndex]->previousWindow = previousScreen; // update where to return
	screens[screenIndex]->scrollBarPosition = 0; // align scrollbar
	return spOpenWindow(screens, screenIndex ,game);

}

int spOpenNextWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !game) {
		return -1;
	}
	// opening the load/save window if necessary
	if((screenIndex == GAME_SCREEN && widgetIndex == GS_LOAD_GAME) ||
	   (screenIndex == MAIN_MENU_WINDOW && widgetIndex == MM_LOAD_GAME)) {
		return spOpenLoadSaveGameWindow(screens,game, LOAD_GAME_WINDOW, screenIndex);
	}
	else if (screenIndex == GAME_SCREEN && widgetIndex == GS_SAVE_GAME) {
		//update where to go after game is saved.
		screens[SAVE_GAME_WINDOW]->nextWindow = GAME_SCREEN;
		return spOpenLoadSaveGameWindow(screens,game,  SAVE_GAME_WINDOW, screenIndex);
	}
	else if (screenIndex == GET_MODE_WINDOW) {
		game->gameMode = widgetIndex;// the game mode is the same as the widget index
		// if game mode is 2 players need to update board and jump to game screen
		spUpdateBoard(screens, game);
		screens[screenIndex]->nextWindow = widgetIndex == 1 ? GET_DIFFICULTY_WINDOW : GAME_SCREEN;
	}
	else if (screenIndex == GET_DIFFICULTY_WINDOW) {
		game->difficulty = widgetIndex; // the difficulty is the same as the widget position
	}
	else if (screenIndex == GET_COLOR_WINDOW) {
		game->userColor = widgetIndex; // the color is the same as the widget position
		if (widgetIndex == BLACK) { // white starts, so need to update computer first move before game starts
			spUpdateBoard(screens, game);
		}
	}


	if(screens[screenIndex]->nextWindow == NO_SCREEN){
		return QUIT;
	}

	return  spOpenWindow(screens, screens[screenIndex]->nextWindow,game);
}

int spOpenPreviousWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !game || widgetIndex > N_MAX_WIDGETS) { // sanity check
		printf("ERROR: Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR, Resources for game were lost. quitting",
								 NULL);
		return QUIT;
	}

	return spOpenWindow(screens, screens[screenIndex]->previousWindow,game);
}

int spQuit(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !game) {
		return -1;
	}
	// checks if warning the user from quitting without saving necessary
	if (screenIndex == GAME_SCREEN && !screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown) {
		return spShowSaveBeforeQuitMessage(screens, game, screenIndex, widgetIndex);
	}

	return QUIT;
}

int spOpenMainMenuWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !game) {
		return -1;
	}
	if (screenIndex == GAME_SCREEN &&  // checks if needs to warn the user from quiting without saving
		screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown == HIDE) {
		return spShowSaveBeforeQuitMessage(screens, game, screenIndex, widgetIndex);
	}
	
	return spOpenWindow(screens, MAIN_MENU_WINDOW,game);
}
