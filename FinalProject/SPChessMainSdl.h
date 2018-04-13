/*
 * SPChessMainSdl.h
 *
 *  Created on: 10 March 2018
 *      Author: user
 */
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPChessGame.h"
#include "Button.h"
#include <string.h>
#include "SPChessParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"


#define NUM_SCREENS 7

#define BACKGROUND_COLOR 255, 204, 153, 0
#define BLACK_COLOR 0, 0, 0, 0
#define CHESS_BROWN_COLOR 204, 102, 0, 0
#define CHESS_YELLOW_COLOR 255, 255, 102, 0

#define GAME_SCREEN 0
#define MAIN_MENU_WINDOW 1
#define GET_MODE_WINDOW 2
#define GET_DIFFICULTY_WINDOW 3
#define GET_COLOR_WINDOW 4
#define LOAD_GAME_WINDOW 5
#define SAVE_GAME_WINDOW 6

#define MM_NEW_GAME 0
#define MM_LOAD_GAME 1
#define MM_QUIT 2
#define MAIN_MENU_N_WIDGETS 3

#define GM_MASSAGE 0
#define GM_1PLAYER 1
#define GM_2PLAYER 2
#define GM_BACK_TO_MM 3
#define GET_MODE_N_WIDGETS 4

#define GD_MASSAGE 0
#define GD_DIFFICULTY(i) i
#define GD_BACK 6
#define GD_BACK_TO_MM 7
#define GET_DIFFICULTY_N_WIDGETS 8

#define GC_MASSAGE 2
#define GC_WHITE 1
#define GC_BLACK 0
#define GC_BACK 3
#define GC_BACK_TO_MM 4
#define GET_COLOR_N_WIDGETS 5

#define CONTINUE 1
#define SHOWN 1
#define HIDE 0
#define ROUNDED_POSITION(x) (40+x-(x+30)%80)
#define BOARD_TO_SCREEN_LOCATION(x) (10+80*x)
#define SCREEN_TO_BOARD_LOCATION(x) ((x-10)/80)


/**
 * SPChessMainSdl summary:
 *
 * Contains the main functions for the GUI mode: the creating of the settings windows and
 * functions used by only them or by them and the game and load/save windows.
 *
 * SPGameCreateScreens     		  - Creates all the relevant screens for the chess game into
 * 								    a given array.
 * SPGameCreateMainMenuWindow     - Creates the main menu.
 * SPGameCreateGetModeWindow      - Creates the get mode window.
 * SPCreateGetDifficultyWindow    - Creates the get difficulty window.
 * SPGameCreateGetModeWindow      - Creates the get mode window.
 * SPCreateGetColorWindow         - Creates the get color window.
 * SPCheckWidgetsInit             - after a screen is created, checks if
 * 									all widgets allocated successfully         	  .
 * SPDrawScreen              	  - Draws a screen and the widgets contained in it.
 * SPDrawWidgets                  - Draws all widgets in a window
 * SPOpenWindow         	      - Shows a window and hides all the others
 * SPOpenLoadSaveGameWindow       - Opens load/save window
 * SPOpenNextWindow               - Opens the next window of the one shown
 * SPOpenPreviousWindow           - Opens the next window of the one shown
 * SPQuit					      - Quits the game
 * SPOpenMainMenuWindow           - Opens the main menu window

 */

/**
 *  Creates all the relevant screens for the chess game into
 * 	a given array.
 *  @param screens - pointer to an array of screens.
 *  @return 0 if an error occurred.
 *  		1 on success.
 */
int SPGameCreateScreens(Screen** screens);

 /**
 *  Creates the main menu. this is the only window shown when initialized
 *  @return		NULL, if an error occurred.
 *  			Otherwise An instant of a screen with content corresponding to the main menu .
 */
Screen* SPGameCreateMainMenuWindow();

/**
*  Creates Creates the get mode window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get mode window.
*/
Screen* SPGameCreateGetModeWindow();

/**
*  Creates Creates the get difficulty window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get difficulty window.
*/
Screen* SPCreateGetDifficultyWindow();

/**
*  Creates Creates the get color window.
*  @return		NULL, if an error occurred.
*  				Otherwise An instant of a screen with content corresponding to the get color window.
*/
Screen* SPCreateGetColorWindow();

/**
*  after a screen is created, checks if
*  all widgets allocated successfully
*  @param screen - pointer to a screen.
*
*  @return 0 if one or more of the widgets soes not exist.
*  		   1 on success.
*/
int SPCheckWidgetsInit(Screen* screen);


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

*
*/
void SPDrawScreen(Screen* screen,int screenIndex);

/**
*  Draws the widgets contained in a screen( this function is used by the other drawing functions).
*  @param screen - pointer to a screen.
*/
void SPDrawWidgets(Screen* screen);

/**
*  Shows a window and hides all the others in the given array.
*  @param screens - pointer to the game's array of screens.
*  		  screenIndex - the index of the screen to be shown.
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*
*/
int SPOpenWindow(Screen** screens,int screenIndex);

/**
*  Shows the load/save window.
*  @param screens - pointer to the game's array of screens.
*  @param screenIndex - the index of the screen: load or save.
*  @param previousScreen - the index of the screen currently opened, needed
		  because we need to know where to return
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPOpenLoadSaveGameWindow(Screen** screens ,int screenIndex , int previousScreen);


/**
*  NOTE:
*  The signing of the following functions is
*  to match them to the signing suitable to the button struct
*/

/**
*  Opens the next window of the one shown at the moment
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPOpenNextWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

/**
*  Opens the next window of the one shown at the moment
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPOpenPreviousWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

/**
*  Quits the game
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return QUIT signal, indicating the user requested to quit.
*/
int SPQuit(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

/**
*  Opens the main menu window.
*  if the user requested from the game's screen, and current game is unsaved, calls
*  the function: SPShowSaveBeforeQuitMassage (see documentation in SPChessGameSdl.h).
*  if the user requested from the game's screen, and current game is saved, resets the
*  current game before opening the main menu.
*
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget used to call this action within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);
