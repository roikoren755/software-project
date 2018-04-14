#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPChessGame.h"
#include "SPChessParser.h"

/***
 * runs the GUI mode display.
 * creates an array of the relavent screens,
 * and loops the currently shown screen (only one
 * screen is shown at any time)
 * and its widgets to see if one is pressed.
 * @param game - pointer to the game's struct
 * @return 0 when exited successfully and all resources freed
 */
int runSdl(SPChessGame* game);

/***
 * Runs the console version of the game.
 * @param game - Game to run
 * @return 0, after freeing all used memory, including game.
 */
int runConsole(SPChessGame* game);

/***
 * Returns the lower of the two. Duh.
 * @param a
 * @param b
 * @return min(a, b)
 */
int min(int a, int b);

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
 * Gets a line of input from stdin, parses it using SPChessParser's spParserParseLine, and returns
 * the resulting SPCommand.
 * @return SPCommand representing the user's input.
 */
SPCommand spGetCommand();

/***
 * Loads saved game from file (file) into game. Assumes file is correctly formatted.
 * @param game - Game to load unto
 * @param file - Full or relative path to file to load from
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or file are NULL, or if an error occurred while opening
 * 										  file, clearing history or checking game state after loading.
 * 		   SP_CHESS_GAME_CHECKMATE if loaded game is over due to checkmate.
 * 		   SP_CHESS_GAME_DRAW if loaded game finished with a draw.
 * 		   SP_CHESS_GAME_CHECK if current state in game, after loading, is check.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessLoadGame(SPChessGame* game, char* file);

/***
 * Verifies that position is a valid representation of a board location, and that it contains a piece, at least
 * in game's game board.
 * @param game - Game to check in.
 * @param position - Position to verify
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL.
 * 		   SP_CHESS_GAME_INVALID_POSITION if position is out of bounds.
 * 		   SP_CHESS_GAME_NO_PIECE_IN_POSITION if there is no piece at position.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessVerifyPositionAndPiece(SPChessGame* game, char position);

/***
 * Prints move as an undone move for color's player, in the format described in the assignment. Assumes both arguments
 * are valid.
 * @param move - Move to print.
 * @param color - Player who made the move.
 */
void spPrintUndoneMove(int move, int color);

/***
 * Prints computer's made move, as per assignment format, with move indicating the move and piece being the moved piece.
 * Both arguments are assumed to be valid.
 * @param piece - Piece that made the move.
 * @param move - Move made by the computer.
 */
void spPrintComputerMove(char piece, int move);

/***
 * Returns the piece at location on game's game board.
 * @param game - Game to get piece from.
 * @param position - Position to get piece at.
 * @return '\0' if game is NULL, position isn't valid, or there's no piece at position.
 * 		   A char representing the piece at position, otherwise.
 */
char spChessGameGetPieceAtPosition(SPChessGame* game, char position);

/***
 * Prints all the moves at list, according to the format given in the assignment.
 * @param list - SPArrayList containing int representations of moves.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if list is NULL.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
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

/***
 * Print " Chess\n" and an underline beneath it. Yeah, I know!
 */
void spChessPrintGameTitle();

/***
 * Sets game mode to the one requested by command, if it is a legal game mode.
 * @param game - Game to set game mode in.
 * @param command - Command to set game mode.
 */
void spChessGameSetGameMode(SPChessGame* game, SPCommand* command);

/***
 * Sets game difficulty to that chosen in command. Also prints difficulty's name to the screen in the required
 * format.
 * @param game - Game to set difficulty in.
 * @param command - Command to set difficulty.
 * @param difficulties - Array of char* difficulty names => difficulties[i] === name of 1-based i-th lowest difficulty.
 */
void spChessGameSetDifficulty(SPChessGame* game, SPCommand* command, char** difficulties);

/***
 * Sets game's user color.
 * @param game - Game to set user color in.
 * @param command - Command to do so.
 */
void spChessGameSetUserColor(SPChessGame* game, SPCommand* command);

/***
 * Loads save game pointed at by command->arguments, into game.
 * @param game - Game to load in
 * @param command - Command to load game
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or command are null, command->cmd isn't SP_LOAD, or an error occurred
 * 										  while loading game.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameLoadGame(SPChessGame* game, SPCommand* command);

/***
 * Resets game's settings to the default ones.
 * @param game - Game to reset settings in.
 */
void spChessGameResetSettings(SPChessGame* game);

/***
 * Makes the move asked for by command, and if needed makes the following computer move, as well.
 * @param game - Game to make move in
 * @param command - Command to make move.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or command are NULL, or an error occurred.
 * 		   SP_CHESS_GAME_INVALID_POSITION if one of the positions in the move command are out of bounds
 * 		   SP_CHESS_GAME_NO_PIECE_IN_POSITION if there is no player piece at move's starting position
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE if move isn't a legal one
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED if move is "legal", but king remains threatened after it
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED if move is "legal" but king would become threatened after it
 * 		   SP_CHESS_GAME_CHECKMATE if game state is checkmate after making all moves
 * 		   SP_CHESS_GAME_DRAW if game state is draw after making all moves
 * 		   SP_CHESS_GAME_CHECK if game state is check after making all moves
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameMove(SPChessGame* game, SPCommand* command);

/***
 * Prints all possible moves for piece asked for by command
 * @param game - Game to print moves at
 * @param command - Command to print moves.
 */
void spChessGetMoves(SPChessGame* game, SPCommand* command);

/***
 * Undoes the last 2 moves (or 1 if only 1 is available to undo), and prints resulting game board.
 * @param game - Game to undo moves in.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, or an error occurred undoing a move
 * 		   SP_CHESS_GAME_NO_HISTORY if there are less than 2 moves to undo
 * 		   SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameUndo(SPChessGame* game);

#endif
