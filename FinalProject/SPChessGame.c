//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
#include <stdio.h>
#include <stdlib.h>

#define STARTING_ROW "RNBQKBNR"
#define FIRST_ROW 0
#define LAST_ROW 7
#define LEFT_MOST_COL 0
#define RIGHT_MOST_COL 7
#define PAWN(color) ('M'+color*('a'-'A'))
#define KNIGHT(color) ('N'+color*('a'-'A'))
#define BISHOP(color) ('B'+color*('a'-'A'))
#define ROOK(color) ('R'+color*('a'-'A'))
#define QUEEN(color) ('Q'+color*('a'-'A'))
#define KING(color) ('K'+color*('a'-'A'))
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define BLANK '_'
#define SEPARATOR '-'
#define FIRST_COLUMN 'A'
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define PAWN_FOR_MOVE 1
#define KNIGHT_FOR_MOVE 2
#define BISHOP_FOR_MOVE 3
#define ROOK_FOR_MOVE 4
#define QUEEN_FOR_MOVE 5
#define KING_FOR_MOVE 6
#define KING_LOC(color) (4+color*N_COLUMNS)
#define QUEEN_LOC(color) (3+color*N_COLUMNS)
#define LEFT_ROOK_LOC(color) (0+color*N_COLUMNS)
#define RIGHT_ROOK_LOC(color) (7+color*N_COLUMNS)
#define LEFT_KNIGHT_LOC(color) (1+color*N_COLUMNS)
#define RIGHT_KNIGHT_LOC(color) (6+color*N_COLUMNS)
#define LEFT_BISHOP_LOC(color) (2+color*N_COLUMNS)
#define RIGHT_BISHOP_LOC(color) (5+color*N_COLUMNS)
#define CHECK_COLOR(color,piece) ((1-2*color)*(piece) < (1-2*color)*('Z'))
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define STAY 0
#define CAPTURED 1
#define THREATENED 1



SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src) {
	if (!src) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	char* starting_row = STARTING_ROW;

	for (int i = 0; i < N_COLUMNS; i++) {
		src->gameBoard[1][i] = PAWN(BLACK);
		src->gameBoard[6][i] = PAWN(WHITE);
		src->gameBoard[0][i] = starting_row[i];
		src->gameBoard[7][i] = CAPITAL_TO_LOW(starting_row[i]);
		for (int j = 0; j < N_ROWS - 4; j++) {
			src->gameBoard[j + 2][i] = '\0';
		}
	}

	for (int i = 0; i < 2*N_COLUMNS; i++) {
		int loc = (i < N_COLUMNS)?(FIRST_ROW):(LAST_ROW);
		//if(i>=N_COLUMNS){int loc = LAST_ROW;}
		loc <<= 4;
		loc |= (i%N_COLUMNS);
		src->locations[i] = loc;
	}

	return SP_CHESS_GAME_SUCCESS;
}

SPChessGame* spChessGameCreate(int historySize) {
	if (historySize <= 0) { // historySize is illegal
		return 0;
	}
	SPChessGame* game = (SPChessGame*) malloc(sizeof(SPChessGame));
	if (!game) { // malloc failed
		return 0;
	}

	game->currentPlayer = WHITE; // set first player
	game->user_color = WHITE;
	game->difficulty = 0;
	game->whiteKingThreaten = false;
	game->blackKingThreaten = false;

	spChessGameResetBoard(game);

	game->history = spArrayListCreate(historySize);
	if (!game->history) { // failed creating spArrayList for history
		free(game);
		return 0;
	}

	return game;
}

SPChessGame* spChessGameCopy(SPChessGame* src) {
	if (!src) {
		return 0;
	}
	SPChessGame* ret = malloc(sizeof(SPChessGame));
	if (!ret) {
		return 0;
	}
	ret->currentPlayer = src->currentPlayer;
	ret->difficulty = src->difficulty;
	ret->user_color = src->user_color;
	ret->whiteKingThreaten = src->whiteKingThreaten;
	ret->blackKingThreaten = src->blackKingThreaten;


	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
			ret->gameBoard[i][j] = src->gameBoard[i][j];
		}
		for (int j = 0; j < 2; j++) {
			ret->locations[i + j] = src->locations[i + j];
		}
	}

	ret->history = spArrayListCopy(src->history);
	if (!ret->history) {
		free(ret);
		return 0;
	}

	return ret;
}

void spChessGameDestroy(SPChessGame* src) {
	if (!src) {
		return;
	}

	spArrayListDestroy(src->history);
	free(src);
}


SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* src, int move){
	char currPosition = spChessGameGetCurrPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	char piece = src->gameBoard[currRow][currColumn];

	if(!piece){return SP_CHESS_GAME_NO_PIECE_IN_POSITION;}

	//check invalid position? if [currRow][currColumn] == [destRow][destColumn]?

	char destPosition = spChessGameGetDestPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char captured = src->gameBoard[destRow][destColumn];

	if(destRow<0||destRow>=N_ROWS||destColumn<0||destColumn>=N_COLUMNS){
		return SP_CHESS_GAME_INVALID_POSITION;
	}

	int color = (CHECK_COLOR(WHITE,piece))?(WHITE):(BLACK);
	if(CHECK_COLOR(color,captured)){return SP_CHESS_GAME_ILLEGAL_MOVE;} //can't capture own piece
	char kingCurrLoc = src->locations[KING_LOC(color)];

	if(piece == KING(color)){
		if(abs(destRow-currRow)<=1 && abs(destColumn-currColumn)<=1){
			return spChessGameCheckPotentialThreat(src,move,destPosition);
		}
		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}

	if(piece == PAWN(color)){
		int dir = (color == WHITE)?(-1):(1);  //if pawn is white, can move up, if black otherwise
		int twoStepsRow = (color == WHITE)?(LAST_ROW-1):(FIRST_ROW+1);
		if(!captured){ //we can only move vertically
			if(destColumn == currColumn){
				if(destRow==currRow+dir){ //move one step
					return spChessGameCheckPotentialThreat(src,move,kingCurrLoc);
				}
				if(currRow == twoStepsRow && destRow==currRow+dir*2
						&& !src->gameBoard[currRow+dir][destColumn]){ //check conditions for 2 steps
					return spChessGameCheckPotentialThreat(src,move,kingCurrLoc);
				}
				return SP_CHESS_GAME_ILLEGAL_MOVE;
			}
		}
		if(abs(destColumn-currColumn)==1){ //if we're here, the move captures, check other conditions
			if(destRow==currRow+dir){
				return spChessGameCheckPotentialThreat(src,move,kingCurrLoc);
			}
		}

		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}


	if(abs(destRow-currRow)==abs(destColumn-currColumn)){ //diagonal move
		if(!(piece == QUEEN(color)) || !(piece == BISHOP(color))){//if we're here, piece is'nt king or pawn
			return SP_CHESS_GAME_ILLEGAL_MOVE;
		}

		if(spChessGameCheckDiagonalMove(src,destPosition,currPosition)){
			return spChessGameCheckPotentialThreat(src,move,kingCurrLoc);//if we're here, piece can perform diagonal move
		}
		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}

	if((destRow==currRow)||(destColumn==currColumn)){ //straight line move
		if(!(piece == QUEEN(color)) || !(piece == ROOK(color))){
			return SP_CHESS_GAME_ILLEGAL_MOVE;
		}

		if(spChessGameCheckStraightLineMove(src,destPosition,currPosition)){
			return spChessGameCheckPotentialThreat(src,move,kingCurrLoc); //if we are here, piece can perform straight line move
		}
			return SP_CHESS_GAME_ILLEGAL_MOVE;
	}

	if(piece == KNIGHT(piece)){
		if(spChessGameCheckKnightMove(src,destPosition,currPosition)){
			return spChessGameCheckPotentialThreat(src,move,kingCurrLoc);
		}
			return SP_CHESS_GAME_ILLEGAL_MOVE;

	}


	return SP_CHESS_GAME_ILLEGAL_MOVE; //shouldn't get here
}

SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move,char location){
	SPChessGame* copy = spChessGameCopy(src);
	if(!copy){
		return SP_CHESS_GAME_ALOCATION_ERROR;
	}
	spChessGameSetMove(copy,move);
	if(spChessGameIsPieceThreaten(copy,location)){
		return SP_CHESS_GAME_MOVE_WILL_THREATEN;
	}
	free(copy);
	return SP_CHESS_GAME_SUCCESS;

}


bool spChessGameIsPieceThreaten(SPChessGame* src, char pieceLocation){
	int pieceCol = spChessGameGetColumnFromPosition(pieceLocation);
	int pieceRow = spChessGameGetRowFromPosition(pieceLocation);
	char piece = src->gameBoard[pieceRow][pieceRow];
	int color = (CHECK_COLOR(WHITE,piece))?(WHITE):(BLACK);
	int oppColor = 1-color;  //The color of the opponent

	if(spChessGameCheckStraightLineMove(src,pieceLocation,
			src->locations[LEFT_ROOK_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckStraightLineMove(src,pieceLocation,
			src->locations[RIGHT_ROOK_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckKnightMove(src,pieceLocation,
			src->locations[LEFT_KNIGHT_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckKnightMove(src,pieceLocation,
			src->locations[RIGHT_KNIGHT_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckDiagonalMove(src,pieceLocation,
			src->locations[LEFT_BISHOP_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckDiagonalMove(src,pieceLocation,
			src->locations[RIGHT_BISHOP_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckDiagonalMove(src,pieceLocation,
			src->locations[QUEEN_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckStraightLineMove(src,pieceLocation,
			src->locations[QUEEN_LOC(oppColor)])){
		return true;
	}
	if(spChessGameCheckKingThreat(src,pieceLocation,
			src->locations[KING_LOC(oppColor)])){
		return true;
	}

	int dir = (color == WHITE)?(-1):(+1); //check pawn threat
	int protectedRow = (color == WHITE)?(FIRST_ROW):(LAST_ROW);
	char oppPawn = PAWN(oppColor);
	if(pieceRow == protectedRow){return false;}
	if(src->gameBoard[pieceRow+dir][pieceCol-1]==oppPawn){
		if(pieceCol != LEFT_MOST_COL){return true;}
	}
	if(src->gameBoard[pieceRow+dir][pieceCol+1]==oppPawn){
		if(pieceCol != RIGHT_MOST_COL){return true;}
	}



	return false;
}

bool spChessGameCheckDiagonalMove(SPChessGame* src , char targetLoc , char threatLoc){
	if(!targetLoc || !threatLoc || !src){
		return false;
	}
	int targetCol = spChessGameGetColumnFromPosition(targetLoc);
	int targetRow = spChessGameGetRowFromPosition(targetLoc);
	int threatCol = spChessGameGetColumnFromPosition(threatLoc);
	int threatRow = spChessGameGetRowFromPosition(threatLoc);
	char piece = src->gameBoard[threatRow][threatCol];
	char temp;

		if(abs(threatCol-targetCol)==abs(threatRow-targetRow)){
			int dirCol = ((threatCol-targetCol)>0)?(RIGHT):(LEFT);
			int dirRow = ((threatRow-targetRow)>0)?(DOWN):(UP);
			for(int i = 1; i<abs(threatRow-targetRow)+1 ; i++){
				temp = src->gameBoard[targetRow+i*dirRow][targetCol+i*dirCol];
				if(temp){
					if(temp == piece){return true;}
					break;
				}
			}

		}
	return false;
}

bool spChessGameCheckStraightLineMove(SPChessGame* src , char targetLoc , char threatLoc){
	if(!targetLoc || !threatLoc || !src){
		return false;
	}
	int targetCol = spChessGameGetColumnFromPosition(targetLoc);
	int targetRow = spChessGameGetRowFromPosition(targetLoc);
	int threatCol = spChessGameGetColumnFromPosition(threatLoc);
	int threatRow = spChessGameGetRowFromPosition(threatLoc);
	char piece = src->gameBoard[threatRow][threatCol];
	char temp;

		if(targetCol==threatCol){
			int dirCol = ((threatCol-targetCol)>0)?(RIGHT):(LEFT);
			for(int i = 1; i<abs(threatRow-targetRow)+1 ; i++){
				temp = src->gameBoard[targetRow][targetCol+i*dirCol];
				if(temp){
					if(temp == piece){return true;}
					break;
				}
			}

		}

		if(targetRow==threatRow){
			int dirRow = ((threatRow-targetRow)>0)?(DOWN):(UP);
			for(int i = 1; i<abs(threatRow-targetRow)+1 ; i++){
				temp = src->gameBoard[targetRow+i*dirRow][targetCol];
				if(temp){
					if(temp == piece){return true;}
					break;
				}
			}

		}

	return false;
}

bool spChessGameCheckKnightMove(SPChessGame* src , char targetLoc , char threatLoc){
	if(!targetLoc || !threatLoc || !src){
			return false;
		}
	int targetCol = spChessGameGetColumnFromPosition(targetLoc);
	int targetRow = spChessGameGetRowFromPosition(targetLoc);
	int threatCol = spChessGameGetColumnFromPosition(threatLoc);
	int threatRow = spChessGameGetRowFromPosition(threatLoc);

	if(abs(targetCol-threatCol)==2){
		if(abs(targetRow-threatRow)==3){
			return true;
		}
	}

	if(abs(targetCol-threatCol)==3){
		if(abs(targetRow-threatRow)==2){
			return true;
		}
	}

	return false;
}

bool spChessGameCheckKingThreat(SPChessGame* src , char targetLoc , char threatLoc){
	if(!targetLoc || !threatLoc || !src){
				return false;
			}
	int targetCol = spChessGameGetColumnFromPosition(targetLoc);
	int targetRow = spChessGameGetRowFromPosition(targetLoc);
	int threatCol = spChessGameGetColumnFromPosition(threatLoc);
	int threatRow = spChessGameGetRowFromPosition(threatLoc);

	if(abs(targetCol-threatCol)<=1 && abs(targetRow-threatRow)<=1){
		return true;
	}
	return false;
}


SPArrayList* spChessGameGetMoves(SPChessGame* src, char position){
	int curCol = spChessGameGetColumnFromPosition(position);
	int curRow = spChessGameGetRowFromPosition(position);
	char piece = src->gameBoard[curRow][curCol];
	//if(!piece){
	//	return 0;
	//}
	int color = (CHECK_COLOR(WHITE,piece))?(WHITE):(BLACK);
	SPArrayList* steps = spArrayListCreate(2*N_ROWS);
	if(!steps){
		return 0;
	}

	if(piece == QUEEN(color)||piece == BISHOP(color)){
		spChessGameAddStepsToList(src,steps,position,DOWN,LEFT,color);
	}
	if(piece == QUEEN(color)||piece == ROOK(color)){
		spChessGameAddStepsToList(src,steps,position,STAY,LEFT,color);
	}
	if(piece == QUEEN(color)||piece == BISHOP(color)){
		spChessGameAddStepsToList(src,steps,position,UP,LEFT,color);
	}
	if(piece == QUEEN(color)||piece == ROOK(color)){
		spChessGameAddStepsToList(src,steps,position,DOWN,STAY,color);
	}
	if(piece == QUEEN(color)||piece == ROOK(color)){
		spChessGameAddStepsToList(src,steps,position,UP,STAY,color);
	}
	if(piece == QUEEN(color)||piece == BISHOP(color)){
		spChessGameAddStepsToList(src,steps,position,DOWN,RIGHT,color);
	}
	if(piece == QUEEN(color)||piece == ROOK(color)){
		spChessGameAddStepsToList(src,steps,position,STAY,RIGHT,color);
	}
	if(piece == QUEEN(color)||piece == BISHOP(color)){
		spChessGameAddStepsToList(src,steps,position,UP,RIGHT,color);
	}




	if(piece == PAWN(color)){
		int dir = (color == WHITE)?(-1):(1);  //if pawn is white, can move up, if black otherwise
		int twoStepsRow = (color == WHITE)?(LAST_ROW-1):(FIRST_ROW+1);

		if (curRow+dir < 0 || curRow+dir >= N_ROWS){ //we are at the last line, pawn can't move
			return moves; //empty list
		}

		int move = setMoveCoordinatesToInt(curRow,curCol,curRow+dir,curCol);
		if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
			spArrayListAddLast(setStepCoordinatesToInt(curRow+dir,curCol,0,
					spChessGameCheckPotentialThreat(src,move,spChessGameGetDestPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN)));
		}
		move = setMoveCoordinatesToInt(curRow,curCol,curRow+2*dir,curCol);
		if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
				spArrayListAddLast(setStepCoordinatesToInt(curRow+dir,curCol,0,!CAPTURED));
		}
	}


	return 0;
}

SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* src,SPArrayList* steps, char position, int verDir, int horDir,int color) {
	int curCol = spChessGameGetColumnFromPosition(position);
	int curRow = spChessGameGetRowFromPosition(position);
	int availbleRows = (verDir==LEFT)?(curRow+1):(N_ROWS-curRow);
	if(!verDir){
		availbleRows = N_ROWS; //if verDir==0 we want the # of for loop iterations decided by availbleColumns only
	}
	int availbleColumns = (horDir==UP)?(curCol+1):(N_COLUMNS-curCol);
	if(!horDir){
		availbleColumns = N_COLUMNS;
	}

	for(int i=1 ; i<min(availbleRows,availbleColumns) ;i++){

		int move = setMoveCoordinatesToInt(curRow,curCol,curRow+i*verDir,curCol+i*horDir);
		int piece = src->gameBoard[curRow+i*verDir][curCol+i*horDir];
		int threaten = (spChessGameCheckPotentialThreat(src,move,spChessGameGetDestPositionFromMove(move))==SP_CHESS_GAME_SUCCESS)?(!THREATENED):(THREATENED);

		if(spChessGameCheckPotentialThreat(src,move,src->locations[KING_LOC(color)])==SP_CHESS_GAME_SUCCESS){ //king won't be threaten
			//check allocation error???
			if(!piece){
				spArrayListAddLast(steps,setStepCoordinatesToInt(curRow+i*verDir,curCol+i*horDir,threaten,!CAPTURED));
				continue;
			}
			if(!(CHECK_COLOR(color,piece))){
				spArrayListAddLast(steps,setStepCoordinatesToInt(curRow+i*verDir,curCol+i*horDir,threaten,CAPTURED));
			}
			return SP_CHESS_GAME_SUCCESS; //we get here if piece is not Null
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}
SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* src,SPArrayList* steps, char position,int color){
	int curCol = spChessGameGetColumnFromPosition(position);
	int curRow = spChessGameGetRowFromPosition(position);


}


SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move) {
	move <<= 8;
	char destPosition = spChessGameGetDestPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	char captured = src->gameBoard[destRow][destColumn];
	if (spArrayListIsFull(src->history)) {
		spArrayListRemoveLast(src->history); // make room for new move in history, if needed
	}
	move <<= 8;
	if (captured) {
		if (captured == PAWN(WHITE) || captured == PAWN(BLACK)) {
			move |= PAWN_FOR_MOVE;
		}
		if (captured == KNIGHT(WHITE) || captured == KNIGHT(BLACK)) {
			move |= KNIGHT_FOR_MOVE;
		}
		if (captured == BISHOP(WHITE) || captured == BISHOP(BLACK)) {
			move |= BISHOP_FOR_MOVE;
		}
		if (captured == QUEEN(WHITE) || captured == QUEEN(BLACK)) {
			move |= QUEEN_FOR_MOVE;
		}
		if (captured == KING(WHITE) || captured == KING(BLACK)) {
			move |= KING_FOR_MOVE;
		}
	}
	SP_ARRAY_LIST_MESSAGE message = spArrayListAddFirst(src->history, move); // add move to history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	for (int i = 0; i < N_COLUMNS * 2; i++) {
		if (src->locations[i] == currPosition) {
			src->locations[i] = destPosition;
		}
	}
	src->gameBoard[destRow][destColumn] = src->gameBoard[currRow][currColumn];
	src->gameBoard[currRow][currColumn] = '\0';

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameUndoPrevMove(SPChessGame* src) {
	if (!src) { // src is NULL
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	if (spArrayListSize(src->history) == 0) { // no move to undo
		return SP_CHESS_GAME_NO_HISTORY;
	}
	int move = spArrayListGetFirst(src->history); // get last move made
	SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(src->history); // remove move from history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	char captured = CLEAN_EXCESS_BYTES(move);
	char destPosition = spChessGameGetDestPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrPositionFromMove(move);
	//int currColumn = spChessGameGetColumnFromPosition(currPosition);
	//int currRow = spChessGameGetRowFromPosition(currPosition);
	for (int i = 0; i < N_COLUMNS * 2; i++) {
		if (src->locations[i] == destPosition) {
			src->locations[i] = currPosition;
		}
	}
	src->gameBoard[destRow][destColumn] = captured;
	src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE;
	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* game) {
	if (!game) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	for (int i = 0; i < N_ROWS; i++) {
		printf("%d|", 8 - i);
		for (int j = 0; j < N_COLUMNS; j++) {
			char currChar = game->gameBoard[i][j];
			if (!currChar) {
				printf(" %c", BLANK);
			}
			else {
				printf(" %c",currChar);
			}
		}
		printf(" |\n");
	}

	printf("  ");

	for (int i = 0; i < 2 * N_COLUMNS + 1; i++) {
		printf("%c", SEPARATOR);
	}

	printf("\n  ");

	for (int i = 0; i < N_COLUMNS; i++) {
		printf(" %c", FIRST_COLUMN + i);
	}

	printf("\n");

	return SP_CHESS_GAME_SUCCESS;
}

char spChessGameGetDestPositionFromMove(int move) {
	return (char) CLEAN_EXCESS_BYTES((move >> 8));
}

char spChessGameGetCurrPositionFromMove(int move) {
	return (char) CLEAN_EXCESS_BYTES((move >> 16));
}

int spChessGameGetColumnFromPosition(char position) {
	return (int) (position << 4) >> 4;
}

int spChessGameGetRowFromPosition(char position) {
	return (int) position >> 4;
}

int setMoveCoordinatesToInt(int curRow,int curCol, int destRow, int destCol){
	int move = 0;
	//move = (curRow|move)<<4;
	//move = (curCol|move)<<4;
	//move = (destRow|move)<<4;
	//move = (destCol|move)<<4;
	return move;
}

int setStepCoordinatesToInt(int destRow,int destCol, int threaten, int capture){
	int posMove = 0;
	posMove = (destRow|posMove)<<4;
	posMove = (destCol|posMove)<<4;
	posMove = (threaten|posMove)<<4;
	posMove = (capture|posMove)<<4;
	return posMove;
}
