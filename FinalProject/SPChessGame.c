//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
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
#define BLANK '_'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
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
#define CAPTURES 1
#define HISTORY_SIZE 3
#define COL_NUM_TO_LETTER(column) (column+'A')

int setMoveCoordinatesToInt(int currentRow, int currentColumn, int destinationRow, int destinationColumn) {
	int move = 0;
	move = (currentRow | move) << 4;
	move = (currentColumn | move) << 4;
	move = (destinationRow | move) << 4;
	move = (destinationColumn | move) << 4;
	return move;
}

int setStepCoordinatesToInt(int destinationRow, int destinationColumn, int threatened, int captures) {
	int step = 0;
	step = (destinationRow | step) << 4;
	step = (destinationColumn | step) << 4;
	step = (threatened | step) << 4;
	step = (captures | step) << 4;
	return step;
}

SP_CHESS_GAME_MESSAGE spChessGameAddSingleStepToList(SPChessGame* src, SPArrayList* steps, int move, int color) {
	if (!src || !steps) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	char destinationPosition = spChessGameGetDestinationPositionFromMove(move << 8);
	int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
	int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
	char piece = src->gameBoard[destinationRow][destinationColumn];
	int threatened = spChessGameCheckPotentialThreat(src, move, destinationPosition) == SP_CHESS_GAME_MOVE_WILL_THREATEN;
	int kingNotThreatened = spChessGameCheckPotentialThreat(src, move, src->locations[KING_LOC(color)]) == SP_CHESS_GAME_SUCCESS;
	int step;

	if (kingNotThreatened) { // king won't be threatened
		if (!piece) {
			step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, !CAPTURES);
			spArrayListAddLast(steps, step);
		}
		else if (!(CHECK_COLOR(color,piece))) {
			step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, CAPTURES);
			spArrayListAddLast(steps, step);
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* src, SPArrayList* steps, char position, int verticalDirection, int horizontalDirection, int color) {
	if (!src || !steps) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int currentColumn = spChessGameGetColumnFromPosition(position);
	int currentRow = spChessGameGetRowFromPosition(position);
	int destinationRow;
	int destinationColumn;
	int move;
	char piece;

	int availbleRows = verticalDirection == LEFT ? currentRow + 1 : N_ROWS - currentRow;
	if (!verticalDirection) {
		availbleRows = N_ROWS; // if verticalDirection == 0 we want the # of for loop iterations decided by availbleColumns only
	}

	int availbleColumns = horizontalDirection == UP ? currentColumn + 1 : N_COLUMNS - currentColumn;
	if (!horizontalDirection) {
		availbleColumns = N_COLUMNS;
	}

	for (int i = 1; i < min(availbleRows, availbleColumns); i++) {
		destinationRow = currentRow + i * verticalDirection;
		destinationColumn = currentColumn + i * horizontalDirection;
		move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

		if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
		piece = src->gameBoard[destinationRow][destinationColumn];
		if (piece) {
			return SP_CHESS_GAME_SUCCESS;
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
	if (!src || !steps) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	int currentColumn = spChessGameGetColumnFromPosition(position);
	int currentRow = spChessGameGetRowFromPosition(position);
	int move;
	int destinationRow;
	int destinationColumn;

	for (int i = -1; i < 2; i += 2) {
		for (int j = -1; j < 2; j += 2) {
			destinationRow = currentRow + i;
			destinationColumn = currentColumn + 2 * i;
			move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);
			if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
				if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
					return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
			}
			destinationRow = currentRow + 2 * i;
			destinationColumn = currentColumn + i;
			move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);
			if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
				if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
					return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
			}
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddKingStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
	if (!src || !steps) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	int currentColumn = spChessGameGetColumnFromPosition(position);
	int currentRow = spChessGameGetRowFromPosition(position);
	int move;
	int destinationColumn;
	int destinationRow;

	for (int i = -1; i < 2; i++) {
		for (int i = -1; i < 2; i++) {
			destinationColumn = currentColumn + i;
			destinationRow = currentRow + i;
			move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);
			if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
				if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
					return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
			}
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddPawnStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
	if (!src || !steps) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	int currentRow = spChessGameGetRowFromPosition(position);
	int currentColumn = spChessGameGetColumnFromPosition(position);
	int direction = color == WHITE ? UP : DOWN;  // if pawn is white, can move up, if black otherwise
	int move;
	int destinationRow;
	int destinationColumn;

	for (int i = 1; i < 3; i++) {
		destinationRow = currentRow + i * direction;
		move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, currentColumn);
		if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
			if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
				return SP_CHESS_GAME_INVALID_ARGUMENT;
			}
		}
		else {
			break;
		} // if 1 step isn't valid, 2 steps also aren't
	}

	destinationRow = currentRow + direction;
	for (int i = -1; i < 2; i+=2) {
		destinationColumn = currentColumn + i;
		move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);
		if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
			if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
				return SP_CHESS_GAME_INVALID_ARGUMENT;
			}
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

int spChessGameGetRowFromStep(int step){
	return step >> 12; //Get bits [12-15]
}
int spChessGameGetColFromStep(int step){
	return ((step << 20) >> 28); //Get bits [8-11]
}
int spChessGameWillStepThreaten(int step){
	return ((step << 24) >> 28); //Get bits [4-7]
}
int spChessGameWillStepCapture(int step){
	return ((step << 28) >> 28); //Get 4 right most bits
}

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
		src->gameBoard[1][i] = PAWN(BLACK); // Put black pawns on the board
		src->gameBoard[6][i] = PAWN(WHITE); // Put white pawns on the board
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

SP_CHESS_GAME_MESSAGE spChessCheckGameState(SPChessGame* src , int color){
	int kingThreatenred = spChessGameIsPieceThreatened(src,src->locations[KING_LOC(color)]);
	int gameOver = 1;
	int index;
	for (int i = 0; i < 2 * N_COLUMNS; i++) {
		      index = i + color * 2 * N_COLUMNS;
		      if (src->locations[index]) {
		    	  SPArrayList* possibleMoves = spChessGameGetMoves(src, src->locations[index]);
		          if (!spArrayListIsEmpty(possibleMoves)) { // There's a piece that can move
		             gameOver = 0; // Game isn't over
		             break;
		          }
		      }
		 }
	if(gameOver){
		if(kingThreatenred){
			return SP_CHESS_GAME_CHECKMATE;
		}
		else{
			return SP_CHESS_GAME_DRAW;
		}
	}
	if(kingThreatenred){
		return SP_CHESS_GAME_CHECK;
	}

	return 0;
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
		int dir = (color == WHITE)?(UP):(DOWN);  //if pawn is white, can move up, if black otherwise
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

	return threatened ? SP_CHESS_GAME_MOVE_WILL_THREATEN : SP_CHESS_GAME_SUCCESS;
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

	int dir = (color == WHITE)?(UP):(DOWN); //check pawn threat
	int protectedRow = (color == WHITE)?(FIRST_ROW):(LAST_ROW); //pawn cant get there
	char oppPawn = PAWN(oppColor);
	if(pieceRow == protectedRow){return false;}
	if(src->gameBoard[pieceRow+dir][pieceCol-1]==oppPawn){
		if(pieceCol != LEFT_MOST_COL){return true;}  //check if we are in range
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


	return steps;
}

SP_CHESS_GAME_MESSAGE spChessPrintMoves(SPArrayList* list){
	int elemNum = spArrayListSize(list);
	int step, destRow, destCol;
	for(int i = 0 ; i<elemNum; i++){
		step = spArrayListGetAt(list, i);
		destRow = spChessGameGetRowFromStep(step);
		destCol = spChessGameGetColFromStep(step);
		printf("<%d,%c>",8 - destRow,COL_NUM_TO_LETTER(destCol));

		if(spChessGameWillStepThreaten(step)){
			printf("*");
		}
		if(spChessGameWillStepCapture(step)){
			printf("^");
		}
		printf("\n");
	}
	return SP_CHESS_GAME_SUCCESS;

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
	char destinationPosition = spChessGameGetDestinationPositionFromMove(move); // Location of captured piece
	int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
	int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
	char currentPosition = spChessGameGetCurrentPositionFromMove(move); // Location of moved piece
	int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
	int currentRow = spChessGameGetRowFromPosition(currentPosition);

	src->gameBoard[currentRow][currentColumn] = src->gameBoard[destinationRow][destinationColumn]; // Set board
	src->gameBoard[destinationRow][destinationColumn] = captured;
	int currentPlayer = src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE; // And current player

    for (int i = 0; i < N_COLUMNS * 4; i++) { // Update location for moved piece
        if (src->locations[i] == destinationPosition) {
            src->locations[i] = currentPosition;
        }
    }

	if (captured == PAWN(currentPlayer)) { // Update location if pawn was captured
        int startIndex = currentPlayer ? 2 * N_COLUMNS : N_COLUMNS; // Pawn locations by color
		for (int i = 0; i < N_COLUMNS; i++) {
			if (!src->locations[i + startIndex]) {
				src->locations[i + startIndex] = destinationPosition;
			}
		}
	}

	int index = -1;
	if (captured == KNIGHT(currentPlayer)) { // Update location if knight was captured
		if (!src->locations[RIGHT_KNIGHT_LOC(currentPlayer)]) {
			index = RIGHT_KNIGHT_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_KNIGHT_LOC(currentPlayer)]) {
			index = LEFT_KNIGHT_LOC(currentPlayer);
		}
	}

	if (captured == ROOK(currentPlayer)) { // Rook taken
		if (!src->locations[RIGHT_ROOK_LOC(currentPlayer)]) {
			index = RIGHT_ROOK_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_ROOK_LOC(currentPlayer)]) {
			index = LEFT_ROOK_LOC(currentPlayer);
		}
	}

	if (captured == BISHOP(currentPlayer)) { // Bishop
		if (!src->locations[RIGHT_BISHOP_LOC(currentPlayer)]) {
			index = RIGHT_BISHOP_LOC(currentPlayer);
		}
		else if (!src->locations[LEFT_BISHOP_LOC(currentPlayer)]) {
			index = LEFT_BISHOP_LOC(currentPlayer);
		}
	}

	if (captured == QUEEN(currentPlayer)) { // QUEEN???
		index = QUEEN_LOC(currentPlayer);
	}
	if (index != -1) {
		src->locations[index] = destinationPosition;
	}

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
				gameEnded = 0; // Game isn't over
				spArrayListDestroy(possibleMoves);
				break;
			}

			spArrayListDestroy(possibleMoves);
		}
	}

	return gameEnded;
}
