/*
 * SPChessGameSdl.c
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */

#include "SPChessGameSdl.h"
#include <string.h>
#define CANCEL 2
#define YES 1
#define NO 0

int var=0;


Screen* SPGameCreateGameScreen(){
	//Allocate screen struct
	Screen* gameScreen = createScreen(900,660,
			"Chess Game",
			N_MAX_WIDGETS,
			HIDE,
			NONE,
			NONE,
			SPDrawScreen);
		if (gameScreen == NULL){
			return NULL;
		}
		SDL_Renderer* rend = gameScreen->renderer;


		//Allocate screen's widgets, starting with the board's pieces in their starting location
	int color;
	char tempString[30], colors[2][30] = {"black","white"};
	for( color = 0; color<2; color++){

		sprintf(tempString,"pics/%s_rook.bmp",colors[color]);
		gameScreen->widgets[LEFT_ROOK_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+0*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
		gameScreen->widgets[RIGHT_ROOK_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+7*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);

		sprintf(tempString,"pics/%s_knight.bmp",colors[color]);
		gameScreen->widgets[LEFT_KNIGHT_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+1*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
		gameScreen->widgets[RIGHT_KNIGHT_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+6*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);


		sprintf(tempString,"pics/%s_bishop.bmp",colors[color]);
		gameScreen->widgets[LEFT_BISHOP_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+2*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
		gameScreen->widgets[RIGHT_BISHOP_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+5*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);

		sprintf(tempString,"pics/%s_queen.bmp",colors[color]);
		gameScreen->widgets[QUEEN_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+3*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);

		sprintf(tempString,"pics/%s_king.bmp",colors[color]);
		gameScreen->widgets[KING_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,
				10+4*SQUARE_WIDTH,10+color*SQUARE_HEIGHT*7,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
	}

	int i;
	for( i = 8; i<16; i++){
		gameScreen->widgets[i] = createSticker(rend,
				"pics/black_pawn.bmp",SPMovePiece,SPHighlightAllMoves,
				10+(i-8)*SQUARE_WIDTH,10+1*SQUARE_HEIGHT,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
		gameScreen->widgets[i+N_COLUMNS] = createSticker(rend,
				"pics/white_pawn.bmp",SPMovePiece,SPHighlightAllMoves,
				10+(i-8)*SQUARE_WIDTH,10+6*SQUARE_HEIGHT,SQUARE_WIDTH,SQUARE_HEIGHT,SHOWN);
	}

	gameScreen->widgets[GS_RESTART_GAME] = createButton(rend,
								"pics/restart_game.bmp",SPRestartGame,672,25,210,50,SHOWN);
	gameScreen->widgets[GS_SAVE_GAME] = createButton(rend,
									"pics/save_game.bmp",SPOpenNextWindow,672,100,210,50,SHOWN);
	gameScreen->widgets[GS_SAVED_GAME_INDICATOR] = createLable(rend,
									"pics/saved_game_indicator.bmp",675,83,180,20,HIDE);
	gameScreen->widgets[GS_LOAD_GAME] = createButton(rend,
								"pics/load_game_gs.bmp",SPOpenNextWindow,672,175,210,50,SHOWN);
	gameScreen->widgets[GS_MAIN_MENU] = createButton(rend,
								"pics/main_menu_gs.bmp",SPOpenMainMenuWindow,672,250,210,50,SHOWN);
	gameScreen->widgets[GS_QUIT] = createButton(rend,
								"pics/quit_gs.bmp",SPQuit,672,325,210,50,SHOWN);

	gameScreen->widgets[GS_UNDO] = createButton(rend,
								"pics/gs_Undo.bmp",SPUndoMove,672,425,210,50,HIDE);
	gameScreen->widgets[GS_GET_MOVES_LEGEND] = createLable(rend,
								"pics/gs_GetMoves_legend.bmp",665,525,227,125,HIDE);
	gameScreen->widgets[GS_CHECK] = createLable(rend,
									"pics/gs_Check.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_CHECK_MATE] = createLable(rend,
									"pics/gs_Check-mate.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_DRAW] = createLable(rend,
									"pics/gs_Draw.bmp",672,540,210,50,HIDE);

	//check if widgets were created successfully
	int success = SPCheckWidgetsInit(gameScreen);
	if(!success){
		return NULL;
	}

	return gameScreen;
}


/**
*  Draws a 8x8 board to the renderer.
*  @param rend - pointer to an SDL_Renderer.
*  @return CONTINUE signal on success.
*  		   -1 if an error accrued
*/
int SPDrawBoard(SDL_Renderer* rend){
	int success;

	//draw black frame
	SDL_Rect frameRect = { .x = 5, .y = 5, .w = 650, .h = 650 };
	success = SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	if(success == -1){
		return -1;
	}

	success = SDL_RenderFillRect (rend, & frameRect);
	if(success == -1){
		return -1;
	}

	//draw yellow within the frame
	frameRect.x+=5;
	frameRect.y+=5;
	frameRect.h-=(5*2);
	frameRect.w-=(5*2);
	success = SDL_SetRenderDrawColor (rend, CHESS_YELLOW_COLOR);
	if(success == -1){
		return -1;
	}
	success = SDL_RenderFillRect (rend, & frameRect);
	if(success == -1){
		return -1;
	}

	int i,j;
	for( i = 0; i<8; i++){ //Draw 32 brown squares inside the board, in the appropriate location
			for( j = 10+(1-i%2)*SQUARE_WIDTH ; j<BOARD_WIDTH; j+=(SQUARE_WIDTH*2)){
				SDL_Rect rect = { .x = j, .y = 10+i*SQUARE_HEIGHT, .w = SQUARE_WIDTH, .h = SQUARE_HEIGHT };
				success = SDL_SetRenderDrawColor(rend, CHESS_BROWN_COLOR);
				if(success == -1){
					return -1;
				}
				success = SDL_RenderFillRect(rend, &rect);
				if(success == -1){
					return -1;
				}
			}
		}
	return CONTINUE;
}

/**
*  Shows the lable indicatong the current game's state.
*
*  @param screens - pointer to the game's screen array.
*  @param state - int representing the game state's widget index.
*  				  if the index does not hold for a corresponding state, none of the 3 lable are shown
*/
void SPShowGameState(Screen** screens, int state){
	int i;
	for(i=GS_CHECK; i<GS_CHECK+3; i++){
		screens[GAME_SCREEN]->widgets[i]->shown = (i==state)?SHOWN:HIDE;
	}
}

/**
*  Updates the game state indicating label according to the current state.
*
*  @param screens - pointer to the game's screen array.
*  @param game - pointer to the game's struct.
*/
void SPUpdateGameState(Screen** screens, SPChessGame* game){
	switch(game->gameState){
		case NORMAL:
			SPShowGameState(screens,NONE);
			break;
		case CHECK:
			SPShowGameState(screens,GS_CHECK);
			break;
		case CHECKMATE:
			SPShowGameState(screens,GS_CHECK_MATE);
			break;
		case DRAW:
			SPShowGameState(screens,GS_DRAW);
			break;
	}
}


int SPUpdateBoard(Screen** screens, SPChessGame* game){
	SDL_Renderer* rend = screens[GAME_SCREEN]->renderer;
	SDL_RenderClear(rend);

	int success = SPDrawBoard(rend);
	if(!success){
		SPShowDrawError();
		return QUIT;
	}

	int raw,col;

	if(game->gameMode==1 && (game->currentPlayer != game->userColor)){ //when game starts, its time to perform computer move
		int move = spMinimaxSuggestMove(game);
		if (move == -1) {
			printf("ERROR: Couldn't figure out computer's first move. Returning to main menu...\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR: Couldn't figure out computer's first move. Returning to main menu..", NULL);
			return SPOpenWindow(screens,MAIN_MENU_WINDOW); //this is the first step, we must quit the current game
		}
		else {
			spChessGameSetMove(game, move);
			//update undo availability
			screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;
		}
	}

	//update location of pieces on the board
	int i;
	for( i = 0; i<(N_COLUMNS * 4); i++){
		if(game->locations[i]){
			raw = spChessGameGetRowFromPosition(game->locations[i]);
			col = spChessGameGetColumnFromPosition(game->locations[i]);
			Sticker * sticker = (Sticker *)screens[0]->widgets[i]->data;
			sticker->location.x = 10+col*80;
			sticker->location.y = 10+raw*80;

			screens[0]->widgets[i]->shown = SHOWN; //ensure widget is shown
		}
		else{
			screens[0]->widgets[i]->shown = HIDE; //ensure widget is hidden
		}

	}

	SPUpdateGameState(screens,game);

	SDL_RenderPresent(rend);

	return PRESSED;
}


int SPRestartGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if(screenIndex!=GAME_SCREEN || widgetIndex!=GS_RESTART_GAME ) {
		return NONE;
		}    
    
    spChessGameResetBoard(game);

    //clear undo history
    spArrayListClear(game->history);

	//update saved game indicator
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE;

	return SPUpdateBoard(screens,game);
;
}

int SPShowSaveBeforeQuitMassage(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
    const SDL_MessageBoxButtonData buttons[] = {
        { /* .flags, .buttonid, .text */        0, 0, "no" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   0, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            {   0,   0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "quit", /* .title */
        "This game is not saved, would you like to save before leaving?", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
		printf("ERROR: Something went wrong! try again...\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									"ERROR: Something went wrong! try again", NULL);
        return PRESSED;
    }
    else if(buttonid == NO){ //user pressed no
    	if(widgetIndex == GS_QUIT){ //user requested quit
    		return QUIT;
    	}
    	spChessGameResetGame(game); //user requested main menu
    	SPUpdateBoard(screens,game);
    	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
    }
    else if(buttonid == YES){
    	return SPOpenLoadSaveGameWindow(screens,SAVE_GAME_WINDOW,screenIndex);
    }
    else if(buttonid == CANCEL){
    	return PRESSED;
    }

    return 0;
}



int SPUndoMove(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	int move = spArrayListGetFirst(game->history);
	int userMove = 0;

	spChessGameUndoMove(game);

	if(game->gameMode == 1 && !spArrayListIsEmpty(game->history)){ //undo another move
		userMove = spArrayListGetFirst(game->history);
		spChessGameUndoMove(game);
	}

	//update undo button availability
	screens[screenIndex]->widgets[widgetIndex]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	//check and update game's state
	if (spChessCheckGameState(game,game->currentPlayer) == SP_CHESS_GAME_SUCCESS){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									"ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again", NULL);
		if(userMove){spChessGameSetMove(game,userMove);}
		spChessGameSetMove(game,move); //TODO
	}


	return 	SPUpdateBoard(screens , game);;
}

/**
*  Handles a move the user has performed, setting it or showing a massage
*  explaining why it is not valid.
*  @param game - pointer the game's struct.
*  @param step - the move.
*  @return SP_CHESS_GAME_INVALID_ARGUMENT if an error occurred.
*  		   SP_CHESS_GAME_NO_PIECE_IN_POSITION if the user's piece was not located in the piece's
*  		   		current location (meaning the user moved the opponent piece).
*  		   SP_CHESS_GAME_ILLEGAL_MOVE if the move is illegal.
*  		   SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED if the move is illegal because the king will
*  		   		remain threatened.
*  		   SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED if the move is illegal because the king will
*  		   		will become threatened.
*  		   SP_CHESS_GAME_SUCCESS if the move is legal and no error occurred
*/
SP_CHESS_GAME_MESSAGE spChessGameHandleMove(SPChessGame* game, int move) {
	int moveToRestore = 0,secondMoveToRestore = 0; //for later use

	SP_CHESS_GAME_MESSAGE message = spChessGameIsValidMove(game, move);
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("ERROR: Something went wrong! try again...\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: Something went wrong! try again", NULL);
		return message;
	}

	else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"That's not your piece!", NULL);
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"Illegal move!", NULL);

	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"Illegal move: king is still threatened!", NULL);

	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"Illegal move: king will be threatened!", NULL);
	}
	else if(message == SP_CHESS_GAME_SUCCESS){
		if (spArrayListIsFull(game->history)) { // in error case, we might want to undo the user move.
			moveToRestore = spArrayListGetLast(game->history); // A move will be lost if we add another to the game's history
			                                             // So save it for later
		}
		message = spChessGameSetMove(game, move);
		if (message != SP_CHESS_GAME_SUCCESS) {
				printf("ERROR: Something went wrong while setting up a move! try again...\n");
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
											"ERROR: Something went wrong while setting up a move! try again", NULL);
		}
		else if(game->gameMode == 1) {
				if (spArrayListIsFull(game->history)) {
					secondMoveToRestore = spArrayListGetLast(game->history); //
				                                             // if needed, save for later use
				}
				move = spMinimaxSuggestMove(game);
				if (move == -1) {
					printf("ERROR: Couldn't figure out computer move. try again...\n");
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR:  Couldn't figure out computer move. try again", NULL);
				    spUndoAndRestoreHistory(game, moveToRestore); //undo the user's move
				 	return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
				else if(move != 0) { //move == 0 indicates computer can't perform any move
					spChessGameSetMove(game, move);
				}
		}


		// check and update gamestate
		if (spChessCheckGameState(game,game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT){
			printf("ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again", NULL);
			spUndoAndRestoreHistory(game, secondMoveToRestore); //if an error occurred ,
			spUndoAndRestoreHistory(game, moveToRestore);       //undo the computer and the user's move
		}
	}


	return message;
}

int SPMovePiece(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game){
	if(game->gameState == CHECKMATE || game->gameState == DRAW){
		return 0;  //gameOver, no moving possible
	}

	Sticker * piece = (Sticker *)src->data;

	//get the piece current location
	int curRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int curCol = SCREEN_TO_BOARD_LOCATION(piece->location.x);

	int done = 0,success;
	int movement = 0;
	int xDif, yDif;
	SDL_Event e = *event;

	// event handling loop
	while (!done) {

		switch (e.type) {
			case SDL_MOUSEBUTTONDOWN:
				if ((e.button.x > piece->location.x) && (e.button.y > piece->location.y) &&
					(e.button.x < piece->location.x+piece->location.w) && (e.button.y < piece->location.y+piece->location.h)) {
					xDif = e.button.x - piece->location.x;
					yDif = e.button.y - piece->location.y;
					movement = 1;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				movement = 0;
				done = 1;
				break;
			case SDL_MOUSEMOTION:
				if (movement) { 			//move's the piece
					piece->location.x = e.motion.x - xDif;
					piece->location.y = e.motion.y - yDif;
				}
				break;
		}
		//draw the game screen
		success = screens[GAME_SCREEN]->draw(screens[GAME_SCREEN],GAME_SCREEN);
		if(!success){
			SPShowDrawError();
			return QUIT;
		}

		SDL_WaitEvent(&e);

	}

	//loop ended, get the new location
	piece->location.x = ROUNDED_POSITION(piece->location.x);
	piece->location.y = ROUNDED_POSITION(piece->location.y);
	int destRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int destCol = SCREEN_TO_BOARD_LOCATION(piece->location.x);
	int move = setMoveCoordinatesToInt(curRow, curCol,  destRow,  destCol);

	//handle the move
	SP_CHESS_GAME_MESSAGE massage = spChessGameHandleMove(game,move);

	if(massage!=SP_CHESS_GAME_SUCCESS){ //no move occurred, set the piece to it's old location
		piece->location.x = BOARD_TO_SCREEN_LOCATION(curCol);
		piece->location.y = BOARD_TO_SCREEN_LOCATION(curRow);
		return PRESSED;
	}

	// check if enabling undo is required
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	//game is not saved anymore
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE;

			//update the board
	return SPUpdateBoard(screens,game);;
}

/**
*  Highlights a specific square on the renderer according a given step (containing the
*  information of where to highlight and in what color).
*  @param rend - pointer the renderer.
*  @param step - the step.
*  @return 0 on success.
*  		   -1 otherwise.
*/
int SPHighlightMove(SDL_Renderer* rend , int step){
	//get the information from the possible step
	   char location = spChessGameGetDestinationPositionFromMove(step);
	   unsigned int row = spChessGameGetRowFromPosition(location);
	   unsigned int column = spChessGameGetColumnFromPosition(location);
	   int threat = spChessGameStepWillThreaten(step);
	   int capture = spChessGameStepWillCapture(step);
	   int x = BOARD_TO_SCREEN_LOCATION(column);
	   int y = BOARD_TO_SCREEN_LOCATION(row);
	   char image[20];

	   //set the right color to load
	   if(threat&capture){
		   strcpy(image ,"pics/gs_purple.bmp");
	   }
	   else if(threat){
		   strcpy(image ,"pics/gs_red.bmp");
	   }
	   else if(capture){
		   strcpy(image ,"pics/gs_blue.bmp");
	   }
	   else{
		   strcpy(image ,"pics/gs_green.bmp");
	   }

	   //set rect in the right location
	   SDL_Rect rect = { .x = x, .y = y, .w = SQUARE_WIDTH, .h = SQUARE_HEIGHT };

	   SDL_Surface* surf = SDL_LoadBMP(image);
	   	if (surf == NULL) {
	   		printf("ERROR: unable to load image: %s\n", SDL_GetError());
	   		return -1;
	   	}
	   	// set a specific color (white: 255,255,255) to transparent
	   	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));

	   	// create a texture from the surface image
	   	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
	   	if (tex == NULL) {
	   		printf("ERROR: unable to create texture: %s\n", SDL_GetError());
	   		return -1;
	   	}
	   	// surface not needed anymore
	   	SDL_FreeSurface(surf);

	   	// draw the texture to the rect
	   	SDL_RenderCopy(rend, tex, NULL, &rect);

	   	//tex done his job
	   	SDL_DestroyTexture(tex);

	   	return 0;

}


int SPHighlightAllMoves(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game){
	Sticker * piece = (Sticker *)src->data;
	SDL_Renderer* rend = screens[GAME_SCREEN]->renderer;
	int i,done = 0;

	//present the legend
	screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND]->draw(screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND],
																rend);

	//get the location of the piece
	unsigned int raw =  SCREEN_TO_BOARD_LOCATION(piece->location.y);
	unsigned int col =  SCREEN_TO_BOARD_LOCATION(piece->location.x);
	unsigned char position  = spChessGameSetLocation(raw,col);

	//get the list of possible moves
	SPArrayList * list = spChessGameGetMoves(game,position);
	if(list == NULL){
		printf("ERORR: couldn't highlight moves due an allocation error\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
								"ERROR: could not highlight moves", NULL);
		return PRESSED;
	}

    int numOfMoves = spArrayListSize(list);
	SDL_Event e = *event;
	int success;

	//event loop, done when mouse button is up
	while (!done) {
			success = 0;
			switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
						for(i=0; i<numOfMoves; i++){
							success += SPHighlightMove(rend,spArrayListGetAt(list, i));
						}
						if(success<0){ //if some highlighting has failed
							SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
													"ERROR: could not highlight some or all moves", NULL);
						}
						break;
				case SDL_MOUSEBUTTONUP:
						done = 1;
						break;
			}

		SDL_RenderPresent(rend);

		SDL_WaitEvent(&e);
	}

	free(list);

	return PRESSED;

}


