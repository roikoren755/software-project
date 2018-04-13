/*
 * main.c
 *
 *  Created on: 6 March 2018
 *      Author: user
 */

#include "SPChessParser.h"
#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
#include "SPMinimax.h"
#include "SPBufferset.h"
#include <stdio.h>
#include <string.h>

#define CONSOLE 0
#define GUI 1
#define CONSOLE_ARGUMENT "-c"
#define GUI_ARGUMENT "-g"
#define USAGE "Usage: ./chessprog [-c | -g]\n"

int main(int argc, char* argv[]) {
	SP_BUFF_SET();
    int mode = CONSOLE;
    if (argc == 1) {
		mode = CONSOLE;
	}
	else if (argc == 2) {
        if (!strcmp(argv[1], GUI_ARGUMENT)) {
            mode = GUI;
        }
        else if (strcmp(argv[1], CONSOLE_ARGUMENT)) {
            printf(USAGE);
            return 0;
        }
    }
	else {
		printf(USAGE);
		return 0;
	}

	SPChessGame* game = spChessGameCreate();
	if (!game) {
		printf("ERROR: Could not create game! Exiting...\n");
		return 1;
	}

	if (mode == GUI) {
		return runSdl(game);
	}
	else {
		return runConsole(game);
	}
}
