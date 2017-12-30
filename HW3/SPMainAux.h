#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

<<<<<<< HEAD
#include "SPFIARGame.h"
=======
#include "SPFiarGame.h"
#include "SPFIARParser.h"
>>>>>>> a60b663113bacc11dd9608100c508e30db50976c

//put auxiliary functions and constants used by the main function here.
int spGetDifficulty();

int spFiarGameGetLastMovePlayed(SPFiarGame* src);

int spFiarGameSuggestMove(SPFiarGame* game, unsigned int maxDepth);

void spFiarGameUndoMove(SPFiarGame* game, char winner);

int spFiarGameAddDisc(SPFiarGame* game, SPCommand command, unsigned int maxDepth);

#endif
