/*
 * SPLoadSaveGameSdl.h
 *
 *  Created on: 10 באפר 2018
 *      Author: user
 */

#ifndef SPLOADSAVEGAMESDL_H_
#define SPLOADSAVEGAMESDL_H_

#include "SPChessMainSdl.h"

Screen* SPCreateLoadSaveGameWindow(int screenIndex);

void SPDrawLoadSaveScreen(Screen* screen);

int SPMoveScrollbar(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPLoadChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

int SPSaveChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex);

void SPUpdateLoadSaveSlots(Screen** screens);


#endif /* SPLOADSAVEGAMESDL_H_ */
