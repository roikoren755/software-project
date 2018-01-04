//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"

int main() {
    int maxDepth = spGetDifficulty(); // get initial difficulty
    if (!maxDepth) { // user entered "quit"
    	printf("Exiting...\n");
    	return 0;
    }
    SPFiarGame* game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE); // create game
    if (!game) { // game creation failed
        printf("Error: spFiarGameCreate has failed");
        return -1;
    }
    spFiarGamePrintBoard(game); // first print
    printf("Please make the next move:\n");
    return spRunGame(game, maxDepth); // run game!
}
