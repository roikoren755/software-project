#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_
#include <stdbool.h>

//Put all declarations and constants here

#define MAX_NODE_CHILDREN_NUM 7

typedef struct Minimax_Node_t {
	SPFiarGame* game;
	unsigned int depth;
	struct Minimax_Node_t* children[MAX_NODE_CHILDREN_NUM];
} Minimax_Node;

/**
 * Creates a node with the given parameters:
 * @param game - the game state.
 * @param height - the height of the node.
 * @param turn - the turn of the user the node's tree is created for.
 * @param valid - if the node is valid.
 *
 * @return
 * NULL, if an allocation error occurred or game is NULL.
 * An instant of a Minimax Node otherwise.
 */
Minimax_Node* spCreateMinimaxNode(SPFiarGame* game, unsigned int depth);

/**
 * Frees all memory resources associated with the source node. If the
 * source node is NULL, then the function does nothing.
 * @param src - the source node
 */
void spDestroyMinimaxNode(Minimax_Node* node);

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
Minimax_Node* spMinimaxCreateTree(SPFiarGame* currentGame, unsigned int maxDepth);

int spCreateMinimaxNodeChildren(Minimax_Node* parent, unsigned int maxDepth);

/**
 * Given a node, evaluates the score of each of the node's children (by calling
 * spEvalNode on them).
 * returns the index of the max.
 *
 * @param Minimax_Node* root - The node (the user has to check this isn't null)
 * @return the index of the child with the highest score.
 */
int spGetMaximumScoreIndex(Minimax_Node* root, int rootPlayer);

int spGetMinimumScoreIndex(Minimax_Node* root, int rootPlayer);

int spGetBestScoreIndex(Minimax_Node* root);

/**
 * Given a node, evaluates the score of the node according to his children.
 * if the node's height is 0, calls scoreBoard on the node's game.
 * otherwise, calls spMaxScore or spMinScore on the node's children.
 *
 * @param Minimax_Node* node - The node (the user has to check this isn't null)
 * @return the score of the node.
 */
int spEvaluateMinimaxNode(Minimax_Node* node, int rootPlayer);

/**
 * Given a node, evaluates the score of each child node (by calling
 * spEvalNode on it) and returns the max score.
 *
 * @param Minimax_Node* node - The node (the user has to check this isn't null)
 * @return the score of the max node.
 */
int spGetMaximumScore(Minimax_Node* node, int rootPlayer);

/**
 * Given a node, evaluates the score of each child node (by calling
 * spEvalNode on it) and returns the min score.
 *
 * @param Minimax_Node* node - The node (the user has to check this isn't null)
 * @return the score of the min node.
 */
int spGetMinimumScore(Minimax_Node* node, int rootPlayer);

/**
 * Given a game, evaluates the score of the game's board according to the scoring
 * function described in the assignment instructions.
 * @param src - the game.
 * @return the score of the game's board.
 */
int scoreBoard(SPFiarGame* src);

#endif
