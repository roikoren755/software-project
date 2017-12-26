#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_

//Put all decleartions and constants here

#define Node_Childs_Num 7

typedef struct MM_Node_t {
	SPFiarGame* game;
	int height;
	bool valid;
	bool turn;
	int score;
	MM_Node* childs;
} MM_Node;


MM_Node* createNode(SPFiarGame* game,int Depth,bool turn,bool valid,int score);
void destroyNode(MM_Node* node);
int scoreBoard(SPFiarGame* src);
int spEvalnode(MM_Node* node);
int spMaxIndex(MM_Node* root);
int spMaxScore(MM_Node* childs);
int spMinScore(MM_Node* childs);

#endif
