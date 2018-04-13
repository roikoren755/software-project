/*
 * SPChessGameSdl.h
 *
 *  Created on: 19 ���� 2018
 *      Author: user
 */
#ifndef SPCHESSGAMESDL_H_
#define SPCHESSGAMESDL_H_

#include "SPChessMainSdl.h"

#define GS_RESTART_GAME 32
#define GS_SAVE_GAME 33
#define GS_LOAD_GAME 34
#define GS_MAIN_MENU 35
#define GS_QUIT 36
#define GS_UNDO 37
#define GS_GET_MOVES_LEGEND 38
#define GS_CHECK 39
#define GS_CHECK_MATE 40
#define GS_DRAW 41
#define GS_SAVED_GAME_INDICATOR 42

#define SQUARE_WIDTH 80
#define SQUARE_HEIGHT 80
#define BOARD_WIDTH 640


/**
 * SPChessGameSdl summary:
 *
 * Contains the GUI functions for the game's screen.
 */


/**
*  Creates the game's screen.
*  @return		NULL, if an error occurred.
*  				otherwise, An instant of a screen with content corresponding to the game's screen.
*/
Screen* SPGameCreateGameScreen();

/**
*  Update's the game's screen's board to be corresponding with the game state.
*  @return		QUIT signal if fatal error occurred.
*  				CONTINUE signal , if otherwise.
*/
int SPUpdateBoard(Screen** screens, SPChessGame* game);

/**
*  Draws a 8x8 board to the renderer.
*  @param renderer - pointer to an SDL_Renderer.
*/
void SPDrawBoard(SDL_Renderer* rend);

/**
*  NOTE:
*  The signatures of the following functions is
*  to match them to the signature suitable to the button/sticker struct
*/

/**
*  Opens the next window of the one shown at the moment
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPRestartGame(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Opens a massage box asking the user if he wants to save the game before quitting.
*  If the answer is yes, opens the save game window.
*  @param screens - pointer to the game's array of screens.
*  @param game - pointer to the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return QUIT signal if the user asked to quit
*  		   otherwise, PRESSED signal, indicating a widget was pressed in order to call this action.
*
*/
int SPShowSaveBeforeQuitMassage(Screen** screens,SPChessGame* game, int screenIndex, int widgetIndex);

/**
*  Undo-ing the last move (or last two moves if necessary) and updates the screen and the undo
*  button availability if necessary.
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @param screenIndex - the index of the screen shown at the moment.
*  @param widgetIndex - the Index of the widget within the screen's widgets array
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPUndoMove(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex);

/**
 * Displays a massage informing an error occurred while waiting for an event
 * and returns QUIT signal (if there is a problem with getting events the user
 * will not be able to quit, quitting from the console unavailable).
 * @return QUIT signal.
*/
int SPShowEventErrorAndQuit();

/**
*  Moves a piece dragged by the user.
*  If the user releases the piece inside the board, if it's a valid move,
*  performs the move and updates the board (if necessary, performs a computer move before updating).
*  If the move is invalid, shows a massage box with an appropriate informative message.
*
*  @param src - pointer the widget representing the piece.
*  @param event - pointer to the event caused the calling to this function
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @return QUIT signal if drawing error occurred
*  		   PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPMovePiece(Widget* src, SDL_Event* event, Screen** screens, SPChessGame* game);

/**
*  Highilghts all possible moves of a piece (calles when piece is left clicked).
*  If an error accrued, shows a massage box with an appropriate message.
*  The moves are highlighted in  four different colors:
*  a green square for standard move, a red square for threatened move,
*  a blue square for capture (occupied by an opponent piece),
*  or a half red, half blue square for both threatened and a capture square.
*  An appropriate legend appears in the bottom right of the screen.
*
*  @param src - pointer the widget representing the piece.
*  @param event - pointer to the event caused the calling to this function
*  @param screens - pointer to the game's array of screens.
*  @param game - the game's struct
*  @return PRESSED signal, indicating a widget was pressed in order to call this action.
*/
int SPHighlightAllMoves(Widget* src, SDL_Event* event, Screen** screens, SPChessGame* game);

#endif /* SPCHESSGAMESDL_H_ */
