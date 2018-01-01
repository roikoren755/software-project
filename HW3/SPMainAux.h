#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "SPFIARGame.h"
#include "SPFIARParser.h"

#define UNDO_MOVES_POSSIBLE 10
#define MAXIMUM_COMMAND_LENGTH 1024

//put auxiliary functions and constants used by the main function here.
int spGetDifficulty();

int spFiarGameGetLastMovePlayed(SPFiarGame* src);

int spFiarGameSuggestMove(SPFiarGame* game, unsigned int maxDepth);

void spFiarGameUndoMove(SPFiarGame* game, char winner);

int spFiarGameAddDisc(SPFiarGame* game, SPCommand command, unsigned int maxDepth);

int spFiarGameRestart(SPFiarGame** game);

#endif
