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
#define SEPARATOR '-'
#define FIRST_COLUMN 'A'
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define KING_LOC(color) (4+color*3*N_COLUMNS)
#define QUEEN_LOC(color) (3+color*3*N_COLUMNS)
#define LEFT_ROOK_LOC(color) (0+color*3*N_COLUMNS)
#define RIGHT_ROOK_LOC(color) (7+color*3*N_COLUMNS)
#define LEFT_KNIGHT_LOC(color) (1+color*3*N_COLUMNS)
#define RIGHT_KNIGHT_LOC(color) (6+color*3*N_COLUMNS)
#define LEFT_BISHOP_LOC(color) (2+color*3*N_COLUMNS)
#define RIGHT_BISHOP_LOC(color) (5+color*3*N_COLUMNS)
#define CHECK_COLOR(color,piece) ((1-2*color)*(piece) < (1-2*color)*('Z'))
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define STAY 0
#define CAPTURED 1
#define THREATENED 1
#define HISTORY_SIZE 3

/***
 * Resets src's game board to the starting set-up
 * @param src
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src) {
	if (!src) { // src is NULL
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	char* starting_row = STARTING_ROW;

	for (int i = 0; i < N_COLUMNS; i++) {
		src->gameBoard[1][i] = PAWN; // Put black pawns on the board
		src->gameBoard[6][i] = CAPITAL_TO_LOW(PAWN); // Put white pawns on the board
		src->gameBoard[0][i] = starting_row[i]; // Put other pieces on the board
		src->gameBoard[7][i] = CAPITAL_TO_LOW(starting_row[i]); // For whitey too

		for (int j = 0; j < N_ROWS - 4; j++) {
			src->gameBoard[j + 2][i] = '\0'; // Everything else is empty
		}

        int location = FIRST_ROW << 4;
        location |= i; // Set location as 4 bits row and 4 bits column for black STARTING_ROW
        src->locations[i] = location;

        location = (FIRST_ROW + 1) << 4;
        location |= i; // Set location for pawns
        src->locations[i + N_COLUMNS] = location;

        location = LAST_ROW << 4;
        location |= i; // Set location for white STARTING_ROW
        src->locations[i + 3 * N_COLUMNS] = location;

        location = (LAST_ROW - 1) << 4;
        location |= i; // Set location for white pawns
        src->locations[i + 2 * N_COLUMNS] = location;
	}

	return SP_CHESS_GAME_SUCCESS;
}

int setMoveCoordinatesToInt(int curRow,int curCol, int destRow, int destCol){
	int move = 0;
	move = (curRow | move) << 4;
	move = (curCol | move) << 4;
	move = (destRow | move) << 4;
	move = (destCol | move) << 4;
	return move;
}

int setStepCoordinatesToInt(int destRow,int destCol, int threaten, int capture){
	int posMove = 0;
	posMove = (destRow | posMove) << 4;
	posMove = (destCol | posMove) << 4;
	posMove = (threaten | posMove) << 4;
	posMove = (capture | posMove) << 4;
	return posMove;
}

SPChessGame* spChessGameCreate() {
	SPChessGame* game = (SPChessGame*) malloc(sizeof(SPChessGame));
	if (!game) { // malloc failed
		return 0;
	}

	game->currentPlayer = WHITE; // Defaults
	game->userColor = WHITE;
	game->difficulty = 2;
	game->whiteKingThreaten = false;
	game->blackKingThreaten = false;
	game->gameMode = 1;

	spChessGameResetBoard(game);

	game->history = spArrayListCreate(HISTORY_SIZE);
	if (!game->history) { // Failed creating spArrayList for history
		free(game);
		return 0;
	}

	return game;
}

SPChessGame* spChessGameCopy(SPChessGame* src) {
	if (!src) { // src is NULL
		return 0;
	}

	SPChessGame* ret = (SPChessGame*) malloc(sizeof(SPChessGame));
	if (!ret) { // malloc failed
		return 0;
	}

	ret->currentPlayer = src->currentPlayer; // Copy what needs to be copied
	ret->difficulty = src->difficulty;
	ret->userColor = src->userColor;
	ret->whiteKingThreaten = src->whiteKingThreaten;
	ret->blackKingThreaten = src->blackKingThreaten;
    ret->gameMode = src->gameMode;

	for (int i = 0; i < N_ROWS; i++) {
		for (int j = 0; j < N_COLUMNS; j++) {
            if (j < 2) {
                ret->locations[i + j] = src->locations[i + j]; // Copy locations
            }

			ret->gameBoard[i][j] = src->gameBoard[i][j]; // And game board
		}
	}

	ret->history = spArrayListCopy(src->history); // Copy history
	if (!ret->history) {
		free(ret);
		return 0;
	}

	return ret;
}

void spChessGameDestroy(SPChessGame* src) {
	if (src) {
        spArrayListDestroy(src->history);
        free(src);
	}
}


SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* src, int move){
	char currPosition = spChessGameGetCurrentPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	char piece = src->gameBoard[currRow][currColumn];

	if(!piece){return SP_CHESS_GAME_NO_PIECE_IN_POSITION;}

	//check invalid position? if [currRow][currColumn] == [destRow][destColumn]?

	char destPosition = spChessGameGetDestinationPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char captured = src->gameBoard[destRow][destColumn];

	if(destRow==currRow && destColumn==currColumn){ // can't stay in place
		return SP_CHESS_GAME_INVALID_POSITION;
	}

	if(destRow<0||destRow>=N_ROWS||destColumn<0||destColumn>=N_COLUMNS){ //not in range
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


bool spChessGameIsPieceThreatened(SPChessGame* src, char pieceLocation){
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
		spChessGameAddStepsToList(src,steps,position,UP,LEFT,color);
		spChessGameAddStepsToList(src,steps,position,DOWN,RIGHT,color);
		spChessGameAddStepsToList(src,steps,position,UP,RIGHT,color);
	}

	if(piece == QUEEN(color)||piece == ROOK(color)){
		spChessGameAddStepsToList(src,steps,position,STAY,LEFT,color);
		spChessGameAddStepsToList(src,steps,position,DOWN,STAY,color);
		spChessGameAddStepsToList(src,steps,position,UP,STAY,color);
		spChessGameAddStepsToList(src,steps,position,STAY,RIGHT,color);
	}

	if(piece == PAWN(color)){
		spChessGameAddPawnStepsToList(src,steps,position,color);
	}

	if(piece == KNIGHT(color)){
		spChessGameAddKnightStepsToList(src,steps,position,color);
	}

	if(piece == KING(color)){
		spChessGameAddKingStepsToList(src,steps,position,color);
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
		int threaten = (spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move))==SP_CHESS_GAME_SUCCESS)?(!THREATENED):(THREATENED);

		if(spChessGameCheckPotentialThreat(src,move,src->locations[KING_LOC(color)])==SP_CHESS_GAME_SUCCESS){ //king won't be threaten
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
	int move;
	for(int i = -1; i<=1;i+=2){
		for(int i = -1; i<=1;i+=2){
			move = setMoveCoordinatesToInt(curRow,curCol,curRow+2*i,curCol+3*i);
			if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
				spArrayListAddLast(setStepCoordinatesToInt(curRow+2*i,curCol+3*i,
					spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN))
					,src->gameBoard[curRow+2*i][curCol+3*i]);
			}
		}
	}
	for(int i = -1; i<=1;i+=2){
		for(int i = -1; i<=1;i+=2){
			move = setMoveCoordinatesToInt(curRow,curCol,curRow+3*i,curCol+2*i);
			if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
				spArrayListAddLast(setStepCoordinatesToInt(curRow+3*i,curCol+2*i,
					spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN))
					,src->gameBoard[curRow+3*i][curCol+2*i]);
			}
		}
	}



}

SP_CHESS_GAME_MESSAGE spChessGameAddKingStepsToList(SPChessGame* src,SPArrayList* steps, char position,int color){
	int curCol = spChessGameGetColumnFromPosition(position);
	int curRow = spChessGameGetRowFromPosition(position);
	int move;
	for(int i = -1; i<=1;i++){
		for(int i = -1; i<=1;i++){
			move = setMoveCoordinatesToInt(curRow,curCol,curRow+i,curCol+i);
			if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
				spArrayListAddLast(setStepCoordinatesToInt(curRow+i,curCol+i,
					spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN))
					,src->gameBoard[curRow+i][curCol+i]);
			}
		}
	}
}

SP_CHESS_GAME_MESSAGE spChessGameAddPawnStepsToList(SPChessGame* src,SPArrayList* steps, char position,int color){
	int curRow = spChessGameGetRowFromPosition(position);
	int curCol = spChessGameGetColumnFromPosition(position);
	int dir = (color == WHITE)?(UP):(DOWN);  //if pawn is white, can move up, if black otherwise

	int move = setMoveCoordinatesToInt(curRow,curCol,curRow+dir,curCol);
	for(int i = 1; i<=2;i++){
		move = setMoveCoordinatesToInt(curRow,curCol,curRow+i*dir,curCol);
		if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
			spArrayListAddLast(setStepCoordinatesToInt(curRow+i*dir,curCol,
					spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN))
					,!CAPTURED);
		}
		else{break;} //if 1 step isn't valid, 2 steps also aren't
	}
	for(int i = -1; i<=1;i+=2){
	move = setMoveCoordinatesToInt(curRow,curCol,curRow+dir,curCol+i);
	if(spChessGameIsValidMove(src,move)==SP_CHESS_GAME_SUCCESS){
			spArrayListAddLast(setStepCoordinatesToInt(curRow+dir,curCol+i,
				spChessGameCheckPotentialThreat(src,move,spChessGameGetDestinationPositionFromMove(move)==SP_CHESS_GAME_MOVE_WILL_THREATEN))
				,CAPTURED);
	}
}

}



SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move) {
	if (!src) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	move <<= 8; // Prepare move for history
	char destPosition = spChessGameGetDestinationPositionFromMove(move);
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrentPositionFromMove(move);
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);
	char captured = src->gameBoard[destRow][destColumn]; // Get (potentially) captured piece
    move |= captured; // Add captured piece to move for history

	SP_ARRAY_LIST_MESSAGE message;
	if (spArrayListIsFull(src->history)) {
		message = spArrayListRemoveLast(src->history); // Make room for new move in history, if needed
		if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
	}

	message = spArrayListAddFirst(src->history, move); // Add move to history
	if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // Shouldn't happen
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	for (int i = 0; i < N_COLUMNS * 4; i++) {
		if (src->locations[i] == destPosition) { // Change captured piece's location
			src->locations[i] = 0;
		}

		if (src->locations[i] == currPosition) { // Change moved piece's location
			src->locations[i] = destPosition;
		}
	}

	src->gameBoard[destRow][destColumn] = src->gameBoard[currRow][currColumn]; // Set board accordingly
	src->gameBoard[currRow][currColumn] = '\0';

	src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE; // Change current player

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameUndoMove(SPChessGame* src) {
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

	char captured = CLEAN_EXCESS_BYTES(move); // Get captured piece
	char destPosition = spChessGameGetDestinationPositionFromMove(move); // Location of captured piece
	int destColumn = spChessGameGetColumnFromPosition(destPosition);
	int destRow = spChessGameGetRowFromPosition(destPosition);
	char currPosition = spChessGameGetCurrentPositionFromMove(move); // Location of moved piece
	int currColumn = spChessGameGetColumnFromPosition(currPosition);
	int currRow = spChessGameGetRowFromPosition(currPosition);

	src->gameBoard[currRow][currColumn] = src->gameBoard[destRow][destColumn]; // Set board
	src->gameBoard[destRow][destColumn] = captured;
	int currentPlayer = src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE; // And current player

    for (int i = 0; i < N_COLUMNS * 4; i++) { // Update location for moved piece
        if (src->locations[i] == destPosition) {
            src->locations[i] = currPosition;
        }
    }

	if (captured == currentPlayer ? CAPITAL_TO_LOW(PAWN) : PAWN) { // Update location if pawn was captured
        int startIndex = currentPlayer ? 2 * N_COLUMNS : N_COLUMNS; // Pawn locations by color
		for (int i = 0; i < N_COLUMNS; i++) {
			if (!src->locations[i + startIndex]) {
				src->locations[i + startIndex] = destPosition;
			}
		}
	}

	int index;
	if (captured == currentPlayer ? CAPITAL_TO_LOW(KNIGHT) : KNIGHT) { // Update location if knight was captured
		if (!src->locations[RIGHT_KNIGHT_LOC(currentPlayer)]) {
			index = RIGHT_KNIGHT_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_KNIGHT_LOC(currentPlayer)]) {
			index = LEFT_KNIGHT_LOC(currentPlayer);
		}
	}

	if (captured == currentPlayer ? CAPITAL_TO_LOW(ROOK) : ROOK) { // Rook taken
		if (!src->locations[RIGHT_ROOK_LOC(currentPlayer)]) {
			index = RIGHT_ROOK_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_ROOK_LOC(currentPlayer)]) {
			index = LEFT_ROOK_LOC(currentPlayer);
		}
	}

	if (captured == currentPlayer ? CAPITAL_TO_LOW(BISHOP) : BISHOP) { // Bishop
		if (!src->locations[RIGHT_BISHOP_LOC(currentPlayer)]) {
			index = RIGHT_BISHOP_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_BISHOP_LOC(currentPlayer)]) {
			index = LEFT_BISHOP_LOC(currentPlayer);
		}
	}

	if (captured == currentPlayer ? CAPITAL_TO_LOW(QUEEN) : QUEEN) { // QUEEN???
		index = QUEEN_LOC(currentPlayer);
	}

	src->locations[index] = destPosition;

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameFprintBoard(SPChessGame* src, FILE* file) {
	if (!src || !file) { // src or file are NULL
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	for (int i = 0; i < N_ROWS; i++) {
		fprintf(file, "%d|", N_ROWS - i); // Print row number
		for (int j = 0; j < N_COLUMNS; j++) {
			char currChar = src->gameBoard[i][j];
            fprintf(file, " %c", currChar ? currChar : BLANK); // Print piece if it's there
		}
		fprintf(file, " |\n"); // End row
	}

	fprintf(file, "  "); // Some spaces

	for (int i = 0; i < 2 * N_COLUMNS + 1; i++) {
		fprintf(file, "%c", SEPARATOR); // Close the board at the bottom
	}

	fprintf(file, "\n  "); // WHITE SPACE

	for (int i = 0; i < N_COLUMNS; i++) {
		fprintf(file, " %c", FIRST_COLUMN + i); // Column letters
	}

	fprintf(file, "\n"); // Voila

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* src) {
	return spChessGameFprintBoard(src, stdout);
}

int spChessGameEnded(SPChessGame* game) {
	if (!game) {
		return -1;
	}

	int gameEnded = 1;
	int index;
	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		index = i + game->currentPlayer * 2 * N_COLUMNS;
		if (game->locations[index]) {
			SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]);
			if (!possibleMoves) {
				return -1;
			}

			if (!spArrayListIsEmpty(possibleMoves)) { // There's a piece that can move
				gameOver = 0; // Game isn't over
				spArrayListDestroy(possibleMoves);
				break;
			}

			spArrayListDestroy(possibleMoves);
		}
	}

	return gameEnded;
}
