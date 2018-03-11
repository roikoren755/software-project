/*
 * SPMinimax.c
 *
 *  Created on: 26 Dec 2017
 *      Author: user
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARGame.h"
#include "SPMinimaxNode.h"
#include "SPMinimax.h"


int spMinimaxSuggestMove(SPFiarGame* currentGame, unsigned int maxDepth){
	if (!currentGame || maxDepth <= 0) { // currentGame is NULL, or maxDepth is invalid
		return -1;
	}
	Minimax_Node* root = spMinimaxCreateTree(currentGame, maxDepth); // create Minimax tree
	if (!root) {
		return -1;
	}
	int next_move = spGetBestScoreIndex(root); // get suggested move
	spDestroyMinimaxNode(root); // free all memory used by Minimax tree
	return next_move;
}
