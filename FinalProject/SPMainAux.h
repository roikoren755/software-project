#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_
#include "SPChessGame.h"
#include "SPChessParser.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPChessSettingsSdl.h"
#include "SPChessGameSdl.h"
#include "SPChessParser.h"
#include "Button.h"

int mainSdl(SPChessGame* game ) ;
int min(int a,int b);

/***
 * Get destination position from int representing a move.
 * @param move - Move to get destination position from.
 * @return the destination position as a char.
 */
unsigned char spChessGameGetDestinationPositionFromMove(unsigned int move);

/***
 * Get current position from int representing a move.
 * @param move - Move to get current position from.
 * @return the current location as a char.
 */
unsigned char spChessGameGetCurrentPositionFromMove(unsigned int move);

/***
 * Get column from char representing a location.
 * @param position - Position to get column from.
 * @return the column number from the given position.
 */
unsigned int spChessGameGetColumnFromPosition(unsigned char position);

/***
 * Get row from char representing a location.
 * @param position - Position to get row from.
 * @return the row number of the given position.
 */
unsigned int spChessGameGetRowFromPosition(unsigned char position);

/***
 * Fprints game's settings, in the format described in the assignment, to file
 * @param game - Game whose settings you want to fprint
 * @param file - Pointer to an OPENED file to fprint to
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or file are NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spFprintSettings(SPChessGame* game, FILE* file);

/***
 * Opens file and saves game to it, in the format described in the assignment
 * @param game - Game whose current state you want to save.
 * @param file - Either relative or full path to file to save game to
 * @return SP_CHESS_GAME_INVALID_MOVE if game or file are NULL, or an error occurred while opening file.
 *         SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessSaveGame(SPChessGame* game, const char* file);

/***
 *
 * @return
 */
SPCommand spGetCommand();

SP_CHESS_GAME_MESSAGE spChessLoadGame(SPChessGame* game, char* file);

SP_CHESS_GAME_MESSAGE spChessVerifyPositionAndPiece(SPChessGame* game, char position);

void spPrintUndoneMove(int move, int color);

void spPrintComputerMove(char piece, int move);

char spChessGameGetPieceAtPosition(SPChessGame* game, char position);

/**
 * given a list, prints all possible moves listed.
 *
 * @param list - The source list
 * @return
 * SP_CHESS_GAME_SUCCESS  - if the the moves were successfully printed
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessPrintMoves(SPArrayList* list);

/***
 * Prepares an int to represent a move, so as to be accepted by spChessGameSetMove(...)
 * @param currentRow - Move's starting row
 * @param currentColumn - Move's starting column
 * @param destinationRow - Move's finishing row
 * @param destinationColumn - Move's finishing column
 * @return
 * An int representing the move indicated by the arguments, with each location as returned by spChessGameSetLocation(...)
 */
unsigned int setMoveCoordinatesToInt(unsigned int currentRow, unsigned int currentColumn, unsigned int destinationRow, unsigned int destinationColumn);

/***
 * Returns whether step captures a piece.
 * @param step - Step made
 * @return 1 if step captures.
 *         0 otherwise.
 */
int spChessGameStepWillCapture(unsigned int step);

/***
 * Returns whether after step piece that made it will be threatened.
 * @param step - Step made
 * @return 1 if piece is threatened after step.
 *         0 otherwise.
 */
int spChessGameStepWillThreaten(unsigned int step);

/***
 * Returns a char representing the location indicated by row and column, if both
 * are within range (< N_ROWS && < N_COLUMNS, respectively).
 * @param row - Row coordinate
 * @param column - Column coordinate
 * @return 0 if one of the coordinates is out of bounds.
 *         A char representing the location, otherwise.
 */
unsigned char spChessGameSetLocation(int row, int column);

#endif
