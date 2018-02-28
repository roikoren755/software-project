//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"

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

SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src){
	if (!src) { // src is NULL
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}

	int i;
	for(i=0;i<N_COLUMNS;i++){
		src->gameBoard[1][i] = 'M';
	}
	for(i=0;i<N_COLUMNS;i++){
		src->gameBoard[6][i] = 'm';
	}
	src->gameBoard[0][0] = src->gameBoard[0][7] = 'R';
	src->gameBoard[0][1] = src->gameBoard[0][6] = 'N';
	src->gameBoard[0][2] = src->gameBoard[0][5] = 'B';
	src->gameBoard[0][3] = 'Q';
	src->gameBoard[0][4] = 'K';

	src->gameBoard[7][0] = src->gameBoard[7][7] = 'r';
	src->gameBoard[7][1] = src->gameBoard[7][6] = 'n';
	src->gameBoard[7][2] = src->gameBoard[7][5] = 'b';
	src->gameBoard[7][3] = 'q';
	src->gameBoard[7][4] = 'k';

	return SP_CHESS_GAME_SUCCESS;
}
