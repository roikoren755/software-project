/*
 * SPMinimaxNode.c
 *
 *  Created on: Dec 25, 2017
 *      Author: Roi
 */
#include <limits.h>
#include <stdio.h>
#include "SPFIARGame.h"
#include "SPMinimaxNode.h"


MM_Node* createNode(SPFiarGame* game,int height, bool turn,bool valid){
	if (!game){return NULL;}

	MM_Node *node = malloc(sizeof(MM_Node));
	if (!node){return NULL;}

	MM_Node* childs = malloc(Node_Childs_Num*sizeof(MM_Node));
	if (!node){
		free(node);
		return NULL;
	}

	node->game = game;
	node->childs = childs;
	node->height = height;
	node->valid = valid;
	node->turn = turn;
	return node;
}


void destroyNode(MM_Node* node){
	free(node->childs);
	spFiarGameDestroy(node->game);
	free(node);
}




int spMaxIndex(MM_Node* root){
	int maxMove = INT_MIN;
	int index = 0;
	for(int i=0;i<Node_Childs_Num;i++){
		if(!root->childs[i].valid){
			if(index==i){index++;}
			break;}
		int score = spEvalnode(root->childs[i]);
		if(score>maxMove){index = i;}
	}
	return index;
}

int spEvalnode(MM_Node* node){
	if (node->height==0){
		return scoreBoard(node->game);
	}

	if (node->turn){return spMaxScore(node->childs);}
	else {return spMinScore(node->childs);}

}

int spMaxScore(MM_Node* childs){
	int maxMove = INT_MIN;
	for(int i=0;i<Node_Childs_Num;i++){
			if(!childs[i].valid){break;}
			int score = spEvalnode(childs[i]);
			if(score>maxMove){maxMove = score;}
		}
	return maxMove;
}

int spMinScore(MM_Node* childs){
	int minMove = INT_MAX;
		for(int i=0;i<Node_Childs_Num;i++){
				if(!childs[i].valid){break;}
				int score = spEvalnode(childs[i]);
				if(score<minMove){minMove = score;}
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



