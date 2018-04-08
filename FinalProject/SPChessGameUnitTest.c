#include "unit_test_util.h"
#include "SPChessGame.h"
#include <string.h>

static int spChessGameBasicTest() {
	SPChessGame* game = spChessGameCreate();
	ASSERT_TRUE(game);
	spChessGameDestroy(game);
	return 1;
}

unsigned char spChessGameSetPosition2(unsigned int row, unsigned int column) {
	unsigned char ret = 1 << 3;
	ret |= row;
	ret <<= 3;
	ret |= column;
	return ret;
}

unsigned int spChessGameSetMoveCoordinates(unsigned int startingRow, unsigned int startingColumn, unsigned int endingRow, unsigned int endingColumn) {
	unsigned int move = spChessGameSetPosition2(startingRow, startingColumn) << 8;
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
	move = spChessGameSetMoveCoordinates(7, 1, 5, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			game->gameBoard[i][j] = '\0';
			if (j < 4) {
				game->locations[i + j * N_COLUMNS] = 0;
			}
		}
	}
	game->gameBoard[7][4] = 'k';
	game->locations[KING_LOC(WHITE)] = spChessGameSetPosition2(7, 4);
	game->gameBoard[6][0] = 'm';
	game->locations[16] = spChessGameSetPosition2(6, 0);
	move = spChessGameSetMoveCoordinates(6, 0, 5, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	move = spChessGameSetMoveCoordinates(6, 0, 4, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	move = spChessGameSetMoveCoordinates(6, 0, 3, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	move = spChessGameSetMoveCoordinates(6, 0, 6, 1);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	move = spChessGameSetMoveCoordinates(6, 0, 5, 1);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	game->gameBoard[5][1] = 'M';
	game->locations[8] = spChessGameSetPosition2(5, 1);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	game->gameBoard[5][1] = 'm';
	game->locations[8] = 0;
	game->locations[17] = spChessGameSetPosition2(5, 1);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	move = spChessGameSetMoveCoordinates(6, 0, 7, 0);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE);
	game->gameBoard[5][1] = game->gameBoard[6][0] = '\0';
	game->locations[16] = game->locations[17] = 0;
	game->gameBoard[6][4] = 'b';
	game->locations[LEFT_BISHOP_LOC(WHITE)] = spChessGameSetPosition2(6, 4);
    move = spChessGameSetMoveCoordinates(6, 4, 5, 5);
    ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS);
	game->gameBoard[4][4] = 'Q';
	game->locations[QUEEN_LOC(BLACK)] = spChessGameSetPosition2(4, 4);
	ASSERT_TRUE(spChessGameIsValidMove(game, move) == SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED);
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
	position = spChessGameSetPosition2(4, 4);
	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		if (i % 2 == 0) {
			game->gameBoard[4][4] = STARTING_ROW[i / 2];
			game->currentPlayer = BLACK;
		}
		else {
			game->gameBoard[4][4] = CAPITAL_TO_LOW(STARTING_ROW[i / 2]);
			game->currentPlayer = WHITE;
		}
		possibleMoves = spChessGameGetMoves(game, position);
		ASSERT_TRUE(possibleMoves);
		switch (i / 2) {
			case RIGHT_ROOK_LOC(BLACK):
			case LEFT_ROOK_LOC(BLACK):
				ASSERT_TRUE(spArrayListSize(possibleMoves) == 11);
				break;
			case RIGHT_KNIGHT_LOC(BLACK):
			case LEFT_KNIGHT_LOC(BLACK):
				if (game->currentPlayer) {
					ASSERT_TRUE(spArrayListSize(possibleMoves) == 6);
				}
				else {
					ASSERT_TRUE(spArrayListSize(possibleMoves) == 8);
				}
				break;
			case RIGHT_BISHOP_LOC(BLACK):
			case LEFT_BISHOP_LOC(BLACK):
				ASSERT_TRUE(spArrayListSize(possibleMoves) == 8);
				break;
			case QUEEN_LOC(BLACK):
				ASSERT_TRUE(spArrayListSize(possibleMoves) == 19);
				break;
			case KING_LOC(BLACK):
				if (game->currentPlayer) {
					ASSERT_TRUE(spArrayListSize(possibleMoves) == 8);
				}
				else {
					ASSERT_TRUE(spArrayListSize(possibleMoves) == 5);
				}
			default:
				break;
		}
	}
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
	int move = spChessGameSetMoveCoordinates(6, 0, 5, 0);
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

static int spChessGameTestCheckGameState() {
    SPChessGame* game = spChessGameCreate();
    ASSERT_TRUE(game);
    ASSERT_TRUE(spChessCheckGameState(game, WHITE) == SP_CHESS_GAME_SUCCESS);
    game->currentPlayer = BLACK;
    ASSERT_TRUE(spChessCheckGameState(game, BLACK) == SP_CHESS_GAME_SUCCESS);
    for (int i = 0; i < N_COLUMNS; i++) {
    	for (int j = 0; j < N_ROWS; j++) {
    		game->gameBoard[i][j] = '\0';
    	}
    }
    for (int i = 0; i < N_COLUMNS * 4; i++) {
        if (i == KING_LOC(WHITE)) {
            game->locations[i] = spChessGameSetPosition2(0, 0);
            game->gameBoard[0][0] = KING(WHITE);
        }
        else if (i == RIGHT_BISHOP_LOC(BLACK)) {
            game->locations[i] = spChessGameSetPosition2(2, 1);
            game->gameBoard[2][1] = BISHOP(BLACK);
        }
        else if (i == LEFT_BISHOP_LOC(BLACK)) {
            game->locations[i] = spChessGameSetPosition2(2, 2);
			game->gameBoard[2][2] = BISHOP(BLACK);
        }
        else if (i == QUEEN_LOC(BLACK)) {
            game->locations[i] = spChessGameSetPosition2(2, 3);
			game->gameBoard[2][3] = QUEEN(BLACK);
        }
        else if (i == KING_LOC(BLACK)) {
        	game->locations[i] = spChessGameSetPosition2(7, 7);
			game->gameBoard[7][7] = KING(BLACK);
        }
        else {
            game->locations[i] = 0;
        }
    }
    game->currentPlayer = WHITE;
    ASSERT_TRUE(spChessCheckGameState(game, WHITE) == SP_CHESS_GAME_CHECKMATE);
    game->locations[RIGHT_BISHOP_LOC(BLACK)] = game->locations[QUEEN_LOC(BLACK)] = game->gameBoard[2][1] = game->gameBoard[2][3] = 0;
    ASSERT_TRUE(spChessCheckGameState(game, WHITE) == SP_CHESS_GAME_CHECK);
    for (int i = 0; i < N_COLUMNS * 4; i++) {
        game->locations[i] = 0;
    }
	ASSERT_TRUE(spChessCheckGameState(game, BLACK) == SP_CHESS_GAME_DRAW);
    game->currentPlayer = WHITE;
    ASSERT_TRUE(spChessCheckGameState(game, WHITE) == SP_CHESS_GAME_DRAW);
    spChessGameDestroy(game);
    return 1;
}

int main() {
	RUN_TEST(spChessGameBasicTest);
	RUN_TEST(spChessGameTestIsValidMove);
	RUN_TEST(spChessGameTestGetMoves);
	RUN_TEST(spChessGameTestCopy);
	RUN_TEST(spChessGameTestSetMove);
	RUN_TEST(spChessGameTestUndoMove);
    RUN_TEST(spChessGameTestCheckGameState);
	return 0;
}
