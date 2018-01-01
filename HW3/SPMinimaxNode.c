/*
 * SPMinimaxNode.c
 *
 *  Created on: Dec 25, 2017
 *      Author: Roi
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARGame.h"
#include "SPMinimaxNode.h"


Minimax_Node* spCreateMinimaxNode(SPFiarGame* game, unsigned int depth) {
	if (!game) { // game is NULL
		return NULL;
	}
	Minimax_Node *node = malloc(sizeof(Minimax_Node));
	if (!node) { // malloc failed
		return NULL;
	}
	for (int  i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		node->children[i] = 0; // set children to NULL
	}
	node->game = game;
	node->depth = depth;
	return node;
}

void spDestroyMinimaxNode(Minimax_Node* node) {
	if (!node) { // node is NULL
		return;
	}
	spFiarGameDestroy(node->game); // destroy game
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			spDestroyMinimaxNode(node->children[i]); // recursively destroy children
		}
	}
	free(node);
}

Minimax_Node* spMinimaxCreateTree(SPFiarGame* currentGame, unsigned int maxDepth){
	SPFiarGame* currentGameCopy = spFiarGameCopy(currentGame); // copy game, so as to not affect it
	if (!currentGameCopy) { // copy failed
		return NULL;
	}
	Minimax_Node* root = spCreateMinimaxNode(currentGameCopy, 0); // create root node
	if (!root) { // creation failed
		free(currentGameCopy);
		return NULL;
	}
	if (!spCreateMinimaxNodeChildren(root, maxDepth)) { // create and check success of creating node children
		spDestroyMinimaxNode(root);
		return NULL;
	}
	return root;
}

int spCreateMinimaxNodeChildren(Minimax_Node* node, unsigned int maxDepth) {
	if (!node) { // node is NULL
		return 0;
	}
	if(node->depth == maxDepth || // node is at maxDepth
			spFiarCheckWinner(node->game)) { // or node's game ended
		return 1; // no children
	}
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (spFiarGameIsValidMove(node->game, i)) { // if child should be created
			SPFiarGame* childGame = spFiarGameCopy(node->game);
			if (!childGame) { // copy game failed
				return 0;
			}
			spFiarGameSetMove(childGame, i); // set correct move
			Minimax_Node* childNode = spCreateMinimaxNode(childGame, node->depth + 1);
			if (!childNode) { // node creation failed
				free(childGame);
				return 0;
			}
			node->children[i] = childNode;
			if (!spCreateMinimaxNodeChildren(childNode, maxDepth)) {
				return 0; // creating child's children failed
			}
		}
	}
	return 1;
}

int spGetBestScoreIndex(Minimax_Node* node) {
	if (!node) { // node is NULL
		return -1;
	}
	int rootPlayer = // determine player at current level
			spFiarGameGetCurrentPlayer(node->game) ==
					SP_FIAR_GAME_PLAYER_1_SYMBOL ? 1 : 0;
	if (rootPlayer) { // player 1 wants to maximize score
		return spGetMaximumScoreIndex(node, rootPlayer);
	}
	return spGetMinimumScoreIndex(node, rootPlayer); // player 2 wants to minimize
}

int spGetMinimumScoreIndex(Minimax_Node* node, int rootPlayer) {
	if (!node) { // node is NULL
		return -1;
	}
	int minScore = INT_MAX;
	int index = 0;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (!node->children[i]) { // no child in position
			if (index == i) {
				index++; // don't return this index!
			}
		}
		else {
			int score = spEvaluateMinimaxNode(node->children[i], rootPlayer); // evaluate child
			if (score < minScore) {
				minScore = score;
				index = i;
			}
		}
	}
	return index;
}

int spGetMaximumScoreIndex(Minimax_Node* node, int rootPlayer) {
	if (!node) { // node is NULL
		return -1;
	}
	int maxScore = INT_MIN;
	int index = 0;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (!node->children[i]) { // no child in position
			if (index == i) {
				index++; // don't return this index!
			}
		}
		else {
			int score = spEvaluateMinimaxNode(node->children[i], rootPlayer); // evaluate child
			if (score > maxScore) {
				maxScore = score;
				index = i;
			}
		}
	}
	return index;
}

int spEvaluateMinimaxNode(Minimax_Node* node, int rootPlayer){
	if (!node) { // node is NULL
		return 0;
	}
	bool leaf = true;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) { // check if node has children
			leaf = false;
		}
	}
	if (leaf) { // no children
		return scoreBoard(node->game); // value of node is score of board
	}
	if ((node->depth % 2 == 0 && rootPlayer) // we're at an even level and player 1 started
			|| (node->depth % 2 == 1 && !rootPlayer)){ // or an odd level and player 2 started
		return spGetMaximumScore(node, rootPlayer); // then current player wants to maximize score
	}
	else { // odd level and player 1 started, or even level and player 2 started
		return spGetMinimumScore(node, rootPlayer); // then current player wants to minimize score
	}

}

int spGetMaximumScore(Minimax_Node* node, int rootPlayer){
	if (!node) { // node is NULL
		return INT_MAX; // level above wants minimum, so won't pick this child
	}
	int maxScore = INT_MIN;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) { // find maximum score among children
		if (node->children[i]) {
			int score = spEvaluateMinimaxNode(node->children[i], rootPlayer);
			if (score > maxScore) {
				maxScore = score;
			}
		}
	}
	return maxScore;
}

int spGetMinimumScore(Minimax_Node* node, int rootPlayer){
	if (!node) { // node is NULL
		return INT_MIN;
	}
	int minScore = INT_MAX;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) { // find minimum score among children
		if (node->children[i]) {
			int score = spEvaluateMinimaxNode(node->children[i], rootPlayer);
			if (score < minScore) {
				minScore = score;
			}
		}
	}
	return minScore;
}

int scoreBoard(SPFiarGame* src) {
	int spans[SP_FIAR_GAME_SPAN * 2 + 1];
	for (int i = 0; i < SP_FIAR_GAME_SPAN * 2 + 1; i++) {
		spans[i] = 0; // reset spans count
	}
	int weights[] = {0, -5, -2, -1, 0, 1, 2, 5, 0}; // pre-defined weights
	int score = 0;
	int player1;
	int player2;
	char symbol;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			if (j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) { // horizontal spans
				player1 = 0;
				player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[i][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				spans[player1 - player2 + SP_FIAR_GAME_SPAN]++; // 0-based index for spans array
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1) { // vertical spans
				player1 = 0;
				player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[i + k][j];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				spans[player1 - player2 + SP_FIAR_GAME_SPAN]++; // 0-based index for spans array
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1 && // diagonal spans
					j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				player1 = 0;
				player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) { // bottom left to top right
					symbol = src->gameBoard[i + k][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				spans[player1 - player2 + SP_FIAR_GAME_SPAN]++; // 0-based index for spans array
				player1 = 0;
				player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) { // top left to bottom right
					symbol = src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1 + k)][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				spans[player1 - player2 + SP_FIAR_GAME_SPAN]++; // 0-based index for spans array
			}
		}
	}
	if (spans[0]) { // spans with 4 player 2 discs
		return INT_MIN;
	}
	if (spans[SP_FIAR_GAME_SPAN * 2]) { // spans with 4 player 1 discs
		return INT_MAX;
	}
	for (int i = 0; i < SP_FIAR_GAME_SPAN * 2 + 1; i++) {
		score += weights[i] * spans[i]; // multiply numbers of spans by weights
	}
	return score;
}
