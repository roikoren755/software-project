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

int alphaBetaPruning(SPChessGame* game, int depth, int alpha, int beta) {
	if (!depth || spChessGameEnded(game)) {
		return spChessScoreBoard(game);
	}
	int v;
	if (game->currentPlayer) {
		v = INT_MIN;
		for (int i = 0; i < 2 * N_COLUMNS; i++) {
			if (game->locations[i + game->currentPlayer * 2 * N_COLUMNS]) {
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[i]);
				while (spArrayListSize(possibleMoves)) {
					int move = spArrayListGetFirst(possibleMoves);
					SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves);
					if (message != SP_ARRAY_LIST_SUCCESS) {
						free(possibleMoves);
						return 0;
					}
					SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move);
					int childScore = alphaBetaPruning(game, depth - 1, alpha, beta);
					gameMessage = spChessGameUndoMove(game);
					v = childScore > v ? childScore : v;
					alpha = v > alpha ? v : alpha;
					if (beta <= alpha) {
						break;
					}
				}
				free(possibleMoves);
				if (beta <= alpha) {
					break;
				}
			}
		}
	}

	else {
		v = INT_MAX;
		for (int i = 0; i < 2 * N_COLUMNS; i++) {
			if (game->locations[i + game->currentPlayer * 2 * N_COLUMNS]) {
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[i]);
				while (spArrayListSize(possibleMoves)) {
					int move = spArrayListGetFirst(possibleMoves);
					SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(possibleMoves);
					if (message != SP_ARRAY_LIST_SUCCESS) {
						free(possibleMoves);
						return 0;
					}
					SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move);
					int childScore = alphaBetaPruning(game, depth - 1, alpha, beta);
					gameMessage = spChessGameUndoMove(game);
					v = childScore < v ? childScore : v;
					beta = v < beta ? v : beta;
					if (beta <= alpha) {
						break;
					}
				}
				free(possibleMoves);
				if (beta <= alpha) {
					break;
				}
			}
		}
	}
	return v;
}

int spMinimaxGetLowestScoringMove(SPChessGame* game) {
	if (!game) {
		return 0;
	}
	int bestScore = INT_MAX;
	int bestMove;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	if (game->currentPlayer) {
		for (int i = 0; i < 2 * N_COLUMNS; i++) {
			if (game->locations[i + game->currentPlayer * 2 * N_COLUMNS]) {
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[i + game->currentPlayer * 2 * N_COLUMNS]);
				while (!spArrayListIsEmpty(possibleMoves)) {
					int move = spArrayListGetFirst(possibleMoves);
					spArrayListRemoveFirst(possibleMoves);
					spChessGameSetMove(move);
					int score = alphaBetaPruning(game, game->difficulty - 1, alpha, beta);
					spChessGameUndoMove(move);
					if (score < bestScore) {
						bestScore = score;
						bestMove = move;
					}
				}
			}
		}
	}
	return bestMove;
}

int spMinimaxGetHighestScoringMove(SPChessGame* game) {
	if (!game) {
		return 0;
	}
	int bestScore = INT_MIN;
	int bestMove;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	if (game->currentPlayer) {
		for (int i = 0; i < 2 * N_COLUMNS; i++) {
			if (game->locations[i + game->currentPlayer * 2 * N_COLUMNS]) {
				SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[i + game->currentPlayer * 2 * N_COLUMNS]);
				while (!spArrayListIsEmpty(possibleMoves)) {
					int move = spArrayListGetFirst(possibleMoves);
					spArrayListRemoveFirst(possibleMoves);
					spChessGameSetMove(move);
					int score = alphaBetaPruning(game, game->difficulty - 1, alpha, beta);
					spChessGameUndoMove(move);
					if (score > bestScore) {
						bestScore = score;
						bestMove = move;
					}
				}
			}
		}
	}
	return bestMove;
}

int spMinimaxGetBestMove(SPChessGame* game) {
	if (game->currentPlayer) {
		return spMinimaxGetHighestScoringMove(game);
	}

	return spMinimaxGetLowestScoringMove(game);
}

int spMinimaxSuggestMove(SPChessGame* currentGame) {
	if (!currentGame) { // currentGame is NULL, or maxDepth is invalid
		return -1;
	}

	SPChessGame* minimaxGame = spChessGameCreate(currentGame->difficulty);
	if (!minimaxGame) {
		return -1;
	}

	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			minimaxGame->gameBoard[i][j] = currentGame->gameBoard[i][j];
			if (j < 4) {
				minimaxGame->locations[i + j * N_COLUMNS] = currentGame->locations[i + j * N_COLUMNS];
			}
		}
	}
	minimaxGame->user_color = currentGame->user_color;
	minimaxGame->difficulty = currentGame->difficulty;
	minimaxGame->currentPlayer = currentGame->currentPlayer;
	minimaxGame->blackKingThreaten = currentGame->blackKingThreaten;
	minimaxGame->whiteKingThreaten = currentGame->whiteKingThreaten;
	minimaxGame->game_mode = currentGame->game_mode;

	return spMinimaxGetBestMove(minimaxGame);
}
