#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_

//Put all decleartions and constants here

#define Node_Childs_Num 7

typedef struct MM_Node_t {
	SPFiarGame* game;
	int height;
	bool valid;
	bool turn;
	MM_Node* childs;
} MM_Node;

/**
 *  Creates a node with the given parameters:
 *  @param game - the game state.
 *  @param height - the height of the node.
 *  @param turn - the turn of the user the node's tree is created for.
 *  @param valid - if the node is valid.
 *
 *  @return
 *  NULL, if an allocation error occurred game is NULL.
 *  An instant of an array list otherwise.
 */
MM_Node* createNode(SPFiarGame* game,int height,bool turn,bool valid);
/**
 * Frees all memory resources associated with the source node. If the
 * source node is NULL, then the function does nothing.
 * @param src - the source node
 */
void destroyNode(MM_Node* node);
/**
 *  given a node, evaluates the score of each of the node's childs (by calling
 *  spEvalnode on them).
 *  returns the index of the max.
 *
 * @param MM_Node* root - The node (the user has to check this isn't null)
 * @return
 *  the score of the node.
 */
int spMaxIndex(MM_Node* root);
/**
 *  given a node, evaluates the score of the node according to his childs.
 *  if the node's height is 0, calls scoreBoard on the node's game.
 *  otherwise, calls spMaxScore or spMinScore on the node's childs.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 *  @return
 *  the score of the node.
 */
int spEvalnode(MM_Node* node);
/**
 *  given a node's array, evaluates the score of each node (by calling
 *  spEvalnode on it) and returns the max score.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 *  @return
 *  the score of the max node.
 */
int spMaxScore(MM_Node* childs);
/**
 *  given a node's array, evaluates the score of each node (by calling
 *  spEvalnode on it) and returns the min score.
 *
 * @param MM_Node* node - The node (the user has to check this isn't null)
 *  @return
 *  the score of the min node.
 */
int spMinScore(MM_Node* childs);
/**
 *  given a game, evaluates the score of the game's board according to the scoring
 *  function described in the assignment instructions.
 *  @param src - the game.
 *  @return
 *  the score of the game's board.
 */
int scoreBoard(SPFiarGame* src);
#endif
