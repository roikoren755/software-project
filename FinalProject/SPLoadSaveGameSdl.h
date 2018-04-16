/*
 * SPLoadSaveGameSdl.h
 *
 *  Created on: 10 March 2018
 *      Author: user
 */

#ifndef SPLOADSAVEGAMESDL_H_
#define SPLOADSAVEGAMESDL_H_

#include "SPChessMainSdl.h"
#include <unistd.h>


#define NUM_SLOTS 5
#define SLOT_HEIGHT 70
#define SCROLLBAR_MAX_POSITION (NUM_SLOTS * SLOT_HEIGHT - 300)
#define NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS 5
#define LSG_MESSAGE 0
#define LSG_BOTTOM_COVER 1
#define LSG_BACK 2
#define LSG_UP_ARROW 3
#define LSG_DOWN_ARROW 4
#define LSG_SLOT(i) (i + NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS - 1)
#define LSG_SLOT_INDICATOR(i) (i + NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS + NUM_SLOTS - 1)
#define WIDGET_TO_SLOT_INDEX(i) (i - NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS)
#define LOAD_SAVE_N_WIDGETS (NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS + 2 * NUM_SLOTS)


/**
 * SPLoadSaveGameSdl summary:
 *
 * Contains the GUI functions for the load/save game screen.
 * To allow a different number of slots to be displayed, one can
 * change the define: NUM_SLOTS in this file. (NOTE: it is essential to check whether the value of
 * LOAD_SAVE_N_WIDGETS is not higher then N_MAX_WIDGETS. if does, change the value of N_MAX_WIDGETS
 * to be high enough).
 */

 /**
 *  Creates the Load/Save window. this is the only window shown when initialized
 *  @param  screenIndex
 *  @return	NULL, if an error occurred.
 *  		Otherwise An instant of a screen with content corresponding to the load/save
 *  		window, according to the given screen index  .
 */
Screen* spCreateLoadSaveGameWindow(int screenIndex);

/**
*  Draws the load/save given screen.
*  @param screen - pointer to a screen.
*  @param screenIndex - the index of the screen to be drawn.
*/
void spDrawLoadSaveScreen(Screen* screen, int screenIndex);

/**
*  Checks if there are saved game available for the file names corresponding the slots.
*  if a slot has a file related to, show the label iindicating it is used. otherwise hide it.
*  @param screens - pointer to the game's screen array.
*/
void spUpdateLoadSaveSlots(Screen** screens);

/**
*  NOTE:
*  The signatures of the following functions is
*  to match them to the signature suitable to the button struct
*/

/**
*  Moves the scrollbar of a the load/save screen.
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spMoveScrollbar(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Loads a game located at the file's path related to a given slot.
*  If loading failed, shows a message to the user
*  Otherwise shows a success message and loads the game.
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spLoadChosenGame(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Saves a game to the file's path related to a given slot, overwriting if exist already.
*  If saving failed, shows a message to the user
*  Otherwise shows a success message and saves the game.
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spSaveChosenGame(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

#endif /* SPLOADSAVEGAMESDL_H_ */
