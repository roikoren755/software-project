/*
 * SPMinimax.c
 *
 *  Created on: 26 ���� 2017
 *      Author: user
 */

#include "SPFIARGame.h"
#include <limits.h>
#include <stdio.h>
#include "SPFIARGame.h"
#include "SPMinimaxNode.h"

#define Error -1


int spMinimaxSuggestMove(SPFiarGame* currentGame,unsigned int maxDepth){
	if(currentGame==NULL | maxDepth<=0){
			return -1;
		}
	MM_Node* root = MMCreateTree(currentGame,maxDepth);
	if (!root){return Error;}
	int next_move = spMaxIndex(root);
	TreeDesrtoy(root);
	return next_move;

}

MM_Node* MMCreateTree(SPFiarGame* currentGame,unsigned int maxDepth){
	SPFiarGame* currentGameCopy = spFiarGameCopy(currentGame);
	if(!currentGameCopy){return NULL;}
	MM_Node *root = createNode(currentGameCopy,maxDepth,true,true,0);
	int success = createNodeChilds(root);
	if(!success){return NULL;}
	return root;
}

int createNodeChilds(MM_Node* parent){
	if (!parent){return 0;}
	if (parent.height == 0){return 1;}
	int height = parent->height;

	SPFiarGame* game = parent.game;
	for(int i=0;i<Node_Childs_Num;i++){
		SPFiarGame* nextGame = spFiarGameCopy(parent.game);
		if (!nextGame){return 0;}

		if (!spFiarGameIsValidMove(nextGame,i)){
			MM_Node* child = createNode(nextGame,0,true,false,0);
			if (!child){return 0;}
			parent->childs[i] = child;
			break;
		}

		spFiarGameSetMove(nextGame,i);

		if(spFiarCheckWinner(nextGame)){
			MM_Node* child = createNode(nextGame,0,true,true,0);
			if (!child){return 0;}
			parent->childs[i] = child;
			break;
		}

		MM_Node* child = createNode(nextGame,height-1,!parent.turn,true,0);
		if (!child){return 0;}
		parent->childs[i] = child;
		if(!createNodeChilds(child)){return 0;}
	}
	return 1;
}

void TreeDesrtoy(MM_Node* root){
	if(root->height==0){
		destroyNode(root);
	}
	else{
		for(int i=0;i<Node_Childs_Num;i++){
			TreeDesrtoy(root->childs[i]);
		}
	}

}
