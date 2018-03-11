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
/**
 * Gets and verifies the validity of the chosen level by the user. A level
 * is considered valid if the user inputs a single digit between 1 and 7,
 * followed by the new-line character.
 * Destroys game if user enters "quit".
 *
 * @return
 * 0 	- if user entered quit,
 * level between 1 and 7 otherwise.
 */
int spGetDifficulty();

/**
 * Check that moves were actually played in src game, and returns
 * 0-based column number where last disc was inserted.
 *
 * @param game 	- the game to get the last move played in
 * @return
 * -1 	- if src is NULL, or no moves were player,
 * 0-based column number between 0 and 6 where last disc was inserted otherwise.
 */
int spChessGameGetLastMovePlayed(SPChessGame* game);

/**
 * Uses the Minimax algorithm, in Minimax.h, to suggest a move to play for the
 * user, and prints the suggested move to the user.
 * Destroys game if an error occurs during the Minimax algorithm's run.
 *
 * @param game 		- the current game
 * @param maxDepth 	- maximum depth of the Minimax tree to be created
 * @return
 * 0 	- if an error occurred,
 * 1 	- otherwise
 */
int spChessGameSuggestMove(SPChessGame* game, unsigned int maxDepth);

/**
 * Undo-s the 1 or 2 moves played since the user's last turn,
 * depending on whether the user won or not. Prints new state of board
 * if there were moves to undo, and notifies the user if there weren't any.
 *
 * @param game 		- the current game to undo moves in
 * @param winner 	- winning player symbol, as returned by spFiarCheckWinner
 */
void spChessGameUndoMove(SPChessGame* game, char winner);

/**
 * Checks that command.arg is a valid column number and adding to that column
 * is a valid move, and prints the correct error message if it isn't.
 * If it is, a user colored disc is added at the chosen column. If the user wins,
 * the function prints the board and skips the computer's turn.
 * Otherwise, the function uses the Minimax algorithm to choose a move for the
 * computer, and plays that move.
 *
 * @param game 		- the current game
 * @param command 	- add_disc command, parsed by SPFIARParser
 * @param maxDepth 	- maximum depth of the Minimax tree
 * @return
 * -1 	- an invalid argument was given, either command.validArg is false,
 * 		  command.arg isn't in the correct range, or the column is full
 * 0 	- an error occurred while running the Minimax algorithm (frees memory)
 * 1 	- otherwise
 */
int spChessGameAddDisc(SPChessGame* game, SPCommand command, unsigned int maxDepth);

/**
 * Restarts the game pointed at by the gamePointer. The function
 * destroys the game pointed at by gamePointer, creates a new one,
 * and initializes it, the same way a game is initialized at program start.
 *
 * @param gamePointer 	- a pointer to the game pointer to destroy
 * @return
 * result of spGetDifficulty (0 for "quit", level between 1 and 7 otherwise)
 */
int spChessGameRestart(SPChessGame** gamePointer);

/**
 * Runs the game. Until the user enters quit somewhere, the function gets
 * input from the user, uses SPFIARParser to parse input into command, and
 * acts according to that command.
 *
 * @param game 		- game to run
 * @param maxDepth 	- maximum depth of Minimax tree, or "difficulty"
 */
int spRunGame(SPChessGame* game, int maxDepth);

#endif
