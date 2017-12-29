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
	if (!game) {
		return NULL;
	}
	Minimax_Node *node = malloc(sizeof(Minimax_Node));
	if (!node) {
		return NULL;
	}
	for (int  i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		node->children[i] = 0;
	}
	node->game = game;
	node->depth = depth;
	return node;
}

void spDestroyMinimaxNode(Minimax_Node* node) {
	if (!node) {
		return;
	}
	spFiarGameDestroy(node->game);
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			spDestroyMinimaxNode(node->children[i]);
		}
	}
	free(node);
}

int spCreateMinimaxNodeChildren(Minimax_Node* node, unsigned int maxDepth) {
	if (!node) {
		return 0;
	}
	if(node->depth == maxDepth) {
		return 1;
	}
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (spFiarGameIsValidMove(node->game, i)) {
			SPFiarGame* childGame = spFiarGameCopy(node->game);
			if (!childGame) {
				return 0;
			}
			spFiarGameSetMove(childGame, i);
			Minimax_Node* childNode = spCreateMinimaxNode(childGame, node->depth + 1);
			if (!childNode) {
				return 0;
			}
			node->children[i] = childNode;
			if (!spFiarCheckWinner(childGame)) {
				if (!spCreateMinimaxNodeChildren(childNode, maxDepth)) {
					return 0;
				}
			}
		}
	}
	return 1;
}

int spGetMaximumScoreIndex(Minimax_Node* node) {
	if (!node) {
		return -1;
	}
	int maxScore = INT_MIN;
	int index = 0;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (!node->children[i]) {
			if (index == i) {
				index++;
			}
		}
		else {
			int score = spEvaluateMinimaxNode(node->children[i]);
			if (score > maxScore) {
				index = i;
			}
		}
	}
	return index;
}

int spEvaluateMinimaxNode(Minimax_Node* node){
	if (!node) {
		return 0;
	}
	bool leaf = true;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			leaf = false;
		}
	}
	if (!node->depth || leaf) {
		return scoreBoard(node->game);
	}
	if (node->depth % 2) {
		return spGetMaximumScore(node);
	}
	else {
		return spGetMinimumScore(node);
	}

}

int spGetMaximumScore(Minimax_Node* node){
	if (!node) {
		return INT_MAX;
	}
	int maxScore = INT_MIN;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			int score = spEvaluateMinimaxNode(node->children[i]);
			if (score > maxScore) {
				maxScore = score;
			}
		}
	}
	return maxScore;
}

int spGetMinimumScore(Minimax_Node* node){
	if (!node) {
		return INT_MIN;
	}
	int minScore = INT_MAX;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			int score = spEvaluateMinimaxNode(node->children[i]);
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
		spans[i] = 0;
	}
	int weights[] = {0, -5, -2, -1, 0, 1, 2, 5, 0};
	int score = 0;
	int player1, player2;
	char symbol;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			if (j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				player1 = player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[i][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				if (player1 != player2) {
					spans[player1 - player2 + SP_FIAR_GAME_SPAN]++;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1) {
				player1 = player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[i + k][j];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				if (player1 != player2) {
					spans[player1 - player2 + SP_FIAR_GAME_SPAN]++;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1 &&
					j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				player1 = player2 = 0;
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[i + k][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				if (player1 != player2) {
					spans[player1 - player2 + SP_FIAR_GAME_SPAN]++;
				}
				for (int k = 0; k < SP_FIAR_GAME_SPAN; k++) {
					symbol = src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1 + k)][j + k];
					if (symbol == SP_FIAR_GAME_PLAYER_1_SYMBOL) {
						player1++;
					}
					if (symbol == SP_FIAR_GAME_PLAYER_2_SYMBOL) {
						player2++;
					}
				}
				if (player1 != player2) {
					spans[player1 - player2 + SP_FIAR_GAME_SPAN]++;
				}
			}
		}
	}
	if (spans[0]) {
		return INT_MIN;
	}
	if (spans[SP_FIAR_GAME_SPAN * 2]) {
		return INT_MAX;
	}
	for (int i = 0; i < SP_FIAR_GAME_SPAN * 2 + 1; i++) {
		score += weights[i] * spans[i];
	}
	return score;
}



