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


MM_Node* createMinimaxNode(SPFiarGame* game, int height, bool turn, bool valid) {
	if (!game) {
		return NULL;
	}
	MM_Node *node = malloc(sizeof(MM_Node));
	if (!node) {
		return NULL;
	}
	for (int  i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		node->children[i] = 0;
	}
	node->game = game;
	node->height = height;
	node->valid = valid;
	node->turn = turn;
	return node;
}

void destroyMinimaxNode(MM_Node* node) {
	if (!node) {
		return;
	}
	spFiarGameDestroy(node->game);
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]) {
			destroyMinimaxNode(node->children[i]);
		}
	}
	free(node);
}

int spGetMaximumScoreIndex(MM_Node* root) {
	int maxMove = INT_MIN;
	int index = 0;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (!root->children[i]->valid) {
			if (index == i) {
				index++;
			}
		}
		else {
			int score = spEvaluateMinimaxNode(root->children[i]);
			if (score > maxMove) {
				index = i;
			}
		}
	}
	return index;
}

int spEvaluateMinimaxNode(MM_Node* node){
	bool leaf = true;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (node->children[i]->valid) {
			leaf = false;
		}
	}
	if (!node->height || leaf) {
		return scoreBoard(node->game);
	}
	if (node->turn) {
		return spGetMaximumScore(node);
	}
	else {
		return spGetMinimumScore(node);
	}

}

int spGetMaximumScore(MM_Node* node){
	int maxMove = INT_MIN;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if (!node->children[i]->valid) {
			continue;
		}
		int score = spEvaluateMinimaxNode(node->children[i]);
		if (score > maxMove) {
			maxMove = score;
		}
	}
	return maxMove;
}

int spGetMinimumScore(MM_Node* node){
	int minMove = INT_MAX;
	for (int i = 0; i < MAX_NODE_CHILDREN_NUM; i++) {
		if(!node->children[i]->valid) {
			continue;
		}
		int score = spEvaluateMinimaxNode(node->children[i]);
		if (score<minMove) {
			minMove = score;
		}
	}
	return minMove;
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



