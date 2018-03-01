//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"

#define STARTING_ROW "RNBQKBNR"
#define CAPITAL_TO_LOW(c) c+'a'-'A'
#define BLANK '_'
#define BOTTOM_PRINT_FORMAT_SEPERATOR "  -----------------"
#define BOTTOM_PRINT_FORMAT_LINE "   A B C D E F G H"

SPChessGame* spChessGameCreate(int historySize){
	if (historySize <= 0) { // historySize is illegal
		return 0;
	}
	SPChessGame* game = malloc(sizeof(SPChessGame));
	if (!game) { // malloc failed
		return 0;
	}
	game->currentPlayer = WHITE; // set first player
	game->user_color = WHITE;
	spChessGameResetBoard(game);

	game->history = spArrayListCreate(historySize);
	if (!game->history) { // failed creating spArrayList for history
		free(game);
		return 0;
	}
	return game;
}

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move){

}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* game){
	if (!game) { // source is NULL
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
	char curChar;
	int i,j;
	for(i=0 ; i<N_COLUMNS ; i++){
		printf("%d|",i);
		for(j=0 ; j<N_COLUMNS ; j++){
			curChar = game->gameBoard[i][j];
			if(!curChar){
				printf(" %c",BLANK);
			}
			else
				printf(" %c",game->gameBoard[i][j]);
		}
		printf(" |\n",i);
	}
	printf("s\n",BOTTOM_PRINT_FORMAT_SEPERATOR);
	printf("s\n",BOTTOM_PRINT_FORMAT_LINE);

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src){
	if (!src) { // src is NULL
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}

	int i,j;
	char* starting_row = STARTING_ROW;

	for(i=0 ; i<N_COLUMNS ; i++){
		src->gameBoard[1][i] = 'M';
	}
	for(i=0 ; i<N_COLUMNS ; i++){
		src->gameBoard[6][i] = 'm';
	}

	for(i=0 ; i<N_COLUMNS ; i++){
		src->gameBoard[0][i] = starting_row[i];
		src->gameBoard[7][i] = LOW_TO_CAPITAL(starting_row[i]);
	}

	for(i=2 ; i<N_COLUMNS-2 ; i++){
		for(j=0 ; j<N_COLUMNS ; j++){
			src->gameBoard[i][j] = 0;
		}
	}


	return SP_CHESS_GAME_SUCCESS;
}
