#ifndef SPMINIMAX_H_
#define SPMINIMAX_H_
#include <limits.h>
#include <stdio.h>
#include "SPFIARGame.h"
#include "SPMinimaxNode.h"
#include "SPFIARGame.h"

#define Error -1

/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a Minimax algorithm up to a
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * -1 if either currentGame is NULL or maxDepth <= 0.
 * On success the function returns a number between [0,SP_FIAR_GAME_N_COLUMNS -1]
 * which is the best move for the current player.
 */
int spMinimaxSuggestMove(SPFiarGame* currentGame,
		unsigned int maxDepth);
/**
 * Given a game state, this function creates the minimax tree up to
 * a specified length given by maxDepth.
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state (the user has to check this isn't null)
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * NULL if any allocation error occurred.
 * On success the function returns a node representing the root of the tree.
 *
 */
MM_Node* spMinimaxCreateTree(SPFiarGame* currentGame,unsigned int maxDepth);

/**
 * Given a node in the minimax tree, this function recursively creates childs for this node.
 * the number of childs is according to the constant Node_Childs_Num defined in the minimaxnode header.
 * for every possible move, the function creates a copy of the parent's game and attempts
 * the move.
 * if the move is invalid, the function creates an invalid child.
 * if the move is a winning/losing/tie, the the function creates a child with height 0.
 * otherwise, the function creates a child with height: (parent.height-1) and calls
 * itself recursively on the new child.
 * if the parent has height 0, the function does nothing.
 *
 * @param MM_Node* parent - The parent node (the user has to check this isn't null)
 * @return
 * 0 if any allocation error occurred.
 * On success the function returns 1.
 *
 */
int spCreateMinimaxNodeChildren(MM_Node* parent);

/**
 * Frees recursively all memory resources associated with the source tree. If the
 * source root is NULL, then the function does nothing.
 * @param src - the source root of the tree.
 */
void spDestroyMinimaxTree(MM_Node* root);

#endif
