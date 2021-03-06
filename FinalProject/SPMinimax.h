#ifndef SPMINIMAX_H_
#define SPMINIMAX_H_

#include "SPChessGame.h"

/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a Minimax algorithm up to a
 * certain length, decided by currentGame's difficulty. The current game
 * state doesn't change by this function including the history of previous
 * moves.
 *
 * @param currentGame - The current game state
 * @return
 * -1 if either currentGame is NULL or an error occurred
 * 0 if there is no possible move to make
 * On success the function returns an integer representing the suggested move.
 */
int spMinimaxSuggestMove(SPChessGame* currentGame);

#endif
