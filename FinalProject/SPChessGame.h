//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSGAME_H
#define SOFTWARE_PROJECT_SPCHESSGAME_H

#include "SPArrayList.h"
#include <stdbool.h>

#define WHITE 1
#define BLACK 0
#define N_ROWS 8
#define N_COLUMNS 8


typedef struct sp_chess_game_t {
	int gameMode;
	int difficulty;
	int userColor;
    char gameBoard[N_ROWS][N_COLUMNS];
	char locations[N_COLUMNS * 4];
    int currentPlayer;
    bool whiteKingThreaten;
    bool blackKingThreaten;
    SPArrayList* history;
} SPChessGame;

typedef enum sp_chess_game_message_t {
    SP_CHESS_GAME_INVALID_ARGUMENT,
    SP_CHESS_GAME_INVALID_COMMAND,
    SP_CHESS_GAME_SUCCESS,
    SP_CHESS_GAME_INVALID_POSITION,
    SP_CHESS_GAME_NO_PIECE_IN_POSITION,
    SP_CHESS_GAME_ILLEGAL_MOVE,
    SP_CHESS_GAME_MOVE_WILL_THREATEN,
    SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED,
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_ALOCATION_ERROR,
} SP_CHESS_GAME_MESSAGE;

/***
 * Create a new game, with the board set to the starting set up
 * @return NULL if an allocation error occurred.
 *         a pointer to the created game, otherwise
 */
SPChessGame* spChessGameCreate();

/***
 * Copies all of src's state to a new game
 * @param src - Pointer to game to copy
 * @return NULL if src is NULL or an allocation error occurred.
 *         A pointer to the new copy, otherwise
 */
SPChessGame* spChessGameCopy(SPChessGame* src);

/***
 * Destroy's src, freeing all used memory
 * @param src - Pointer to game to destroy
 */
void spChessGameDestroy(SPChessGame* src);

/**
 * given an in range move, checks if the move is valid regarding the piece at the
 * current position.
 *
 * @param src - The source game
 * @param move - The specified move.
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* src, int move);

/**
 * given a move, Checks if a the piece that after the move will be
 * at the given location will be threaten (after that move).
 * this function does not check whether the move is valid regarding the movement abilities
 * of the pieces.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move,char location){

/**
 * Checks if a piece is threatened by an opponent.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
bool spChessGameIsPieceThreatened(SPChessGame* src, char pieceLocation);

bool spChessGameCheckDiagonalMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckStraightLineMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckKnightMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckKingThreat(SPChessGame* src , char targetLoc , char threatLoc);

/**
 * returns all possible moves of the piece located at <x,y>.
 * This function assumes there is a piece in that location.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SPArrayList* spChessGameGetMoves(SPChessGame* src, int position);

SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* src,SPArrayList* steps, char position, int verDir, int horDir,int color);

SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* src,SPArrayList* steps, char position,int color);

/***
 * Makes the move passed to it in src's game.
 * @param src - Game to make move in
 * @param move - A legal move to play
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL, or an error happened while accessing src->history
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move);

/***
 * Tries undo-ing the last move played
 * @param src - Game to undo move in
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL, or an error occurred while accessing src->history
 *         SP_CHESS_GAME_NO_HISTORY if there is no move to undo
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameUndoMove(SPChessGame* src);

/***
 * Prints src's game board, according to the format given
 * @param src - Game whose board you want to print
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* src);

void spChessGameDestroy(SPChessGame* src);

#endif //SOFTWARE_PROJECT_SPCHESSGAME_H