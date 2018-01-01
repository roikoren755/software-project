//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"
#include "SPBufferset.h"



int main() {
	SP_BUFF_SET();
    int maxDepth = spGetDifficulty();
    if (!maxDepth) {
    	printf("Exiting...\n");
    	return 0;
    }
    SPFiarGame* game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE);
    if (!game) {
        printf("Error: spFiarGameCreate has failed");
        return -1;
    }
    char input[MAXIMUM_COMMAND_LENGTH + 1];
    spFiarGamePrintBoard(game);
    char winner = 0;
    int success;
    printf("Please make the next move:\n");
    return spRunGame(input,game,winner,success,maxDepth);

}
