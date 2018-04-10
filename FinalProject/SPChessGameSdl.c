/*
 * SPChessGameSdl.c
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */

#include "SPChessGameSdl.h"
#include <string.h>
#define CANCEL 0
#define YES 1
#define NO 2

int var=0;


Screen* SPGameCreateGameScreen(){
	Screen* gameScreen = createScreen(900,660,
			"Chess Game",
			N_MAX_WIDGETS,
			HIDE,
			NONE,
			NONE,
			SPDrawGameScreen);
		if (gameScreen == NULL){
			return NULL;
		}
		SDL_Renderer* rend = gameScreen->renderer;


//	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

//	SPDrawBoard(rend);

	int color;
	char tempString[30], colors[2][30] = {"black","white"};
	for( color = 0; color<2; color++){

		sprintf(tempString,"pics/%s_rook.bmp",colors[color]);
		gameScreen->widgets[LEFT_ROOK_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+0*80,10+color*80*7,80,80,SHOWN);
		gameScreen->widgets[RIGHT_ROOK_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+7*80,10+color*80*7,80,80,SHOWN);

		sprintf(tempString,"pics/%s_knight.bmp",colors[color]);
		gameScreen->widgets[LEFT_KNIGHT_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+1*80,10+color*80*7,80,80,SHOWN);
		gameScreen->widgets[RIGHT_KNIGHT_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+6*80,10+color*80*7,80,80,SHOWN);


		sprintf(tempString,"pics/%s_bishop.bmp",colors[color]);
		gameScreen->widgets[LEFT_BISHOP_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+2*80,10+color*80*7,80,80,SHOWN);
		gameScreen->widgets[RIGHT_BISHOP_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+5*80,10+color*80*7,80,80,SHOWN);

		sprintf(tempString,"pics/%s_queen.bmp",colors[color]);
		gameScreen->widgets[QUEEN_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+3*80,10+color*80*7,80,80,SHOWN);

		sprintf(tempString,"pics/%s_king.bmp",colors[color]);
		gameScreen->widgets[KING_LOC(color)] = createSticker(rend,
				tempString,SPMovePiece,SPHighlightAllMoves,10+4*80,10+color*80*7,80,80,SHOWN);
	}
	int i;
	for( i = 8; i<16; i++){
		gameScreen->widgets[i] = createSticker(rend,
				"pics/black_pawn.bmp",SPMovePiece,SPHighlightAllMoves,10+(i-8)*80,10+1*80,80,80,SHOWN);
		gameScreen->widgets[i+N_COLUMNS] = createSticker(rend,
				"pics/white_pawn.bmp",SPMovePiece,SPHighlightAllMoves,10+(i-8)*80,10+6*80,80,80,SHOWN);
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
								"pics/gs_Undo.bmp",SPUndoMove,672,475,210,50,HIDE);
	gameScreen->widgets[GS_GET_MOVES_LEGEND] = createLable(rend,
								"pics/gs_GetMoves_legend.bmp",672,600,210,50,HIDE);
	gameScreen->widgets[GS_CHECK] = createLable(rend,
									"pics/gs_Check.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_CHECK_MATE] = createLable(rend,
									"pics/gs_Check-mate.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_DRAW] = createLable(rend,
									"pics/gs_Draw.bmp",672,540,210,50,HIDE);

	return gameScreen;
}


int SPDrawBoard(SDL_Renderer* rend){
	SDL_Rect frameRect = { .x = 5, .y = 5, .w = 650, .h = 650 };
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);  //draw black frame
	SDL_RenderFillRect (rend, & frameRect);

	frameRect.x+=5;
	frameRect.y+=5;
	frameRect.h-=(5*2);
	frameRect.w-=(5*2);
	    // Color from within the rectangle
	SDL_SetRenderDrawColor (rend, CHESS_YELLOW_COLOR);
	// Fill in the rectangle
	SDL_RenderFillRect (rend, & frameRect);
	int i,j;
	for( i = 0; i<8; i++){ //Draw board
			for( j = 10+(1-i%2)*80 ; j<640; j+=160){
				SDL_Rect rect = { .x = j, .y = 10+i*80, .w = 80, .h = 80 };
				SDL_SetRenderDrawColor(rend, CHESS_BROWN_COLOR);
				SDL_RenderFillRect(rend, &rect);
			}
		}
	return PRESSED;
}

int SPUpdateBoard(Screen** screens, SPChessGame* game){
	SDL_Renderer* rend = screens[0]->renderer;
	SDL_RenderClear(rend);
	SPDrawBoard(rend);
	int raw,col;

	if(game->gameMode==1 && game->currentPlayer != game->userColor){
		int move = spMinimaxSuggestMove(game);
		if (move == -1) {
			printf("ERROR: Couldn't figure out computer move. Quitting...\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR: Couldn't figure out computer move. Quitting..", NULL);
			SPOpenMainMenuWindow(screens,game,GAME_SCREEN ,GS_MAIN_MENU);
		}
		else {
			spChessGameSetMove(game, move);
			screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;
		}
	}

	int i;
	for( i = 0; i<32; i++){
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

void SPShowGameState(Screen** screens, int state){
	int i;
	for(i=GS_CHECK; i<GS_CHECK+3; i++){
		screens[GAME_SCREEN]->widgets[i]->shown = (i==state)?SHOWN:HIDE;
	}
}

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

void SPDrawGameScreen(Screen* screen){
	if(screen==NULL){
		return;
	}
	SDL_Renderer* rend = screen->renderer;
	SDL_SetRenderDrawColor(rend, BACKGROUND_COLOR);
	SDL_RenderClear(rend);

	SPDrawBoard(rend);
	SPDrawWidgets(screen);

	SDL_Delay(10);
	SDL_RenderPresent(rend);

}

int SPRestartGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
    spChessGameResetBoard(game);

    //clear undo history
    spArrayListClear(game->history);
    game->currentPlayer = WHITE;
	//update saved game indicator
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE;

    SPUpdateBoard(screens,game);
	return PRESSED;
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
        return QUIT; //TODO
    }
    else if(buttonid == CANCEL){ //user pressed cancel
    	if(widgetIndex == GS_QUIT){
    		return QUIT;
    	}
    	spChessGameResetGame(game);
    	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
    }
    else if(buttonid == YES){
    	return SPOpenNextWindow(screens , game, GAME_SCREEN , GS_SAVE_GAME);
    }
    else if(buttonid == NO){
    	return PRESSED;
    }

    return 0;
}



int SPUndoMove(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	spChessGameUndoMove(game);

	if(game->gameMode == 1){
		spChessGameUndoMove(game);
	}

	//update undo button availability
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	if (spChessCheckGameState(game,game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									"ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again", NULL);
	//TODO
	}


	SPUpdateBoard(screens , game);
	return PRESSED;
}


SP_CHESS_GAME_MESSAGE spChessGameHandleMove(SPChessGame* game, int move) {
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
		int lastMove = 0;
		if (spArrayListIsFull(game->history)) { // in error case, we might want to undo the user move.
			lastMove = spArrayListGetLast(game->history); // A move will be lost if we add another to the game's history
			                                             // So save it for later
		}
		message = spChessGameSetMove(game, move);
		if (message != SP_CHESS_GAME_SUCCESS) {
				printf("ERROR: Something went wrong while setting up a move! try again...\n");
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
											"ERROR: Something went wrong while setting up a move! try again", NULL);
		}
		else if(game->gameMode == 1) {
				// check and update gamestate
				if (spChessCheckGameState(game,game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT){
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
											"ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again", NULL);
					int success = spUndoAndRestoreHistory(game, lastMove);
				    if(!success){
				    	//TODO
				    }
				}
				if(game->gameState==CHECKMATE||game->gameState==DRAW){
					return message; //we are done here
				}
				move = spMinimaxSuggestMove(game);
				printf("%d__",move);
				if (move == -1) {
					printf("ERROR: Couldn't figure out computer move. try again...\n");
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR:  Couldn't figure out computer move. try again", NULL);
				    int success = spUndoAndRestoreHistory(game, lastMove);
				    if(!success){//TODO
				    }
				 	return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
				else {
					spChessGameSetMove(game, move);
				}
		}

		// check and update gamestate
		if (spChessCheckGameState(game,game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT){
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
										"ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again", NULL);
			int success = spUndoAndRestoreHistory(game, lastMove);
			    if(!success){
			    	//TODO
			    }
		}
	}


	return message;
}

int SPMovePiece(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game){
	if(game->gameState == CHECKMATE || game->gameState == DRAW){
		return 0;  //gameOver
	}

	Sticker * piece = (Sticker *)src->data;

	int curRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int curCol = SCREEN_TO_BOARD_LOCATION(piece->location.x);

	int done = 0;
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
				if (movement) {
					piece->location.x = e.motion.x - xDif;
					piece->location.y = e.motion.y - yDif;
				}
				break;
		}

		printf("%d.in the movement, done-%d,move-%d\n",var++,done,movement);
		screens[GAME_SCREEN]->draw(screens[GAME_SCREEN]);

		//src->draw(src,screens[GAME_SCREEN]->renderer);
		SDL_WaitEvent(&e);

	}

	piece->location.x = ROUNDED_POSITION(piece->location.x);
	piece->location.y = ROUNDED_POSITION(piece->location.y);
	int destRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int destCol = SCREEN_TO_BOARD_LOCATION(piece->location.x);
	int move = setMoveCoordinatesToInt(curRow, curCol,  destRow,  destCol);

	printf("%d.<%d,%d> to <%d,%d>,\n",var++,curRow,curCol,destRow,destCol);

	SP_CHESS_GAME_MESSAGE massage = spChessGameHandleMove(game,move);
	printf("%d.massage-%d\n",var++,massage);

	if(massage!=SP_CHESS_GAME_SUCCESS){ //no move occurred
		printf("%d.no move, raw-%d,col-%d,\n",var++,curRow,curCol);

		piece->location.x = BOARD_TO_SCREEN_LOCATION(curCol);
		piece->location.y = BOARD_TO_SCREEN_LOCATION(curRow);
		return PRESSED;
	}



	// check if enabling undo is required
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;
	//game is not saved anymore
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE;


	SPUpdateBoard(screens,game);
	printLocations(game);
	return PRESSED;
}


int SPHighlightMove(SDL_Renderer* rend , int step){
	   char location = spChessGameGetDestinationPositionFromMove(step);
	   unsigned int row = spChessGameGetRowFromPosition(location);
	   unsigned int column = spChessGameGetColumnFromPosition(location);
	   int threat = spChessGameStepWillThreaten(step);
	   int capture = spChessGameStepWillCapture(step);
	   int x = BOARD_TO_SCREEN_LOCATION(column);
	   int y = BOARD_TO_SCREEN_LOCATION(row);
	   char image[20];

		printf("%d.highlighting:<%d,%d>\n",var++,x,y);

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

	   SDL_Rect rect = { .x = x, .y = y, .w = 80, .h = 80 };

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
	   		return 0;
	   	}
	   	// surface not needed anymore - free it
	   	SDL_FreeSurface(surf);

	   	// draw entire texture (NULL) to part of window (rect)
	   	SDL_RenderCopy(rend, tex, NULL, &rect);

	   	return PRESSED;

}


int SPHighlightAllMoves(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game){
	Sticker * piece = (Sticker *)src->data;
	SDL_Renderer* rend = screens[GAME_SCREEN]->renderer;
	int i,done = 0;

	screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND]->draw(screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND],
																rend);

	unsigned int raw =  SCREEN_TO_BOARD_LOCATION(piece->location.y);
	unsigned int col =  SCREEN_TO_BOARD_LOCATION(piece->location.x);
	unsigned char position  = spChessGameSetLocation(raw,col);
	SPArrayList * list = spChessGameGetMoves(game,position);
	if(list == NULL){
		printf("%d.NULL.., raw-%d,col-%d,done-%d\n",var++,raw,col,done);
		return -1;
	}

    int numOfMoves = spArrayListSize(list);
	SDL_Event e = *event;

	while (!done) {

			switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
					printf("%d.highlighting, numOfMoves-%d done-%d\n",var++,numOfMoves,done);
						for(i=0; i<numOfMoves; i++){
							printf("%d.highlighting, i-%d done-%d\n",var++,i,done);
							SPHighlightMove(rend,spArrayListGetAt(list, i));
						}
					break;
				case SDL_MOUSEBUTTONUP:
					done = 1;
					break;
			}

		printf("%d.highlighting, done-%d\n",var++,done);
		//SPDrawScreen(screens,GAME_SCREEN);
		SDL_RenderPresent(rend);

		//src->draw(src,screens[GAME_SCREEN]->renderer);
		SDL_WaitEvent(&e);
	}

	free(list);

	return PRESSED;

}


void printLocations(SPChessGame* game){
	printf("%d.",var++);
	for(int i=0;i<32;i++){
		printf("<%d,%d>\n",
					spChessGameGetRowFromPosition(game->locations[i]),
					spChessGameGetColumnFromPosition(game->locations[i]));

	}
}
