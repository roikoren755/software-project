/*
 * SPMinimax.c
 *
 *  Created on: 26 Dec 2017
 *      Author: user
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPChessGame.h"
#include "SPMinimax.h"

#define KING 'K'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define PAWN_SCORE 1
#define KNIGHT_SCORE 3
#define BISHOP_SCORE KNIGHT_SCORE
#define ROOK_SCORE 5
#define QUEEN_SCORE 9

/***
 * Converts a piece's index (0-7) in the starting row to score
 * @param index - index of piece
 * @return The corresponding piece's score
 */
int indexToPieceScore(int index) {
	if (index == 0 || index == 7) {
		return ROOK_SCORE;
	}

	if (index == 1 || index == 6) {
		return KNIGHT_SCORE;
	}

	if (index == 2 || index == 5) {
		return BISHOP_SCORE;
	}

	if (index == 3) {
		return QUEEN_SCORE;
	}

	return 0; // Like there would be only one king in a game. Like, ever
}

/***
 * Scores the board, according to the scoring function provided
 * @param game
 * @return -1 if game is NULL
 * 		   game's board's score, otherwise
 */
int spChessScoreBoard(SPChessGame* game) {
	if (!game) {
		return -1;
	}

    int gameOver = 1;
	int index;
    for (int i = 0; i < 2 * N_COLUMNS; i++) {
        index = i + game->currentPlayer * 2 * N_COLUMNS;
		if (game->locations[index]) {
            SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]);
            if (!spArrayListIsEmpty(possibleMoves)) { // There's a piece that can move
                gameOver = 0; // Game isn't over
                break;
            }
        }
    }

	if (gameOver) {
		if (spChessGameIsPieceThreatened(game->currentPlayer ? CAPITAL_TO_LOW(KING) : KING)) { // Check-mate anyone?
			return game->currentPlayer ? -1000 : 1000; // YOU LOSE/WIN
		}
		return 0; // Draw
	}

	int result = 0;
	int pieceScore;
	for (int i = 0; i < N_COLUMNS; i++) {
		if (game->locations[i + N_COLUMNS]) { // Subtract for black pawn in play
			result -= PAWN_SCORE;
		}
		if (game->locations[i + 2 * N_COLUMNS]) { // Add for white one
			result += PAWN_SCORE;
		}

		pieceScore = indexToPieceScore(i); // Get pieces score at index

		if (game->locations[i]) { // Subtract for black
			result -= pieceScore;
		}
		if (game->locations[i + 3 * N_COLUMNS]) { // Add for white
			result += pieceScore;
		}
	}

	return result; // And that's the score
}

int alphaBetaPruning(SPChessGame* game, int depth, int alpha, int beta) {
	if (!depth) { // Got to maximum depth
		return spChessScoreBoard(game);
	}

	int v = game->currentPlayer ? INT_MIN : INT_MAX; // Set worst result for current player
	int index;

	if (game->currentPlayer) { // Current player is white
		for (int i = 0; i < 2 * N_COLUMNS; i++) { // Go over all possible player pieces
			index = i + 2 * N_COLUMNS;

			if (game->locations[index]) { // Piece is still in play, index is white pieces' range
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]); // Get piece's moves
				if (!possibleMoves) { // Oops
					return 0;
				}

				while (spArrayListSize(possibleMoves)) { // As long as there are any we haven't checked
					int move = spArrayListGetFirst(possibleMoves); // Get move

					SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves); // Delete move
					if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves); // Free used memory
						return 0; // Bye bye
					}

					SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move); // Make the move
					if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves); // Free memory
						return 0; // Bye bye
					}

					int childScore = alphaBetaPruning(game, depth - 1, alpha, beta); // Recursively get child's score

					gameMessage = spChessGameUndoMove(game); // Undo move
					if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves);
						return 0;
					}

					v = childScore > v ? childScore : v; // Got a higher score
					alpha = v > alpha ? v : alpha; // Improved on our alpha

					if (beta <= alpha) { // Got best possible move
						break;
					}
				}
				spArrayListDestroy(possibleMoves); // Finished with piece, free memory
			}
		}

        if (v == INT_MIN) { // I.e. no possible moves!
            return spChessScoreBoard(game); // This is a leaf node, return board score
        }
	}

	else { // Current player is black
		for (int i = 0; i < 2 * N_COLUMNS; i++) { // Go over all possible player pieces
			if (game->locations[i]) { // Black pieces' range, and piece in play
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[i]); // Get moves for piece
				if (!possibleMoves) { // Oops
					return 0;
				}

				while (spArrayListSize(possibleMoves)) { // While there are moves we haven't checked
					int move = spArrayListGetFirst(possibleMoves); // Get a move

					SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves); // Delete move
					if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves); // Free memory
						return 0; // Bye bye
					}

					SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move); // Make the move
					if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves); // Free memory
						return 0; // Bye bye
					}

					int childScore = alphaBetaPruning(game, depth - 1, alpha, beta); // Score the child recursively

					gameMessage = spChessGameUndoMove(game); // Undo the move
					if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
						spArrayListDestroy(possibleMoves);
						return 0;
					}
					v = childScore < v ? childScore : v; // Did we improve?
					beta = v < beta ? v : beta; // Did we?

					if (beta <= alpha) { // Did we do our best?
						break;
					}
				}
				spArrayListDestroy(possibleMoves); // Free memory
			}
		}

        if (v == INT_MAX) { // again, no possible moves!
            return spChessScoreBoard(game); // Score the leaf
        }
	}

	return v; // Got here after trying all moves and getting best score
}

int spMinimaxGetLowestScoringMove(SPChessGame* game) {
	if (!game) { // game is NULL
		return 0;
	}

	int bestScore = INT_MAX;
	int bestMove;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int index;

	for (int i = 0; i < 2 * N_COLUMNS; i++) { // Go over all player's pieces
		index = i + game->currentPlayer * 2 * N_COLUMNS;

		if (game->locations[index]) { // If it's still in play
			SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]); // Get piece's moves
			if (!possibleMoves) { // Oops
				return 0;
			}

			while (!spArrayListIsEmpty(possibleMoves)) { // For each move
				int move = spArrayListGetFirst(possibleMoves); // Get move

				SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves); // Remove move
				if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves); // Freeeeee
					return 0; // As a birrddddd
				}

				SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(move); // Do the move
				if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves);
					return 0;
				}

				int score = alphaBetaPruning(game, game->difficulty - 1, alpha, beta); // Score child recursively

				gameMessage = spChessGameUndoMove(move); // Undo the move
				if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves);
					return 0;
				}

				if (score < bestScore) { // Got temp best score?
					bestScore = score;
					bestMove = move;
				}
			}
		}
	}

	return bestMove;
}

int spMinimaxGetHighestScoringMove(SPChessGame* game) { // SAME ^
	if (!game) {
		return 0;
	}

	int bestScore = INT_MIN;
	int bestMove;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int index;

	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		index = i + game->currentPlayer * 2 * N_COLUMNS;

		if (game->locations[index]) {
			SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]);
			if (!possibleMoves) {
				return 0;
			}

			while (!spArrayListIsEmpty(possibleMoves)) {
				int move = spArrayListGetFirst(possibleMoves);

				SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves);
				if (message != SP_ARRAY_LIST_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return 0;
				}

				SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(move);
				if (gameMessage != SP_CHESS_GAME_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return 0;
				}

				int score = alphaBetaPruning(game, game->difficulty - 1, alpha, beta);

				gameMessage = spChessGameUndoMove(move);
				if (gameMessage != SP_CHESS_GAME_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return 0;
				}

				if (score > bestScore) {
					bestScore = score;
					bestMove = move;
				}
			}
		}
	}

	return bestMove;
}

int spMinimaxGetBestMove(SPChessGame* game) {
	if (game->currentPlayer) { // White plays, wants to maximize
		return spMinimaxGetHighestScoringMove(game);
	}

	return spMinimaxGetLowestScoringMove(game); // Black wants to minimize
}

int spMinimaxSuggestMove(SPChessGame* currentGame) {
	if (!currentGame) { // currentGame is NULL
		return -1;
	}

	SPChessGame* minimaxGame = spChessGameCreate(currentGame->difficulty); // Create a new game
	if (!minimaxGame) {
		return -1;
	}

    spArrayListDestroy(minimaxGame->history);
    minimaxGame->history = spArrayListCreate(currentGame->difficulty);
    if (!minimaxGame->history) {
        free(minimaxGame);
        return -1;
    }

	for (int i = 0; i < N_ROWS; i++) { // Copy the board and locations
		for (int j = 0; j < N_COLUMNS; j++) {
			minimaxGame->gameBoard[i][j] = currentGame->gameBoard[i][j];
			if (j < 4) {
				minimaxGame->locations[i + j * N_COLUMNS] = currentGame->locations[i + j * N_COLUMNS];
			}
		}
	}

	minimaxGame->userColor = currentGame->userColor; // And everything else quasi-important
	minimaxGame->currentPlayer = currentGame->currentPlayer;
	minimaxGame->blackKingThreaten = currentGame->blackKingThreaten;
	minimaxGame->whiteKingThreaten = currentGame->whiteKingThreaten;
	minimaxGame->gameMode = currentGame->gameMode;

	return spMinimaxGetBestMove(minimaxGame); // And gimme a move
}
