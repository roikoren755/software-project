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

#define ERROR -1


int spMinimaxSuggestMove(SPFiarGame* currentGame, unsigned int maxDepth){
	if (!currentGame || maxDepth <= 0) {
		return ERROR;
	}
	MM_Node* root = spMinimaxCreateTree(currentGame, maxDepth);
	if (!root) {
		return ERROR;
	}
	int next_move = spGetMaximumScoreIndex(root);
	spDestroyMinimaxTree(root);
	return next_move;
}

MM_Node* spMinimaxCreateTree(SPFiarGame* currentGame, unsigned int maxDepth){
	SPFiarGame* currentGameCopy = spFiarGameCopy(currentGame);
	if (!currentGameCopy) {
		return NULL;
	}
	MM_Node* root = createMinimaxNode(currentGameCopy, maxDepth, true, true);
	int success = spCreateMinimaxNodeChildren(root);
	if (!success) {
		return NULL;
	}
	return root;
}

int spCreateMinimaxNodeChildren(MM_Node* parent){
	if (!parent) {
		return 0;
	}
	if (!parent->height) {
		return 1;
	}
	int height = parent->height;
	SPFiarGame* game = parent->game;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++){
		SPFiarGame* nextGame = spFiarGameCopy(game);
		if (!nextGame) {
			return 0;
		}
		if (!spFiarGameIsValidMove(nextGame, i)) {
			MM_Node* child = createMinimaxNode(nextGame, 0, true, false);
			if (!child) {
				return 0;
			}
			parent->children[i] = child;
			continue;
		}
		spFiarGameSetMove(nextGame,i);
		if(spFiarCheckWinner(nextGame)) {
			MM_Node* child = createMinimaxNode(nextGame, 0, true, true);
			if (!child) {
				return 0;
			}
			parent->children[i] = child;
			continue;
		}

		MM_Node* child = createMinimaxNode(nextGame, height - 1, !parent->turn, true);
		if (!child) {
			return 0;
		}
		parent->children[i] = child;
		if (!spCreateMinimaxNodeChildren(child)) {
			return 0;
		}
	}
	return 1;
}

void spDestroyMinimaxTree(MM_Node* root) {
	if (!root->height) {
		destroyMinimaxNode(root);
	}
	else {
		for(int i=0; i < MAX_NODE_CHILDREN_NUM; i++) {
			spDestroyMinimaxTree(root->children[i]);
		}
	}
}
