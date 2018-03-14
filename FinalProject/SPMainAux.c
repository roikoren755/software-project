//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <string.h>
#include "SPChessGame.h"
#include "SPChessParser.h"
//#include "SPMinimax.h"
//#include "SPMainAux.h"


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

SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move, char location) {
	if (!src) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int lastMove = 0;
	int full = 0;
	int threatened = 0;
	if (spArrayListIsFull(src->history)) {
		lastMove = spArrayListGetLast(src->history);
		full = 1;
	}

	SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(src, move);
	if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	if (spChessGameIsPieceThreatened(src, location)) {
		threatened = 1;
	}

	gameMessage = spChessGameUndoMove(src);
	if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	if (full) {
		SP_ARRAY_LIST_MESSAGE message = spArrayListAddLast(src->history, lastMove);
		if (message != SP_ARRAY_LIST_SUCCESS) {
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

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