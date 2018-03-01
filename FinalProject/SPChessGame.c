//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"
#include "SPArrayList.h"
#include <stdlib.h>

#define STARTING_ROW "RNBQKBNR"
#define PAWN 'M'
#define KNIGHT 'N'
#define BISHOP 'B'
#define ROOK 'R'
#define QUEEN 'Q'
#define KING 'K'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define BLANK '_'
#define SEPARATOR '-'
#define FIRST_COLUMN 'A'
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define PAWN_FOR_MOVE 1
#define KNIGHT_FOR_MOVE 2
#define BISHOP_FOR_MOVE 3
#define ROOK_FOR_MOVE 4
#define QUEEN_FOR_MOVE 5
#define KING_FOR_MOVE 6

char spChessGameGetDestPositionFromMove(int move) {
	return (char) CLEAN_EXCESS_BYTES((move >> 8));
}

char spChessGameGetCurrPositionFromMove(int move) {
	return (char) CLEAN_EXCESS_BYTES((move >> 16));
}

int spChessGameGetColumnFromPosition(char position) {
	return (int) (position << 4) >> 4;
}

int spChessGameGetRowFromPosition(char position) {
	return (int) position >> 4;
}

SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src) {
	if (!src) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	char* starting_row = STARTING_ROW;

	for (int i = 0; i < N_COLUMNS; i++) {
		src->gameBoard[1][i] = PAWN;
		src->gameBoard[6][i] = CAPITAL_TO_LOW(PAWN);
		src->gameBoard[0][i] = starting_row[i];
		src->gameBoard[7][i] = CAPITAL_TO_LOW(starting_row[i]);
		for (int j = 0; j < N_ROWS - 4; j++) {
			src->gameBoard[j + 2][i] = '\0';
		}
	}
	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		src->locations[i] = 0;
	}

	return SP_CHESS_GAME_SUCCESS;
}

SPChessGame* spChessGameCreate(int historySize) {
	if (historySize <= 0) { // historySize is illegal
		return 0;
	}
	SPChessGame* game = (SPChessGame*) malloc(sizeof(SPChessGame));
	if (!game) { // malloc failed
		return 0;
	}

	game->currentPlayer = WHITE; // set first player
	game->user_color = WHITE;
	game->difficulty = 0;
	spChessGameResetBoard(game);

	game->history = spArrayListCreate(historySize);
	if (!game->history) { // failed creating spArrayList for history
		free(game);
		return 0;
	}

	return game;
}

SPChessGame* spChessGameCopy(SPChessGame* src) {
	if (!src) {
		return 0;
	}
	SPChessGame* ret = malloc(sizeof(SPChessGame));
	if (!ret) {
		return 0;
	}
	ret->currentPlayer = src->currentPlayer;
	ret->difficulty = src->difficulty;
	ret->user_color = src->user_color;

	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			ret->gameBoard[i][j] = src->gameBoard[i][j];
		}
		for (int j = 0; j < 2; j++) {
			ret->locations[i + j] = src->locations[i + j];
		}
	}

	ret->history = spArrayListCopy(src->history);
	if (!ret->history) {
		free(ret);
		return 0;
	}

	return ret;
}

void spChessGameDestroy(SPChessGame* src) {
	if (!src) {
		return;
	}

	spArrayListDestroy(src->history);
	free(src);
}

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move) {
	move <<= 8;
	char destPosition = spChessGameGetDestPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	char captured = src->gameBoard[destRow][destColumn];
	if (spArrayListIsFull(src->history)) {
		spArrayListRemoveLast(src->history); // make room for new move in history, if needed
	}
	move <<= 8;
	if (captured) {
		if (captured == PAWN || captured == CAPITAL_TO_LOW(PAWN)) {
			move |= PAWN_FOR_MOVE;
		}
		if (captured == KNIGHT || captured == CAPITAL_TO_LOW(KNIGHT)) {
			move |= KNIGHT_FOR_MOVE;
		}
		if (captured == BISHOP || captured == CAPITAL_TO_LOW(BISHOP)) {
			move |= BISHOP_FOR_MOVE;
		}
		if (captured == QUEEN || captured == CAPITAL_TO_LOW(QUEEN)) {
			move |= QUEEN_FOR_MOVE;
		}
		if (captured == KING || captured == CAPITAL_TO_LOW(KING)) {
			move |= KING_FOR_MOVE;
		}
	}
	SP_ARRAY_LIST_MESSAGE message = spArrayListAddFirst(src->history, move); // add move to history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	for (int i = 0; i < N_COLUMNS * 2; i++) {
		if (src->locations[i] == currPosition) {
			src->locations[i] = destPosition;
		}
	}
	src->gameBoard[destRow][destColumn] = src->gameBoard[currRow][currColumn];
	src->gameBoard[currRow][currColumn] = '\0';

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameUndoPrevMove(SPChessGame* src) {
	if (!src) { // src is NULL
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	if (spArrayListSize(src->history) == 0) { // no move to undo
		return SP_CHESS_GAME_NO_HISTORY;
	}
	int move = spArrayListGetFirst(src->history); // get last move made
	SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(src->history); // remove move from history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	char captured = CLEAN_EXCESS_BYTES(move);
	char destPosition = spChessGameGetDestPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	for (int i = 0; i < N_COLUMNS * 2; i++) {
		if (src->locations[i] == destPosition) {
			src->locations[i] = currPosition;
		}
	}
	src->gameBoard[destRow][destColumn] = captured;
	src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE;
	return SP_FIAR_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* game) {
	if (!game) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	for (int i = 0; i < N_ROWS; i++) {
		printf("%d|", 8 - i);
		for (int j = 0; j < N_COLUMNS; j++) {
			char currChar = game->gameBoard[i][j];
			if (!currChar) {
				printf(" %c", BLANK);
			}
			else {
				printf(" %c",currChar);
			}
		}
		printf(" |\n  ");
	}

	for (int i = 0; i < 2 * N_COLUMNS + 1; i++) {
		printf("%c", SEPARATOR);
	}

	for (int i = 0; i < N_COLUMNS; i++) {
		printf(" %c", FIRST_COLUMN + i);
	}

	return SP_CHESS_GAME_SUCCESS;
}
