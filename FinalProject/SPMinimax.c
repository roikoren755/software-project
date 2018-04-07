/*
 * SPMinimax.c
 *
 *  Created on: 26 Dec 2017
 *      Author: user
 */
#include "SPMinimax.h"
#include "SPMainAux.h"
#include <limits.h>
#include <stdlib.h>

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

    SP_CHESS_GAME_MESSAGE message = spChessCheckGameState(game, game->currentPlayer);
    if (message == SP_CHESS_GAME_DRAW) { // Draw
        return 0;
    }
    else if (message == SP_CHESS_GAME_CHECKMATE) { // Check
        return game->currentPlayer ? -1000 : 1000; // And mate
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

		pieceScore = indexToPieceScore(i); // Get piece's score at index

		if (game->locations[i]) { // Subtract for black
			result -= pieceScore;
		}
		if (game->locations[i + 3 * N_COLUMNS]) { // Add for white
			result += pieceScore;
		}
	}

	return result; // And that's the score
}

/***
 * The main recursive function of the minimax algorithm. Recursively goes over all possible moves,
 * up to game->difficulty moves ahead, and returns the score of the node representing the current game state.
 * This function utilizes alpha-beta pruning, as per the instructions
 * @param game - Current game state to score
 * @param depth - Depth of current minimax node, with the leaves having depth 0
 * @param alpha - Alpha cut-off
 * @param beta - Beta cut-off
 * @return
 * 0 if an error ocurred (or that is the node's score...)
 * The node's score according to the minimax algorithm and scoring function given in the instructions.
 */
int alphaBetaPruning(SPChessGame* game, int depth, int alpha, int beta) {
	if (!game) {
		return 0;
	}

	if (!depth) { // Got to maximum depth
		return spChessScoreBoard(game);
	}

	int v = game->currentPlayer ? INT_MIN : INT_MAX; // Set worst result for current player
	int index;
	unsigned char currentLocation;
	unsigned char destinationLocation;
	int move;
	int step;

	if (game->currentPlayer) { // Current player is white
		for (int i = 0; i < 2 * N_COLUMNS; i++) { // Go over all possible player pieces
			index = i + 2 * N_COLUMNS;

			currentLocation = game->locations[index]; // So we have starting position ready to be added to a move
			if (currentLocation) { // Piece is still in play, index is white pieces' range
				SPArrayList* possibleMoves = spChessGameGetMoves(game, currentLocation); // Get piece's moves
				if (!possibleMoves) { // Oops
					return 0;
				}

				while (!spArrayListIsEmpty(possibleMoves)) { // As long as there are any we haven't checked
					step = spArrayListGetFirst(possibleMoves);
					destinationLocation = spChessGameGetDestinationPositionFromMove(step); // Get destination position
					move = currentLocation << 8;
					move |= destinationLocation;

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
			currentLocation = game->locations[i];
			if (currentLocation) { // Black pieces' range, and piece in play
				SPArrayList* possibleMoves = spChessGameGetMoves(game, currentLocation); // Get moves for piece
				if (!possibleMoves) { // Oops
					return 0;
				}

				while (!spArrayListIsEmpty(possibleMoves)) { // While there are moves we haven't checked
					step = spArrayListGetFirst(possibleMoves);
					destinationLocation = spChessGameGetDestinationPositionFromMove(step);
					move = currentLocation << 8;
					move |= destinationLocation;

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

/***
 * Returns the move that will lead to the lowest score, as per the scoring function and minimax algorithm.
 * @param game - Current game where we want to find lowest scoring move
 * @param difficulty - How many moves forward to check in the minimax algorithm
 * @return
 * -1 if game is NULL, or an error occurred
 * An integer representation of the move that will lead to the lowest score, otherwise.
 */
int spMinimaxGetLowestScoringMove(SPChessGame* game, int difficulty) {
	if (!game) { // game is NULL
		return -1;
	}

	int bestScore = INT_MAX; // Initializations
	int bestMove = -1;
	char bestStartingPosition = -1;
	char bestDestinationPosition = -1;
	int bestStartingColumn;
	int bestStartingRow;
	int bestDestinationColumn;
	int bestDestinationRow;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int index;
	int move;
	int score;
	unsigned char startingPosition;
	unsigned char destinationPosition;
	int startingColumn;
	int startingRow;
	int destinationColumn;
	int destinationRow;

	for (int i = 0; i < 2 * N_COLUMNS; i++) { // Go over all player's pieces (only applicable for black player)
		index = i + game->currentPlayer * 2 * N_COLUMNS;
		startingPosition = game->locations[index];

		if (startingPosition) { // If it's still in play
			SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]); // Get piece's moves
			if (!possibleMoves) { // Oops
				return -1;
			}

			while (!spArrayListIsEmpty(possibleMoves)) { // For each move
				destinationPosition = spChessGameGetDestinationPositionFromMove(spArrayListGetFirst(possibleMoves)); // Get move
				move = startingPosition << 8;
				move |= destinationPosition; // Move is now in the correct format for spChessGameSetMove

				SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves); // Remove move
				if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves); // Freeeeee
					return -1; // As a birrddddd
				}

				SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move); // Do the move
				if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves);
					return -1;
				}

				score = alphaBetaPruning(game, difficulty - 1, alpha, beta); // Score child recursively

				gameMessage = spChessGameUndoMove(game); // Undo the move
				if (gameMessage != SP_CHESS_GAME_SUCCESS) { // Shouldn't happen
					spArrayListDestroy(possibleMoves);
					return -1;
				}

				if (score < bestScore) { // Got temp best score?
					bestScore = score;
					bestMove = move;
					bestStartingPosition = startingPosition;
					bestDestinationPosition = destinationPosition;
				}
                else if (score == bestScore) { // If score is same, prefer move with lowest:
                    bestStartingColumn = spChessGameGetColumnFromPosition(bestStartingPosition);
                    bestStartingRow = spChessGameGetRowFromPosition(bestStartingPosition);
                    bestDestinationColumn = spChessGameGetColumnFromPosition(bestDestinationPosition);
                    bestDestinationRow = spChessGameGetRowFromPosition(bestDestinationPosition);
                    startingColumn = spChessGameGetColumnFromPosition(startingPosition);
                    startingRow = spChessGameGetRowFromPosition(startingPosition);
                    destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
                    destinationRow = spChessGameGetRowFromPosition(destinationPosition);
                    if (startingColumn < bestStartingColumn || // 1. Starting column
						(startingColumn == bestStartingColumn && (startingRow > bestStartingRow || // 2. Starting row
						(startingRow == bestStartingRow && (destinationColumn < bestDestinationColumn || // 3. Destination column
						(destinationColumn == bestDestinationColumn && (destinationRow > bestDestinationRow))))))) { // 4. Destination Row
						bestMove = move;
						bestStartingPosition = startingPosition;
						bestDestinationPosition = destinationPosition;
					}
                }
			}

			spArrayListDestroy(possibleMoves);
		}
	}

	return bestMove;
}

/***
 * Same as above, only for the move leading to the highest score.
 * @param game
 * @param difficulty
 * @return
 */
int spMinimaxGetHighestScoringMove(SPChessGame* game, int difficulty) { // SAME (only for white player) ^
	if (!game) {
		return -1;
	}

	int bestScore = INT_MIN;
	int bestMove = -1;
	char bestStartingPosition = -1;
	char bestDestinationPosition = -1;
	int bestStartingColumn;
	int bestStartingRow;
	int bestDestinationColumn;
	int bestDestinationRow;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int index;
	int move;
	int score;
	unsigned char startingPosition;
	unsigned char destinationPosition;
	int startingColumn;
	int startingRow;
	int destinationColumn;
	int destinationRow;

	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		index = i + game->currentPlayer * 2 * N_COLUMNS;
		startingPosition = game->locations[index];

		if (startingPosition) {
			SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]);
			if (!possibleMoves) {
				return -1;
			}

			while (!spArrayListIsEmpty(possibleMoves)) {
				destinationPosition = spChessGameGetDestinationPositionFromMove(spArrayListGetFirst(possibleMoves)); // Get move
				move = startingPosition << 8;
				move |= destinationPosition;

				SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves);
				if (message != SP_ARRAY_LIST_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return -1;
				}

				SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move);
				if (gameMessage != SP_CHESS_GAME_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return -1;
				}

				score = alphaBetaPruning(game, difficulty - 1, alpha, beta);

				gameMessage = spChessGameUndoMove(game);
				if (gameMessage != SP_CHESS_GAME_SUCCESS) {
					spArrayListDestroy(possibleMoves);
					return -1;
				}

				if (score > bestScore) { // Got temp best score?
					bestScore = score;
					bestMove = move;
					bestStartingPosition = startingPosition;
					bestDestinationPosition = destinationPosition;
				}
				else if (score == bestScore) { // If score is same, prefer move with lowest:
					bestStartingColumn = spChessGameGetColumnFromPosition(bestStartingPosition);
					bestStartingRow = spChessGameGetRowFromPosition(bestStartingPosition);
					bestDestinationColumn = spChessGameGetColumnFromPosition(bestDestinationPosition);
					bestDestinationRow = spChessGameGetRowFromPosition(bestDestinationPosition);
					startingColumn = spChessGameGetColumnFromPosition(startingPosition);
					startingRow = spChessGameGetRowFromPosition(startingPosition);
					destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
					destinationRow = spChessGameGetRowFromPosition(destinationPosition);
					if (startingColumn < bestStartingColumn || // 1. Starting column
						(startingColumn == bestStartingColumn && (startingRow > bestStartingRow || // 2. Starting row
						(startingRow == bestStartingRow && (destinationColumn < bestDestinationColumn || // 3. Destination column
						(destinationColumn == bestDestinationColumn && (destinationRow > bestDestinationRow))))))) { // 4. Destination Row
						bestScore = score;
						bestMove = move;
						bestStartingPosition = startingPosition;
						bestDestinationPosition = destinationPosition;
					}
				}
			}

			spArrayListDestroy(possibleMoves);
		}
	}

	return bestMove;
}

/***
 * Returns the best move for the player whose turn it currently is, by using the minimax algorith,
 * @param game - Game to get best move for current player for
 * @return
 * -1 if game is NULL, or an error occurred
 * An integer representing the best move, otherwise
 */
int spMinimaxGetBestMove(SPChessGame* game) {
	if (!game) {
		return -1;
	}
	if (game->currentPlayer) { // White plays, wants to maximize
		return spMinimaxGetHighestScoringMove(game, game->difficulty);
	}

	return spMinimaxGetLowestScoringMove(game, game->difficulty); // Black wants to minimize
}

int spMinimaxSuggestMove(SPChessGame* currentGame) {
	if (!currentGame) { // currentGame is NULL
		return -1;
	}

	SPChessGame* minimaxGame = spChessGameCreate(); // Create a new game
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
	minimaxGame->gameMode = currentGame->gameMode;

	return spMinimaxGetBestMove(minimaxGame); // And gimme a move
}
