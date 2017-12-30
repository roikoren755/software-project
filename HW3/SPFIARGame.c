/*
 * SPFIARGame.c
 *
 *  Created on: Dec 22, 2017
 *      Author: Roi
 */
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARGame.h"
#include "SPArrayList.h"

#define PRINT_FORMAT_SIDE "|"
#define PRINT_FORMAT_BOTTOM "-"
#define NULL_CHAR '\0'

SPFiarGame* spFiarGameCreate(int historySize) {
	if (historySize <= 0) {
		return 0;
	}
	SPFiarGame* game = malloc(sizeof(SPFiarGame));
	if (!game) {
		return 0;
	}
	game->currentPlayer = SP_FIAR_GAME_PLAYER_1_SYMBOL;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			game->gameBoard[i][j] = SP_FIAR_GAME_EMPTY_ENTRY;
		}
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS; i++) {
		game->tops[i] = 0;
	}
	game->history = spArrayListCreate(historySize);
	if (!game->history) {
		free(game);
		return 0;
	}
	return game;
}

SPFiarGame* spFiarGameCopy(SPFiarGame* src) {
	if (!src) {
		return 0;
	}
	int historySize = spArrayListMaxCapacity(src->history);
	SPFiarGame* ret = spFiarGameCreate(historySize);
	if (!ret) {
		return 0;
	}
	ret->currentPlayer = src->currentPlayer;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			ret->gameBoard[i][j] = src->gameBoard[i][j];
		}
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS; i++) {
		ret->tops[i] = src->tops[i];
	}
	ret->history = spArrayListCopy(src->history);
	if (!ret->history) {
		free(ret);
		return 0;
	}
	return ret;
}

void spFiarGameDestroy(SPFiarGame* src) {
	if (!src) {
		return;
	}
	free(src->history);
	free(src);
	return;
}

SP_FIAR_GAME_MESSAGE spFiarGameSetMove(SPFiarGame* src, int col) {
	if (!src || col < 0 || col > SP_FIAR_GAME_N_COLUMNS - 1) {
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	if (src->tops[col] == SP_FIAR_GAME_N_ROWS) {
		return SP_FIAR_GAME_INVALID_MOVE;
	}
	int row = src->tops[col]++;
	if (spArrayListIsFull(src->history)) {
		spArrayListRemoveLast(src->history);
	}
	SP_ARRAY_LIST_MESSAGE message = spArrayListAddFirst(src->history, col);
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) {
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	src->gameBoard[row][col] = src->currentPlayer;
	src->currentPlayer = src->currentPlayer == SP_FIAR_GAME_PLAYER_1_SYMBOL ? SP_FIAR_GAME_PLAYER_2_SYMBOL : SP_FIAR_GAME_PLAYER_1_SYMBOL;
	return SP_FIAR_GAME_SUCCESS;
}

bool spFiarGameIsValidMove(SPFiarGame* src, int col) {
	return (src && src->tops[col] < SP_FIAR_GAME_N_ROWS);
}

SP_FIAR_GAME_MESSAGE spFiarGameUndoPrevMove(SPFiarGame* src) {
	if (!src) {
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	if (spArrayListSize(src->history) == 0) {
		return SP_FIAR_GAME_NO_HISTORY;
	}
	int col = spArrayListGetFirst(src->history);
	SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(src->history);
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) {
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	int row = src->tops[col];
	src->tops[col]--;
	src->gameBoard[row - 1][col] = SP_FIAR_GAME_EMPTY_ENTRY;
	src->currentPlayer = src->currentPlayer == SP_FIAR_GAME_PLAYER_1_SYMBOL ? SP_FIAR_GAME_PLAYER_2_SYMBOL : SP_FIAR_GAME_PLAYER_1_SYMBOL;
	return SP_FIAR_GAME_SUCCESS;
}

SP_FIAR_GAME_MESSAGE spFiarGamePrintBoard(SPFiarGame* src) {
	if (!src) {
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		printf(PRINT_FORMAT_SIDE);
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			printf(" %c", src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1)][j]);
		}
		printf(" %s\n", PRINT_FORMAT_SIDE);
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS * 2 + 3; i++) {
		printf(PRINT_FORMAT_BOTTOM);
	}
	printf("\n  ");
	for (int i = 1; i <= SP_FIAR_GAME_N_COLUMNS; i++) {
		printf("%d ", i);
	}
	printf(" \n");
	return SP_FIAR_GAME_SUCCESS;
}

char spFiarGameGetCurrentPlayer(SPFiarGame* src) {
	if (!src) {
		return SP_FIAR_GAME_EMPTY_ENTRY;
	}
	return src->currentPlayer;
}

char spFiarCheckWinner(SPFiarGame* src) {
	if (!src) {
		return NULL_CHAR;
	}
	char symbol;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			if (j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[i][j + 1] &&
						symbol == src->gameBoard[i][j + 2] &&
						symbol == src->gameBoard[i][j + 3]) {
					return symbol;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1) {
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[i + 1][j] &&
						symbol == src->gameBoard[i + 2][j] &&
						symbol == src->gameBoard[i + 3][j]) {
					return symbol;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1 &&
					j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[i + 1][j + 1] &&
						symbol == src->gameBoard[i + 2][j + 2] &&
						symbol == src->gameBoard[i + 3][j + 3]) {
					return symbol;
				}
				symbol = src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1)][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 2)][j + 1] &&
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 3)][j + 2] &&
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 4)][j + 3]) {
					return symbol;
				}
			}
		}
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS; i++) {
		if (src->tops[i] != SP_FIAR_GAME_N_ROWS) {
			return NULL_CHAR;
		}
	}
	return SP_FIAR_GAME_TIE_SYMBOL;
}
