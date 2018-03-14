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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSOLE 0
#define GUI 1
#define CONSOLE_ARGUMENT "-c"
#define GUI_ARGUMENT "-g"

int main(int argc, char* argv[]) {
	int mode = CONSOLE;
	if (argc == 2) {
		if (!strcmp(argv[1], GUI_ARGUMENT)) {
			mode = GUI;
		}
		else if (strcmp(argv[1], CONSOLE_ARGUMENT)) {
			printf("Usage: ./chessprog [-c | -g]");
			return 0;
		}
	}

	SPChessGame* game = spChessGameCreate();
	if (!game) {
		perror("ERROR: Could not create game! Exiting...\n");
		return 1;
	}

	if (!mode) {
		printf(" Chess\n");
		for (int i = 0; i < 7; i++) {
			printf("-");
		}
		printf("\n");
	}
	char* difficulties[] = {"", "amateur", "easy", "moderate", "hard", "expert"};

	int quit = 0;
	int settings = 1;
	int printedWelcome = 0;
	while (!quit) {
		while (settings) {
			if (!printedWelcome) {
				printf("Specify game settings or type 'start' to begin a game with the current settings:\n");
				printedWelcome = 1;
			}
			printf("GET COMMAND MESSAGE"); // TODO - fix me!
			SPCommand cmd = spGetCommand();

			if (cmd.cmd == SP_GAME_MODE) {
				int gameMode = spParserGetNonNegativeInt(cmd.arguments);
				if (gameMode == 1 || gameMode == 2) {
					game->gameMode = gameMode;
					printf("Game mode is set to %d-player\n", gameMode);
				}
				else {
					printf("Wrong game mode\n");
				}
			}

			else if (cmd.cmd == SP_DIFFICULTY && game->gameMode == 1) {
				int difficulty = spParserGetNonNegativeInt(cmd.arguments);
				if (difficulty >= 1 && difficulty <= 5) {
					game->difficulty = difficulty;
					printf("Difficulty level is set to %s", difficulties[difficulty]);
				}
				else {
					printf("Wrong difficulty level. The value should be between 1 to 5\n");
				}
			}

			else if (cmd.cmd == SP_USER_COLOR && game->gameMode == 1) {
				int userColor = spParserGetNonNegativeInt(cmd.arguments);
				if (userColor == 0 || userColor == 1) {
					game->userColor = userColor;
					printf("User color is set to %s", userColor ? "white" : "black");
				}
				else {
					printf("Wrong user color. The value should be 0 or 1\n");
				}
			}

			else if (cmd.cmd == SP_LOAD) {
				spLoadGame(game, cmd.arguments);
			}

			else if (cmd.cmd == SP_DEFAULT) {
				game->userColor = WHITE;
				game->gameMode = 1;
				game->difficulty = 2;
			}

			else if (cmd.cmd == SP_PRINT_SETTINGS) {
				printf("SETTINGS:\n");
				printf("GAME_MODE: %d-player\n", game->gameMode);
				if (game->gameMode == 1) {
					printf("DIFFICULTY: %s", difficulties[game->difficulty]);
					printf("USER_COLOR: %s", game->userColor ? "white" : "black");
				}
			}

			else if (cmd.cmd == SP_QUIT) {
				quit = 1;
			}

			else if (cmd.cmd == SP_START) {
				settings = 0;
				printf("Starting game...\n");
			}

			else {
				perror("ERROR: Invalid command");
			}
		}


	}
}
