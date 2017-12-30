//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
<<<<<<< HEAD
#include "SPFIARGame.h"
=======
#include "SPFiarGame.h"
#include "SPFIARParser.h"
#include "SPMinimax.h"
>>>>>>> a60b663113bacc11dd9608100c508e30db50976c

int spGetDifficulty() {
    printf("Please enter the difficulty level between [1-7]:\n");
    char maxDepth[10];
    int result = scanf("%9s", maxDepth);
    while (result == EOF || result == 0 || maxDepth[0] < '1' || maxDepth[0] > '7' || maxDepth[1]!='\0') {
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
        return 1;
	}
	if (command.arg < 1 || command.arg > SP_FIAR_GAME_N_COLUMNS) {
    	printf("Error: column number must be in range 1-7\n");
    	return 1;
	}
	int col = command.arg - 1;
	if (!spFiarGameIsValidMove(game, col)) {
		printf("Error: column %d is full\n", command.arg);
		return 1;
	}
	spFiarGameSetMove(game, col);
	int computerCol = spMinimaxSuggestMove(game, maxDepth);
	if (computerCol == -1) {
    	printf("Error: spMinimaxSuggestMove has failed");
    	spFiarGameDestroy(game);
    	return 0;
	}
    printf("Computer move: add disc to column %d\n", computerCol + 1);
    spFiarGameSetMove(game, computerCol);
    return 1;
}
