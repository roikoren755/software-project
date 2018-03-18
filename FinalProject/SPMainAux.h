#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_
#include "SPChessGame.h"
#include "SPChessParser.h"

//put auxiliary functions and constants used by the main function here.
#define UNDO_MOVES_POSSIBLE 10
#define MAXIMUM_COMMAND_LENGTH 1024

int min(int a,int b);
int max(int a,int b);
//int abs(int a);

/***
 * Get destination position from int representing a move
 * @param move
 * @return the destination position as a char
 */
char spChessGameGetDestinationPositionFromMove(int move);

/***
 * Get current position from int representing a move
 * @param move
 * @return the current location as a char
 */
char spChessGameGetCurrentPositionFromMove(int move);

/***
 * Get column from char representing a location
 * @param position
 * @return the column number from the given position
 */
int spChessGameGetColumnFromPosition(char position);

/***
 * Get row from char representing a location
 * @param position
 * @return the row number of the given position
 */
int spChessGameGetRowFromPosition(char position);

SPCommand spGetCommand();

SP_CHESS_GAME_MESSAGE spFprintSettings(SPChessGame* game, FILE* file);

SP_CHESS_GAME_MESSAGE spChessLoadGame(SPChessGame* game, char* file);

#endif
