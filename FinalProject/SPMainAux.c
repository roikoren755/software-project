//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <string.h>
#include "SPChessGame.h"
#include "SPChessParser.h"
#include "SPMinimax.h"
#include "SPMainAux.h"


int min(int a,int b){
	if(a<b){return a;}
	else{return b;}
}

int max(int a,int b){
	if(a>b){return a;}
		else{return b;}
	}

//int abs(int a){
//	if(a>0){return a;}
//	else{return -a;}
//}


SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move,char location) {
	SPChessGame* copy = spChessGameCopy(src);
	if (!copy) {
		return SP_CHESS_GAME_ALOCATION_ERROR;
	}
	spChessGameSetMove(copy,move);
	if (spChessGameIsPieceThreatened(copy,location)) {
		return SP_CHESS_GAME_MOVE_WILL_THREATEN;
	}
	spChessGameDestroy(copy);
	return SP_CHESS_GAME_SUCCESS;
}

int spRunGame(SPChessGame* src) {

}
