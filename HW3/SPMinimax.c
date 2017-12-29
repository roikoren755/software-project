/*
 * SPMinimax.c
 *
 *  Created on: 26 Dec 2017
 *      Author: user
 */
#include <limits.h>
#include <stdio.h>
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
	int next_move = spGetMaximumScoreIndex(root);
	spDestroyMinimaxNode(root);
	return next_move;
}

Minimax_Node* spMinimaxCreateTree(SPFiarGame* currentGame, unsigned int maxDepth){
	SPFiarGame* currentGameCopy = spFiarGameCopy(currentGame);
	if (!currentGameCopy) {
		return NULL;
	}
	Minimax_Node* root = spCreateMinimaxNode(currentGameCopy, 0);
	if (!root) {
		return NULL;
	}
	if (!spCreateMinimaxNodeChildren(root, maxDepth)) {
		spDestroyMinimaxNode(root);
		return NULL;
	}
	return root;
}
