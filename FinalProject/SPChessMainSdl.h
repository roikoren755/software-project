/*
 * SPChessMainSdl.h
 *
 *  Created on: 10 באפר 2018
 *      Author: user
 */
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPBufferset.h"
#include "SPChessGame.h"
#include "Button.h"
#include <string.h>
#include "SPChessParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"

#include <unistd.h>

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

#define GM_MASSAGE 0
#define GM_1PLAYER 1
#define GM_2PLAYER 2
#define GM_BACK_TO_MM 3

#define GD_MASSAGE 0
#define GD_DIFFICULTY(i) i
#define GD_BACK 6
#define GD_BACK_TO_MM 7

#define GC_MASSAGE 2
#define GC_WHITE 1
#define GC_BLACK 0
#define GC_BACK 3
#define GC_BACK_TO_MM 4

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

//#define OFFSET_FOR_STATES_WIDGETS_INDEX 38

#define NUM_SLOTS 9
#define SLOT_HEIGHT 70
#define SCROLLBAR_MAX_POSITION (NUM_SLOTS*SLOT_HEIGHT-300)
#define NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS 6
#define LSG_MASSAGE 0
#define LSG_BOTTOM_COVER 1
#define LSG_BACK 2
#define LSG_NO_SLOTS 3
#define LSG_UP_ARRAW 4
#define LSG_DOWN_ARRAW 5
#define LSG_SLOT(i) (i+5)
#define LSG_SLOT_INDICATOR(i) (i+5+NUM_SLOTS)
#define WIDGET_TO_SLOT_INDEX(i) (i-5)

#define SHOWN 1
#define HIDE 0
#define ROUNDED_POSITION(x) (40+x-(x+30)%80)
#define BOARD_TO_SCREEN_LOCATION(x) (10+80*x)
#define SCREEN_TO_BOARD_LOCATION(x) ((x-10)/80)

int SPGameCreateScreens(Screen** screens);

Screen* SPGameCreateMainMenuWindow();

Screen* SPGameCreateGetModeWindow();

Screen* SPCreateGetDifficultyWindow();

Screen* SPCreateGetColorWindow();

void SPDefaultDrawScreen(Screen* screen);

int SPOpenWindow(Screen** screens,int window);

void SPDrawWidgets(Screen* screen);

int SPOpenLoadSaveGameWindow(Screen** screens ,int screenIndex , int previousScreen);

int SPOpenNextWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPOpenPreviousWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPQuit(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);
