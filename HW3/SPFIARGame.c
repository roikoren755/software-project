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
	if (historySize <= 0) { // historySize is illegal
		return 0;
	}
	SPFiarGame* game = malloc(sizeof(SPFiarGame));
	if (!game) { // malloc failed
		return 0;
	}
	game->currentPlayer = SP_FIAR_GAME_PLAYER_1_SYMBOL; // set first player
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			game->gameBoard[i][j] = SP_FIAR_GAME_EMPTY_ENTRY; // set board to empty
			if (!i) { // just once (for i == 0)
				game->tops[j] = 0; // set tops to empty
			}
		}
	}
	game->history = spArrayListCreate(historySize);
	if (!game->history) { // failed creating spArrayList for history
		free(game);
		return 0;
	}
	return game;
}

SPFiarGame* spFiarGameCopy(SPFiarGame* src) {
	if (!src) { // src is NULL
		return 0;
	}
	SPFiarGame* game = malloc(sizeof(SPFiarGame));
	if (!game) { // malloc failed
		return 0;
	}
	game->currentPlayer = src->currentPlayer; // set current player
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			game->gameBoard[i][j] = src->gameBoard[i][j]; // copy board
			if (!i) { // again, just once
				game->tops[j] = src->tops[j]; // copy tops
			}
		}
	}
	game->history = spArrayListCopy(src->history);
	if (!game->history) { // failed copying history
		free(game);
		return 0;
	}
	return game;
}

void spFiarGameDestroy(SPFiarGame* src) {
	if (!src) { // src is NULL
		return;
	}
	spArrayListDestroy(src->history);
	free(src);
	return;
}

SP_FIAR_GAME_MESSAGE spFiarGameSetMove(SPFiarGame* src, int col) {
	if (!src || col < 0 || col > SP_FIAR_GAME_N_COLUMNS - 1) {
		return SP_FIAR_GAME_INVALID_ARGUMENT; // invalid argument
	}
	if (src->tops[col] == SP_FIAR_GAME_N_ROWS) {
		return SP_FIAR_GAME_INVALID_MOVE; // selected column is full
	}
	int row = src->tops[col]++; // increase tops
	if (spArrayListIsFull(src->history)) {
		spArrayListRemoveLast(src->history); // make room for new move in history, if needed
	}
	SP_ARRAY_LIST_MESSAGE message = spArrayListAddFirst(src->history, col); // add move to history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	src->gameBoard[row][col] = src->currentPlayer; // add disc to board
	src->currentPlayer = // switch current player
			src->currentPlayer == SP_FIAR_GAME_PLAYER_1_SYMBOL ?
					SP_FIAR_GAME_PLAYER_2_SYMBOL : SP_FIAR_GAME_PLAYER_1_SYMBOL;
	return SP_FIAR_GAME_SUCCESS;
}

bool spFiarGameIsValidMove(SPFiarGame* src, int col) {
	return (src && // src isn't NULL,
			col >= 0 && col < SP_FIAR_GAME_N_COLUMNS && // col is valid,
			src->tops[col] < SP_FIAR_GAME_N_ROWS); // and there is room in selected column
}

SP_FIAR_GAME_MESSAGE spFiarGameUndoPrevMove(SPFiarGame* src) {
	if (!src) { // src is NULL
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	if (spArrayListSize(src->history) == 0) { // no move to undo
		return SP_FIAR_GAME_NO_HISTORY;
	}
	int col = spArrayListGetFirst(src->history); // get last move made
	SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(src->history); // remove move from history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	int row = src->tops[col]; // get row to remove disc from
	src->tops[col]--; // decrease top at column
	src->gameBoard[row - 1][col] = SP_FIAR_GAME_EMPTY_ENTRY; // clear disc from board
	src->currentPlayer = // switch current player
			src->currentPlayer == SP_FIAR_GAME_PLAYER_1_SYMBOL ?
					SP_FIAR_GAME_PLAYER_2_SYMBOL : SP_FIAR_GAME_PLAYER_1_SYMBOL;
	return SP_FIAR_GAME_SUCCESS;
}

SP_FIAR_GAME_MESSAGE spFiarGamePrintBoard(SPFiarGame* src) {
	if (!src) { // src is NULL
		return SP_FIAR_GAME_INVALID_ARGUMENT;
	}
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) { // print board itself
		printf(PRINT_FORMAT_SIDE); // sides of board
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			printf(" %c", src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1)][j]); // print each disc, separated by space
		}
		printf(" %s\n", PRINT_FORMAT_SIDE); // another space, and side of board
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS * 2 + 3; i++) {
		printf(PRINT_FORMAT_BOTTOM); // print bottom of board
	}
	printf("\n  "); // new line and leading spaces
	for (int i = 1; i <= SP_FIAR_GAME_N_COLUMNS; i++) {
		printf("%d ", i); // print column numbers
	}
	printf(" \n"); // final new line and trailing spaces
	return SP_FIAR_GAME_SUCCESS;
}

char spFiarGameGetCurrentPlayer(SPFiarGame* src) {
	if (!src) { // src is NULL
		return SP_FIAR_GAME_EMPTY_ENTRY;
	}
	return src->currentPlayer;
}

char spFiarCheckWinner(SPFiarGame* src) {
	if (!src) { // src is NULL
		return NULL_CHAR;
	}
	char symbol;
	for (int i = 0; i < SP_FIAR_GAME_N_ROWS; i++) {
		for (int j = 0; j < SP_FIAR_GAME_N_COLUMNS; j++) {
			if (j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) { // horizontal spans
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[i][j + 1] &&
						symbol == src->gameBoard[i][j + 2] &&
						symbol == src->gameBoard[i][j + 3]) {
					return symbol;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1) { // vertical spans
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY &&
						symbol == src->gameBoard[i + 1][j] &&
						symbol == src->gameBoard[i + 2][j] &&
						symbol == src->gameBoard[i + 3][j]) {
					return symbol;
				}
			}
			if (i < SP_FIAR_GAME_N_ROWS - SP_FIAR_GAME_SPAN + 1 && // diagonal spans
					j < SP_FIAR_GAME_N_COLUMNS - SP_FIAR_GAME_SPAN + 1) {
				symbol = src->gameBoard[i][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY && // bottom left to top right
						symbol == src->gameBoard[i + 1][j + 1] &&
						symbol == src->gameBoard[i + 2][j + 2] &&
						symbol == src->gameBoard[i + 3][j + 3]) {
					return symbol;
				}
				symbol = src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 1)][j];
				if (symbol != SP_FIAR_GAME_EMPTY_ENTRY && // top left to bottom right
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 2)][j + 1] &&
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 3)][j + 2] &&
						symbol == src->gameBoard[SP_FIAR_GAME_N_ROWS - (i + 4)][j + 3]) {
					return symbol;
				}
			}
		}
	}
	for (int i = 0; i < SP_FIAR_GAME_N_COLUMNS; i++) {
		if (src->tops[i] != SP_FIAR_GAME_N_ROWS) { // check if there is any room left
			return NULL_CHAR;
		}
	}
	return SP_FIAR_GAME_TIE_SYMBOL; // if not, it's a tie!
}
