/*
 * SPChessSdl.h
 *
 *  Created on: 19 ���� 2018
 *      Author: user
 */
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPBufferset.h"
#include "SPChessGame.h"
#include "Button.h"
#include "SPChessGameSdl.h"

#ifndef SPCHESSSDL_H_
#define SPCHESSSDL_H_

#define BACK_TO_MAIN_MENU 2
#define BACK 1

int SPGameCreateScreens(Screen** screens);

Screen* SPGameCreateGameScreen();

Screen* SPGameCreateMainMenuWindow();

Screen* SPGameCreateGetModeWindow();

Screen* SPCreateGetDifficultyWindow();

Screen* SPCreateGetColorWindow();

#endif /* SPCHESSSDL_H_ */
