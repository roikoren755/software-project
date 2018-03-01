//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSGAME_H
#define SOFTWARE_PROJECT_SPCHESSGAME_H

#include "SPArrayList.h"

#define WHITE 1
#define BLACK 0
#define N_ROWS 8
#define N_COLUMNS 8


typedef struct sp_chess_game_t {
	int game_mode;
	int difficulty;
	int user_color;
    char gameBoard[8][8];
    int currentPlayer;
    SPArrayList* history;
} SPChessGame;

typedef enum sp_chess_game_message_t {
    SP_CHESS_GAME_INVALID_ARGUMENT,
    SP_CHESS_GAME_INVALID_COMMAND,
    SP_CHESS_GAME_SUCCESS,
    SP_CHESS_GAME_INVALID_POSITION,
    SP_CHESS_GAME_NO_PIECE_IN_POSITION,
    SP_CHESS_GAME_ILLEGAL_MOVE,
    SP_CHESS_GAME_ILLEGAL_MOVE_WILL_THREATEN,
    SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED,
} SP_CHESS_GAME_MESSAGE;

SPChessGame* spChessGameCreate(int historySize);

SPChessGame* spChessGameCopy(SPChessGame* src);

/**
 * Checks if a disk can be put in the specified column.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
bool spChessGameIsValidMove(SPChessGame* src, int move);

SP_CHESS_GAME_MESSAGE spChessGameGetMoves(SPChessGame* src, int position);

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move);

SP_CHESS_GAME_MESSAGE spChessGameUndoPrevMove(SPChessGame* src);

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* src);

SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src);

void spChessGameDestroy(SPChessGame* src);


#endif //SOFTWARE_PROJECT_SPCHESSGAME_H
