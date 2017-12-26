#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_
#define SP_FIAR_GAME_N_COLUMNS 7


typedef struct MM_Node_t {
	SPFiarGame* game;
	int height;
	bool valid;
	bool turn;
	int score;
	MM_Node* childs;
} MM_Node;

//Put all decleartions and constants here

MM_Node* createNode(SPFiarGame* game,int Depth,bool turn,bool valid,int score);
MM_Node* copyNode(MM_Node* node);
void destroyNode(MM_Node* node);

int scoreBoard(SPFiarGame* src);
int spEvalGame(SPFiarGame* game, unsigned int height, bool turn);
int spMaxIndex(SPFiarGame* game);
int spMaxScore(SPFiarGame* game, unsigned int height);
int spMinScore(SPFiarGame* game, unsigned int height);

#endif
