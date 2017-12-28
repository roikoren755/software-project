#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_
#include <stdbool.h>

//Put all declarations and constants here

#define MAX_NODE_CHILDREN_NUM 7

typedef struct MM_Node_t {
	SPFiarGame* game;
	int height;
	bool valid;
	bool turn;
	struct MM_Node_t* children[MAX_NODE_CHILDREN_NUM];
} MM_Node;

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
MM_Node* createMinimaxNode(SPFiarGame* game,int height,bool turn,bool valid);

/**
 * Frees all memory resources associated with the source node. If the
 * source node is NULL, then the function does nothing.
 * @param src - the source node
 */
void destroyMinimaxNode(MM_Node* node);

/**
 * Given a node, evaluates the score of each of the node's children (by calling
 * spEvalNode on them).
 * returns the index of the max.
 *
 * @param MM_Node* root - The node (the user has to check this isn't null)
 * @return the index of the child with the highest score.
 */
int spGetMaximumScoreIndex(MM_Node* root);

/**
 * Given a node, evaluates the score of the node according to his children.
 * if the node's height is 0, calls scoreBoard on the node's game.
 * otherwise, calls spMaxScore or spMinScore on the node's children.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 * @return the score of the node.
 */
int spEvaluateMinimaxNode(MM_Node* node);

/**
 * Given a node, evaluates the score of each child node (by calling
 * spEvalNode on it) and returns the max score.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 * @return the score of the max node.
 */
int spGetMaximumScore(MM_Node* node);

/**
 * Given a node, evaluates the score of each child node (by calling
 * spEvalNode on it) and returns the min score.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 * @return the score of the min node.
 */
int spGetMinimumScore(MM_Node* node);

/**
 * Given a game, evaluates the score of the game's board according to the scoring
 * function described in the assignment instructions.
 * @param src - the game.
 * @return the score of the game's board.
 */
int scoreBoard(SPFiarGame* src);
#endif
