#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_
#define Node_Childs_Num 7


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
void destroyNode(MM_Node* node);

int scoreBoard(SPFiarGame* src);
int spEvalGame(SPFiarGame* game, unsigned int height, bool turn);
int spMaxIndex(SPFiarGame* game);
int spMaxScore(SPFiarGame* game, unsigned int height);
int spMinScore(SPFiarGame* game, unsigned int height);

#endif
