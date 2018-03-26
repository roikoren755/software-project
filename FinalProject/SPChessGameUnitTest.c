#include "unit_test_util.h"
#include "SPChessGame.h"
#include <string.h>

static int spChessGameBasicTest() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	spChessGameDestroy(game);
	return 1;
}

char spChessGameSetPosition2(int row, int column) {
	char ret = 1 << 3;
	ret |= row;
	ret <<= 3;
	ret |= column;
	return ret;
}

int spChessGameSetMoveCoordinates(int startingRow, int startingColumn, int endingRow, int endingColumn) {
	int move = spChessGameSetPosition2(startingRow, startingColumn) << 8;
	move |= spChessGameSetPosition2(endingRow, endingColumn);
	return move;
}

static int spChessGameTestIsValidMove() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	int move = spChessGameSetMoveCoordinates(0, 0, 0, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	move = spChessGameSetMoveCoordinates(1, 0, 3, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_NO_PIECE_IN_POSITION);
	move = spChessGameSetMoveCoordinates(6, 0, 4, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	spChessGameDestroy(game);
	return 1;
}

static int spChessGameTestGetMoves() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	char position = spChessGameSetPosition2(0, 0);
	SPArrayList* possibleMoves = spChessGameGetMoves(game, position);
	ASSERT_TRUE(possibleMoves);
	ASSERT_TRUE(spArrayListIsEmpty(possibleMoves));
	spArrayListDestroy(possibleMoves);
	position = spChessGameSetPosition2(6, 0);
	possibleMoves = spChessGameGetMoves(game, position);
	ASSERT_TRUE(possibleMoves);
	ASSERT_TRUE(spArrayListSize(possibleMoves) == 2);
	spArrayListDestroy(possibleMoves);
	spChessGameDestroy(game);
	return 1;
}

static int spChessGameTestCopy() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	SPChessGame* copy = spChessGameCopy(game);
	ASSERT_TRUE(copy);
	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i][j]);
			if (j < 4) {
				ASSERT_TRUE(game->locations[i + j * N_COLUMNS] == copy->locations[i + j * N_COLUMNS]);
			}
		}
	}
	spChessGameDestroy(game);
	spChessGameDestroy(copy);
	return 1;
}

static int spChessGameTestSetMove() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	SPChessGame* copy = spChessGameCopy(game);
	ASSERT_TRUE(copy);
	int move = spChessGameSetMoveCoordinates(0, 0, 0, 0);
	ASSERT_TRUE(spChessGameSetMove(game, move) == SP_CHESS_GAME_INVALID_ARGUMENT);for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i][j]);
			if (j < 4) {
				ASSERT_TRUE(game->locations[i + j * N_COLUMNS] == copy->locations[i + j * N_COLUMNS]);
			}
		}
	}
	ASSERT_TRUE(game->currentPlayer == copy->currentPlayer);
	move = spChessGameSetMoveCoordinates(6, 0, 5, 0);
	ASSERT_TRUE(spChessGameSetMove(game, move) == SP_CHESS_GAME_SUCCESS);
	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			if ((i != 6 && i != 5) || j != 0) {
				ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i][j]);
			}
			else if (i == 5) {
				ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i + 1][j]);
			}
			else {
				ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i - 1][j]);
			}

			if (j == 2 && i == 0) {
				ASSERT_TRUE(game->locations[i + j * N_COLUMNS] == spChessGameSetPosition2(5, 0));
				ASSERT_TRUE(copy->locations[i + j * N_COLUMNS] == spChessGameSetPosition2(6, 0));
			}
			else if (j < 4) {
				ASSERT_TRUE(game->locations[i + j * N_COLUMNS] == copy->locations[i + j * N_COLUMNS]);
			}
		}
	}
	ASSERT_TRUE(game->currentPlayer != copy->currentPlayer);
	spChessGameDestroy(game);
	spChessGameDestroy(copy);
	return 1;
}

static int spChessGameTestUndoMove() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	SPChessGame* copy = spChessGameCopy(game);
	ASSERT_TRUE(copy);
	int move = spChessGameSetMoveCoordinates(6, 0, 5, 0);
	ASSERT_TRUE(spChessGameSetMove(game, move) == SP_CHESS_GAME_SUCCESS);
	ASSERT_TRUE(spChessGameUndoMove(game) == SP_CHESS_GAME_SUCCESS);
	ASSERT_TRUE(spChessGameUndoMove(game) == SP_CHESS_GAME_NO_HISTORY);
	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			ASSERT_TRUE(game->gameBoard[i][j] == copy->gameBoard[i][j]);
			if (j < 4) {
				ASSERT_TRUE(game->locations[i + j * N_COLUMNS] == copy->locations[i + j * N_COLUMNS]);
			}
		}
	}
	spChessGameDestroy(game);
	spChessGameDestroy(copy);
	return 1;
}

char spChessGameGetDestinationPositionFromMove2(unsigned int move) {
    move >>= 8;
    move <<= 24;
    move >>= 24;
    return (char) move; // Get 2nd byte from the right
}

char spChessGameGetCurrentPositionFromMove2(unsigned int move) {
    move >>= 16;
    move <<= 24;
    move >>= 24;
    return (char) move; // Get 3rd Byte from the left
}

unsigned int spChessGameGetColumnFromPosition2(unsigned char position) {
    position <<= 5;
    position >>= 5;
    return (unsigned int) position; // Get 3 rightmost bits
}

unsigned int spChessGameGetRowFromPosition2(unsigned char position) {
	position <<= 2;
    position >>= 5;
    return (unsigned int) position; // Get next 3 bits
}

int main() {
	RUN_TEST(spChessGameBasicTest);
	RUN_TEST(spChessGameTestIsValidMove);
	RUN_TEST(spChessGameTestGetMoves);
	RUN_TEST(spChessGameTestCopy);
	RUN_TEST(spChessGameTestSetMove);
	RUN_TEST(spChessGameTestUndoMove);
	return 0;
}
