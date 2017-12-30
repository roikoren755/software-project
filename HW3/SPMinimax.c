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
	if (!currentGame || maxDepth <= 0) {
		return -1;
	}
	Minimax_Node* root = spMinimaxCreateTree(currentGame, maxDepth);
	if (!root) {
		return -1;
	}
	int next_move = spGetBestScoreIndex(root);
	spDestroyMinimaxNode(root);
	return next_move;
}
