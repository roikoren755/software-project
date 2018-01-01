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
#define QUIT  "quit"

int spGetDifficulty() {
    printf("Please enter the difficulty level between [1-7]:\n");
    char maxDepth[10];
    int result = scanf("%9s", maxDepth);
    while (result == EOF || result == 0 || maxDepth[0] < '1' || maxDepth[0] > '7' || maxDepth[1]!='\0') {
    	if (!strcmp(maxDepth, QUIT)) {
    		return 0;
    	}
        printf("Error: invalid level (should be between 1 to 7)\n");
        printf("Please enter the difficulty level between [1-7]:\n");
        result = scanf("%9s", maxDepth);
    }
    getc(stdin);
    int x = maxDepth[0] - '0';
    return x;
}

int spFiarGameGetLastMovePlayed(SPFiarGame* src) {
	if (!src || spArrayListIsEmpty(src->history)) {
		return -1;
	}
	return spArrayListGetFirst(src->history);
}

int spFiarGameSuggestMove(SPFiarGame* game, unsigned int maxDepth) {
	int suggestedMove = spMinimaxSuggestMove(game, maxDepth);
	if (suggestedMove == -1) {
		printf("Error: spMinimaxSuggestMove has failed");
		spFiarGameDestroy(game);
		return 0;
	}
	printf("Suggested move: drop a disc to column %d\n", suggestedMove + 1);
	return 1;
}

void spFiarGameUndoMove(SPFiarGame* game, char winner) {
	if (spArrayListIsEmpty(game->history)) {
		printf("Error: cannot undo previous move!\n");
	}
	else {
		if (winner != SP_FIAR_GAME_PLAYER_1_SYMBOL) {
            printf("Remove disc: remove computer's disc at column %d\n", spFiarGameGetLastMovePlayed(game) + 1);
            spFiarGameUndoPrevMove(game);
		}
        printf("Remove disc: remove user's disc at column %d\n", spFiarGameGetLastMovePlayed(game) + 1);
        spFiarGameUndoPrevMove(game);
        spFiarGamePrintBoard(game);
        printf("Please make the next move:\n");
	}
}

int spFiarGameAddDisc(SPFiarGame* game, SPCommand command, unsigned int maxDepth) {
	if (!command.validArg) {
        printf("Error: invalid command\n");
        return -1;
	}
	//char* str_command[10];
	//itoa(command.arg,str_command,10);
	if (command.arg < 1 || command.arg > SP_FIAR_GAME_N_COLUMNS) {
    	printf("Error: column number must be in range 1-7\n");
    	return -1;
	}
	int col = command.arg - 1;
	if (!spFiarGameIsValidMove(game, col)) {
		printf("Error: column %d is full\n", command.arg);
		return -1;
	}
	spFiarGameSetMove(game, col);
	char winner = spFiarCheckWinner(game);
	if (winner) {
		return 1;
	}
	int computerCol = spMinimaxSuggestMove(game, maxDepth);
	if (computerCol == -1) {
    	printf("Error: spMinimaxSuggestMove has failed");
    	spFiarGameDestroy(game);
    	return 0;
	}
    printf("Computer move: add disc to column %d\n", computerCol + 1);
    spFiarGameSetMove(game, computerCol);
    spFiarGamePrintBoard(game);
    return 1;
}

int spFiarGameRestart(SPFiarGame** game) {
	printf("Game restarted!\n");
	spFiarGameDestroy(*game);
	*game = (spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE));
	int maxDepth = spGetDifficulty();
	if (!maxDepth) {
		printf("Exiting...\n");
		spFiarGameDestroy(*game);
		return maxDepth;
	}
	spFiarGamePrintBoard(*game);
	printf("Please make the next move:\n");
	return maxDepth;
}

int spRunGame(SPFiarGame* game, int maxDepth) {
	int success;
	char winner = 0;
	char input[MAXIMUM_COMMAND_LENGTH + 1];
	while (1) {
		fgets(input, MAXIMUM_COMMAND_LENGTH, stdin);
		SPCommand command = spParserPraseLine(input);
		if (winner && (command.cmd == SP_ADD_DISC || command.cmd == SP_SUGGEST_MOVE)) {
			printf("Error: the game is over\n");
		}
		if (command.cmd == SP_INVALID_LINE) {
			printf("Error: invalid command\n");
		}
		if (command.cmd == SP_SUGGEST_MOVE && !winner) {
			success = spFiarGameSuggestMove(game, maxDepth);
			if (!success) {
				return -1;
			}
		}
		if (command.cmd == SP_UNDO_MOVE) {
			spFiarGameUndoMove(game, winner);
		}
		if (command.cmd == SP_ADD_DISC && !winner) {
			success = spFiarGameAddDisc(game, command, maxDepth);
			if (!success) {
				return -1;
			}
			if (success == 1) {
				winner = spFiarCheckWinner(game);
				if (!winner) {
					printf("Please make the next move:\n");
				}
				else {
					if (winner == SP_FIAR_GAME_TIE_SYMBOL) {
						printf("Game over: it's a tie\nPlease enter 'quit' to exit or 'restart' to start a new game!\n");
					}
					else {
						printf("Game over: %s\nPlease enter 'quit' to exit or 'restart' to start a new game!\n", winner == SP_FIAR_GAME_PLAYER_1_SYMBOL ? "you win" : "computer wins");
					}
				}
			}
		}
		if (command.cmd == SP_QUIT) {
			printf("Exiting...\n");
			spFiarGameDestroy(game);
			return 0;
		}
		if (command.cmd == SP_RESTART) {
			maxDepth = spFiarGameRestart(&game);
			if (!maxDepth) {
				return 0;
			}
			winner = 0;
		}
	}
	spFiarGameDestroy(game);
	return 1;
}
