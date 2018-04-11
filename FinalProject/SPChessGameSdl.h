/*
 * SPChessGameSdl.h
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */
#ifndef SPCHESSGAMESDL_H_
#define SPCHESSGAMESDL_H_

#include "SPChessMainSdl.h"

Screen* SPGameCreateGameScreen();

int SPDrawBoard(SDL_Renderer* rend);

int SPUpdateBoard(Screen** screens, SPChessGame* game);

Screen* SPGameCreateGetModeWindow();

Screen* SPCreateGetColorWindow();

void SPDrawGameScreen(Screen* screen);

int SPRestartGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPShowSaveBeforeQuitMassage(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPUndoMove(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPMovePiece(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game);

int SPHighlightAllMoves(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game);

//void SPShowGameState(Screen** screens, int state);



void printLocations(SPChessGame* game);

#endif /* SPCHESSGAMESDL_H_ */
