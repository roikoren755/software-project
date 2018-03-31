/*
 * SPChessGameSdl.h
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */

#ifndef SPCHESSGAMESDL_H_
#define SPCHESSGAMESDL_H_
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPBufferset.h"
#include "SPChessGame.h"
#include "Button.h"
#include "SPChessGameSdl.h"
#include <string.h>
#include "SPChessParser.h"
#include "SPMainAux.h"

#define NUM_SCREENS 5

#define MAIN_MENU_WINDOW 1
#define MM_NEW_GAME 0
#define MM_LOAD_GAME 1
#define MM_QUIT 2
#define GET_MODE_WINDOW 2
#define GM_MASSAGE 0
#define GM_1PLAYER 1
#define GM_2PLAYER 2
#define GM_BACK_TO_MM 3
#define GET_DIFFICULTY_WINDOW 3
#define GD_MASSAGE 0
#define LG_DIFFICULTY(i) i
#define GD_BACK 6
#define GD_BACK_TO_MM 7
#define GET_COLOR_WINDOW 4
#define GC_MASSAGE 2
#define GC_WHITE 1
#define GC_BLACK 0
#define GC_BACK 3
#define GC_BACK_TO_MM 4
#define GAME_SCREEN 0
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
#define SHOWN 1
#define HIDE 0
#define LG_MASSAGE 0
#define LG_SLOT(i) i+2
#define LG_BACK 1
#define LG_NO_SLOTS 2

#define ROUNDED_POSITION(x) (40+x-(x+30)%80)
#define BOARD_TO_SCREEN_LOCATION(x) (10+80*x)
#define SCREEN_TO_BOARD_LOCATION(x) ((x-10)/80)
#define NONE 0
int SPDrawScreen(Screen** screens, int window);

int SPDrawBoard(SDL_Renderer* rend);

void SPShowGameState(Screen** screens, int state);

int SPUpdateBoard(Screen** screens, SPChessGame* game);

int SPUpdateGameState(Screen** screens, SPChessGame* game);

int SPRestartGame(Screen** screens ,SPChessGame* game,int j);
int SPOpenSaveGameWindow(Screen** screens ,SPChessGame* game,int j);
int SPOpenLoadGameWindow(Screen** screens ,SPChessGame* game,int j);
int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int j);
int SPOpenGetModeWindow(Screen** screens ,SPChessGame* game,int j);
int SPOpenGetDifficultyWindow(Screen** screens ,SPChessGame* game,int j);
int SPOpenGetColorWindow(Screen** screens ,SPChessGame* game,int j);
int SPLoadChosenGame(Screen** screens ,SPChessGame* game,int j);

int SPQuit(Screen** screens ,SPChessGame* game,int j);
int SPUndoMove(Screen** screens ,SPChessGame* game,int j);
int SPSetGameMode(Screen** screens ,SPChessGame* game,int j);
int SPSetGameDifficulty(Screen** screens ,SPChessGame* game,int j);
int SPSetGameColor(Screen** screens ,SPChessGame* game,int j);
int SPMovePiece(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game);
int SPHighlightAllMoves(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game);
int SPHighlightMove(SDL_Renderer* rend , int step);

int SPNoOperation(Screen** screens ,SPChessGame* game,Widget * src);
int SPCloseWindow(Screen** screens ,SPChessGame* game,int j);

int SPOpenWindow(Screen** screens,int window);

#endif /* SPCHESSGAMESDL_H_ */
