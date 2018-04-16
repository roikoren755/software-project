/*
 * SPChessMainSdl.h
 *
 *  Created on: 10 March 2018
 *      Author: user
 */
#include <SDL.h>
#include <SDL_video.h>
#include "Widget.h"
#include "SPChessGame.h"

#define NUM_SCREENS 7

#define BACKGROUND_COLOR 255, 204, 153, 0
#define CHESS_YELLOW_COLOR 255, 255, 102, 0

#define GAME_SCREEN 0
#define MAIN_MENU_WINDOW 1
#define LOAD_GAME_WINDOW 5
#define SAVE_GAME_WINDOW 6
#define NO_SCREEN -1

#define CONTINUE 1
#define SHOWN 1
#define HIDE 0

/**
 * SPChessMainSdl summary:
 *
 * Contains the main functions for the GUI mode: the creating of the settings windows and
 * functions used by only them or by them and the game and load/save windows.
 *
 * spGameCreateScreens     		  - Creates all the relevant screens for the chess game into
 * 								    a given array.
 * spCheckWidgetsInit             - after a screen is created, checks if
 * 									all widgets allocated successfully         	  .
 * spDrawScreen              	  - Draws a screen and the widgets contained in it.
 * spOpenWindow         	      - Shows a window and hides all the others
 * spOpenLoadSaveGameWindow       - Opens load/save window
 * spOpenNextWindow               - Opens the next window of the one shown
 * spOpenPreviousWindow           - Opens the next window of the one shown
 * spQuit					      - Quits the game
 * spOpenMainMenuWindow           - Opens the main menu window
 */

/**
 *  Creates all the relevant screens for the chess game into
 * 	a given array.
 *  @param screens - pointer to an array of screens.
 *  @return 0 if an error occurred.
 *  		1 on success.
 */
int spGameCreateScreens(Screen** screens);

/**
*  after a screen is created, checks if
*  all widgets allocated successfully
*  @param screen - pointer to a screen.
*
*  @return 0 if one or more of the widgets soes not exist.
*  		   1 on success.
*/
int spCheckWidgetsInit(Screen* screen);

/**
*  NOTE:
*  in drawing errors, no massage box show up.
*  Drawing is done in loops, can't 'overflow' the user with massages.
*  Moreover, in some cases drawing errors will not have a great effect, and the game
*  is still playable.
*/

/**
*  Draws a screen and the widgets contained in it, and presents the changes to user.
*  if the screen is the game's screen, draws the board first.
*  @param screen - pointer to a screen.
*  @param screenIndex - the index of the screen to be drawn.
*/
void spDrawScreen(Screen* screen,int screenIndex);

/**
*  Shows a window and hides all the others in the given array.
*  @param screens - pointer to the game's array of screens.
*  		  screenIndex - the index of the screen to be shown.
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*
*/
int spOpenWindow(Screen** screens, int screenIndex);

/**
*  Shows the load/save window.
*  @param screens - pointer to the game's array of screens.
*  @param screenIndex - the index of the screen: load or save.
*  @param previousScreen - the index of the screen currently opened, needed
		  because we need to know where to return
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spOpenLoadSaveGameWindow(Screen** screens, int screenIndex, int previousScreen);

/**
*  NOTE:
*  The signatures of the following functions is
*  to match them to the signature suitable to the button struct
*/

/**
*  Opens the next window of the one shown at the moment
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spOpenNextWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Opens the next window of the one shown at the moment
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spOpenPreviousWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Quits the game
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return QUIT signal, indicating the user requested to quit.
*/
int spQuit(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Opens the main menu window.
*  if the user requested from the game's screen, and current game is unsaved, calls
*  the function: spShowSaveBeforeQuitMessage (see documentation in SPChessGameSdl.h).
*  if the user requested from the game's screen, and current game is saved, resets the
*  current game before opening the main menu.
*
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget used to call this action within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int spOpenMainMenuWindow(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);
