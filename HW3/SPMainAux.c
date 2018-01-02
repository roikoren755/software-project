//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <string.h>
#include "SPFIARGame.h"
#include "SPFIARParser.h"
#include "SPMinimax.h"
#include "SPMainAux.h"

#define UNDO_MOVES_POSSIBLE 10
#define QUIT "quit\n"

int spGetDifficulty() {
	printf("Please enter the difficulty level between [1-7]:\n");
	char input[MAXIMUM_COMMAND_LENGTH + 1];
	fgets(input, MAXIMUM_COMMAND_LENGTH, stdin); // get level from user
	while (input[0] < '1' || input[0] > '7' || // first char isn't digit in range
			input[1] != '\n') { // or next char isn't new-line
		if (!strcmp(input, QUIT)) { // user entered "quit"
			return 0;
		}
		printf("Error: invalid level (should be between 1 to 7)\n");
		printf("Please enter the difficulty level between [1-7]:\n");
		fgets(input, MAXIMUM_COMMAND_LENGTH, stdin); // try again
			}
	return input[0] - '0'; // return first char as int
}

int spFiarGameGetLastMovePlayed(SPFiarGame* game) {
	if (!game || spArrayListIsEmpty(game->history)) { // invalid arguments
		return -1;
	}
	return spArrayListGetFirst(game->history); // get last move added to history
}

int spFiarGameSuggestMove(SPFiarGame* game, unsigned int maxDepth) {
	int suggestedMove = spMinimaxSuggestMove(game, maxDepth); // suggest move
	if (suggestedMove == -1) { // suggest move failed!
		printf("Error: spMinimaxSuggestMove has failed");
		spFiarGameDestroy(game);
		return 0;
	}
	printf("Suggested move: drop a disc to column %d\n", suggestedMove + 1); // change to 1-base
	return 1;
}

void spFiarGameUndoMove(SPFiarGame* game, char winner) {
	if (spArrayListIsEmpty(game->history)) { // nothing to undo
		printf("Error: cannot undo previous move!\n");
	} else {
		if (winner != SP_FIAR_GAME_PLAYER_1_SYMBOL) { // computer played last
			printf("Remove disc: remove computer's disc at column %d\n",
					spFiarGameGetLastMovePlayed(game) + 1);
			spFiarGameUndoPrevMove(game); // undo computer's move
		}
		printf("Remove disc: remove user's disc at column %d\n",
				spFiarGameGetLastMovePlayed(game) + 1);
		spFiarGameUndoPrevMove(game); // undo player's move
		spFiarGamePrintBoard(game);
		printf("Please make the next move:\n");
	}
}

int spFiarGameAddDisc(SPFiarGame* game, SPCommand command,
		unsigned int maxDepth) {
	if (!command.validArg || // // invalid argument for add_disc command
			command.arg < 1 || command.arg > SP_FIAR_GAME_N_COLUMNS) { // check arg is valid for game
		printf("Error: column number must be in range 1-7\n");
		return -1;
	}
	int col = command.arg - 1; // switch to 0-based
	if (!spFiarGameIsValidMove(game, col)) { // make sure move is valid
		printf("Error: column %d is full\n", command.arg);
		return -1;
	}
	spFiarGameSetMove(game, col); // make move
	char winner = spFiarCheckWinner(game); // check for winner
	if (winner) {
		spFiarGamePrintBoard(game);
		return 1; // skip computer's turn
	}
	int computerCol = spMinimaxSuggestMove(game, maxDepth); // get computer move
	if (computerCol == -1) { // failed!
		printf("Error: spMinimaxSuggestMove has failed");
		spFiarGameDestroy(game);
		return 0;
	}
	printf("Computer move: add disc to column %d\n", computerCol + 1);
	spFiarGameSetMove(game, computerCol);
	spFiarGamePrintBoard(game);
	return 1;
}

int spFiarGameRestart(SPFiarGame** gamePointer) {
	printf("Game restarted!\n");
	spFiarGameDestroy(*gamePointer); // clear game
	*gamePointer = (spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE)); // new game!
	int maxDepth = spGetDifficulty();
	if (!maxDepth) { // user entered "quit"
		printf("Exiting...\n");
		spFiarGameDestroy(*gamePointer);
		return maxDepth;
	}
	spFiarGamePrintBoard(*gamePointer);
	printf("Please make the next move:\n");
	return maxDepth;
}

int spRunGame(SPFiarGame* game, int maxDepth) {
	int success;
	char winner = 0;
	char input[MAXIMUM_COMMAND_LENGTH + 1];
	while (1) { // until user entered "quit"
		fgets(input, MAXIMUM_COMMAND_LENGTH, stdin); // get command
		SPCommand command = spParserPraseLine(input); // parse it
		if (winner && // game ended, and illegal command entered
				(command.cmd == SP_ADD_DISC || command.cmd ==SP_SUGGEST_MOVE)) {
			printf("Error: the game is over\n");
		}
		if (command.cmd == SP_INVALID_LINE) { // invalid command
			printf("Error: invalid command\n");
		}
		if (command.cmd == SP_SUGGEST_MOVE && !winner) { // can suggest move
			success = spFiarGameSuggestMove(game, maxDepth); // do it
			if (!success) { // suggest move failed, all memory freed
				return -1;
			}
		}
		if (command.cmd == SP_UNDO_MOVE) { // can always undo (mostly)
			spFiarGameUndoMove(game, winner);
			winner = 0;
		}
		if (command.cmd == SP_ADD_DISC && !winner) { // can add disc
			success = spFiarGameAddDisc(game, command, maxDepth);
			if (!success) { // add disc failed, all memory freed
				return -1;
			}
			if (success == 1) {
				winner = spFiarCheckWinner(game);
				if (!winner) { // game continues
					printf("Please make the next move:\n");
				}
				else {
					if (winner == SP_FIAR_GAME_TIE_SYMBOL) { // it's a tie!
						printf("Game over: it's a tie\nPlease enter 'quit' to exit or 'restart' to start a new game!\n");
					}
					else { // print correct winning message
						printf("Game over: %s\nPlease enter 'quit' to exit or 'restart' to start a new game!\n", winner == SP_FIAR_GAME_PLAYER_1_SYMBOL ? "you win" : "computer wins");
					}
				}
			}
		}
		if (command.cmd == SP_QUIT) { // bye bye
			printf("Exiting...\n");
			spFiarGameDestroy(game);
			return 0;
		}
		if (command.cmd == SP_RESTART) {
			maxDepth = spFiarGameRestart(&game);
			if (!maxDepth) { // user entered "quit" during restart
				return 0;
			}
			winner = 0;
		}
	}
	if (game) {
		spFiarGameDestroy(game); // shouldn't get here, but just in case...
	}
	return 1;
}
