//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSGAME_H
#define SOFTWARE_PROJECT_SPCHESSGAME_H

#include "SPArrayList.h"
#include <stdio.h>

#define WHITE 1
#define BLACK 0
#define N_ROWS 8
#define N_COLUMNS 8
#define STARTING_ROW "RNBQKBNR"
#define PAWN(color) ('M' + color * ('a' - 'A'))
#define KNIGHT(color) ('N' + color * ('a' - 'A'))
#define BISHOP(color) ('B' + color * ('a' - 'A'))
#define ROOK(color) ('R' + color * ('a' - 'A'))
#define QUEEN(color) ('Q' + color * ('a' - 'A'))
#define KING(color) ('K' + color * ('a' - 'A'))
#define BLANK '_'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define KING_LOC(color) (4 + color * 3 * N_COLUMNS)
#define QUEEN_LOC(color) (3 + color * 3 * N_COLUMNS)
#define LEFT_ROOK_LOC(color) (0 + color * 3 * N_COLUMNS)
#define RIGHT_ROOK_LOC(color) (7 + color * 3 * N_COLUMNS)
#define LEFT_KNIGHT_LOC(color) (1 + color * 3 * N_COLUMNS)
#define RIGHT_KNIGHT_LOC(color) (6 + color * 3 * N_COLUMNS)
#define LEFT_BISHOP_LOC(color) (2 + color * 3 * N_COLUMNS)
#define RIGHT_BISHOP_LOC(color) (5 + color * 3 * N_COLUMNS)
#define CHECK_COLOR(piece) (piece > 'Z')
#define CHECKMATE 3
#define CHECK 2
#define DRAW 1
#define NORMAL 0

typedef struct sp_chess_game_t {
	int gameMode;
	int difficulty;
	int userColor;
    char gameBoard[N_ROWS][N_COLUMNS];
	char locations[N_COLUMNS * 4];
    int currentPlayer;
    SPArrayList* history;
    int gameState;
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
	SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED,
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_ALLOCATION_ERROR,
	SP_CHESS_GAME_CHECK,
	SP_CHESS_GAME_CHECKMATE,
	SP_CHESS_GAME_DRAW
} SP_CHESS_GAME_MESSAGE;

/***
 * Create a new game, with the board set to the starting set up
 * @return NULL if an allocation error occurred.
 *         a pointer to the created game, otherwise
 */
SPChessGame* spChessGameCreate();

/***
 * Copies all of game's state to a new game
 * @param game - Pointer to game to copy
 * @return NULL if game is NULL or an allocation error occurred.
 *         A pointer to the new copy, otherwise
 */
SPChessGame* spChessGameCopy(SPChessGame* game);

/***
 * Destroy's game, freeing all used memory
 * @param game - Pointer to game to destroy
 */
void spChessGameDestroy(SPChessGame* game);

/***
 * Check's the current game state for the color player in game's game board, updates game->gameState,
 * and returns accordingly.
 * @param game - Game to check state at.
 * @param color - Player color to check state for.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, or an error occurred while check possible moves.
 * 		   SP_CHESS_GAME_CHECKMATE if color's king is threatened and there are no saving moves.
 * 		   SP_CHESS_GAME_DRAW if the king isn't threatened and there are no moves possible.
 * 		   SP_CHESS_GAME_CHECK if the king is threatened but there are saving moves.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessCheckGameState(SPChessGame* game, int color);

/**
 * Checks whether move represents a valid move one game's game board.
 * @param game - The source game
 * @param move - The specified move.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL.
 * 		   SP_CHESS_GAME_INVALID_POSITION if move's starting position or destination position are out of bounds.
 * 		   SP_CHESS_GAME_NO_PIECE_IN_POSITION if there is no piece of game->currentPlayer's color in starting position.
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED if the move results in game->currentPlayer's king being threatened.
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED if the move leaves game->currentPlayer's king threatened.
 * 		   SP_CHESS_GAME_SUCCESS if the move is a legal move, according to assignment.
 * 		   SP_CHESS_GAME_ILLEGAL_MOVE otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* game, int move);

/**
 * Returns an SPArrayList containing all possible moves for piece at position, saving for each move it's destination
 * position, whether the moving piece is threatened after it, and if the move captures an opponent's piece.
 * @param game - The source game
 * @param position - The piece's position
 * @return NULL if game is NULL, an allocation error occurred while creating the SPArrayList or one happened while
 * 				adding steps to list.
 * 		   A pointer to an SPArrayList containing all legal moves, otherwise.
 */
SPArrayList* spChessGameGetMoves(SPChessGame* game, char position);

/***
 * Makes the move passed to it in game's game.
 * @param game - Game to make move in
 * @param move - A legal move to play
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, or an error happened while accessing game->history
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* game, int move);

/***
 * Tries undo-ing the last move played
 * @param game - Game to undo move in
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, or an error occurred while accessing game->history
 *         SP_CHESS_GAME_NO_HISTORY if there is no move to undo
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameUndoMove(SPChessGame* game);

/***
 * Prints game's game board, according to the format given
 * @param game - Game whose board you want to print
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* game);

/***
 * Fprints game's board to the given file
 * @param game - The game whose board you want to fprint
 * @param file - The file to fprint to
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT if game or file are NULL
 * SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameFprintBoard(SPChessGame* game, FILE* file);

/***
 * Returns an integer representing the last move made in game.
 * @param game - Game to get last move from.
 * @return 0 if game is NULL, or game's history is empty.
 * 		   An integer representing the last move made, otherwise.
 */
unsigned int spChessGameGetLastMovePlayed(SPChessGame* game);

/***
 * Resets the game pointed at by game to the starting set-up, by re-arranging the board,
 * and clearing up game's history.
 * @param game - Game to reset
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, or an error occurred while clearing game's
 * 										  history.
 * 		   SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameResetGame(SPChessGame* game);

/***
 * Resets game's game board to the starting set-up
 * @param game - Pointer to game whose board you want to reset.
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* game);

#endif //SOFTWARE_PROJECT_SPCHESSGAME_H
