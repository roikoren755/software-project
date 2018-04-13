/*
 * SPChessGameSdl.c
 *
 *  Created on: 19 March 2018
 *      Author: user
 */

#include "SPChessGameSdl.h"
#include <string.h>
#define CANCEL 2
#define YES 1
#define NO 0

Screen* SPGameCreateGameScreen() {
	//Allocate screen struct
	Screen* gameScreen = createScreen(900, 660, "Chess Game", N_MAX_WIDGETS, HIDE, NONE, NONE, SPDrawScreen);
	if (gameScreen == NULL) {
		return NULL;
	}

	SDL_Renderer* renderer = gameScreen->renderer;

	//Allocate screen's widgets, starting with the board's pieces in their starting location
	int color;
	char tempString[30];
	char* colors[2] = {"black","white"};
	for (color = 0; color < 2; color++) {
		sprintf(tempString, "pics/%s_rook.bmp", colors[color]);
		gameScreen->widgets[LEFT_ROOK_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																  SPHighlightAllMoves, 10 + 0 * SQUARE_WIDTH,
																  10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																  SQUARE_HEIGHT, SHOWN);
		gameScreen->widgets[RIGHT_ROOK_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																   SPHighlightAllMoves, 10 + 7 * SQUARE_WIDTH,
																   10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																   SQUARE_HEIGHT, SHOWN);

		sprintf(tempString,"pics/%s_knight.bmp",colors[color]);
		gameScreen->widgets[LEFT_KNIGHT_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																	SPHighlightAllMoves, 10 + 1 * SQUARE_WIDTH,
																	10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																	SQUARE_HEIGHT, SHOWN);
		gameScreen->widgets[RIGHT_KNIGHT_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																	 SPHighlightAllMoves, 10 + 6 * SQUARE_WIDTH,
																	 10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																	 SQUARE_HEIGHT, SHOWN);

		sprintf(tempString,"pics/%s_bishop.bmp",colors[color]);
		gameScreen->widgets[LEFT_BISHOP_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																	SPHighlightAllMoves, 10 + 2 * SQUARE_WIDTH,
																	10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																	SQUARE_HEIGHT, SHOWN);
		gameScreen->widgets[RIGHT_BISHOP_LOC(color)] = createSticker(renderer, tempString, SPMovePiece,
																	 SPHighlightAllMoves, 10 + 5 * SQUARE_WIDTH,
																	 10 + color * SQUARE_HEIGHT * 7, SQUARE_WIDTH,
																	 SQUARE_HEIGHT, SHOWN);

		sprintf(tempString,"pics/%s_queen.bmp",colors[color]);
		gameScreen->widgets[QUEEN_LOC(color)] = createSticker(renderer, tempString, SPMovePiece, SPHighlightAllMoves,
															  10 + 3 * SQUARE_WIDTH, 10 + color * SQUARE_HEIGHT * 7,
															  SQUARE_WIDTH, SQUARE_HEIGHT, SHOWN);

		sprintf(tempString,"pics/%s_king.bmp",colors[color]);
		gameScreen->widgets[KING_LOC(color)] = createSticker(renderer, tempString, SPMovePiece, SPHighlightAllMoves,
															 10 + 4 * SQUARE_WIDTH, 10 + color * SQUARE_HEIGHT * 7,
															 SQUARE_WIDTH, SQUARE_HEIGHT, SHOWN);
	}

	int i;
	for (i = 8; i < 16; i++) {
		gameScreen->widgets[i] = createSticker(renderer, "pics/black_pawn.bmp", SPMovePiece, SPHighlightAllMoves,
											   10 + (i - 8) * SQUARE_WIDTH, 10 + 1 * SQUARE_HEIGHT, SQUARE_WIDTH,
											   SQUARE_HEIGHT, SHOWN);
		gameScreen->widgets[i+N_COLUMNS] = createSticker(renderer, "pics/white_pawn.bmp", SPMovePiece,
														 SPHighlightAllMoves, 10 + (i - 8) * SQUARE_WIDTH,
														 10 + 6 * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT, SHOWN);
	}

	gameScreen->widgets[GS_RESTART_GAME] = createButton(renderer, "pics/restart_game.bmp", SPRestartGame, 672, 25, 210,
														50, SHOWN);

	gameScreen->widgets[GS_SAVE_GAME] = createButton(renderer, "pics/save_game.bmp", SPOpenNextWindow, 672, 100, 210,
													 50, SHOWN);

	gameScreen->widgets[GS_SAVED_GAME_INDICATOR] = createLabel(renderer, "pics/saved_game_indicator.bmp", 675, 83, 180,
															   20, HIDE);

	gameScreen->widgets[GS_LOAD_GAME] = createButton(renderer, "pics/load_game_gs.bmp", SPOpenNextWindow, 672, 175, 210,
													 50, SHOWN);

	gameScreen->widgets[GS_MAIN_MENU] = createButton(renderer, "pics/main_menu_gs.bmp", SPOpenMainMenuWindow, 672, 250,
													 210, 50, SHOWN);

	gameScreen->widgets[GS_QUIT] = createButton(renderer, "pics/quit_gs.bmp", SPQuit, 672, 325, 210, 50, SHOWN);

	gameScreen->widgets[GS_UNDO] = createButton(renderer, "pics/gs_Undo.bmp", SPUndoMove, 672, 425, 210, 50, HIDE);

	gameScreen->widgets[GS_GET_MOVES_LEGEND] = createLabel(renderer, "pics/gs_GetMoves_legend.bmp", 665, 525, 227, 125,
														   HIDE);

	gameScreen->widgets[GS_CHECK] = createLabel(renderer, "pics/gs_Check.bmp", 672, 540, 210, 50, HIDE);

	gameScreen->widgets[GS_CHECK_MATE] = createLabel(renderer, "pics/gs_Check-mate.bmp", 672, 540, 210, 50, HIDE);

	gameScreen->widgets[GS_DRAW] = createLabel(renderer, "pics/gs_Draw.bmp", 672, 540, 210, 50, HIDE);

	int success = SPCheckWidgetsInit(gameScreen); // check if widgets were created successfully
	if (!success) {
		SPDestroyScreen(gameScreen);
		return NULL;
	}

	return gameScreen;
}



void SPDrawBoard(SDL_Renderer* rend){
	int success;

	//draw black frame
	SDL_Rect frameRect = { .x = 5, .y = 5, .w = 650, .h = 650 };
	success = SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}

	success = SDL_RenderFillRect (rend, & frameRect);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}

	//draw yellow within the frame
	frameRect.x+=5;
	frameRect.y+=5;
	frameRect.h-=(5*2);
	frameRect.w-=(5*2);
	success = SDL_SetRenderDrawColor (rend, CHESS_YELLOW_COLOR);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}
	success = SDL_RenderFillRect (rend, & frameRect);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}

	int i,j;
	for( i = 0; i<8; i++){ //Draw 32 brown squares inside the board, in the appropriate location
			for( j = 10+(1-i%2)*SQUARE_WIDTH ; j<BOARD_WIDTH; j+=(SQUARE_WIDTH*2)){
				SDL_Rect rect = { .x = j, .y = 10+i*SQUARE_HEIGHT, .w = SQUARE_WIDTH, .h = SQUARE_HEIGHT };
				success = SDL_SetRenderDrawColor(rend, CHESS_BROWN_COLOR);
				if(success == -1){
					printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
				}
				success = SDL_RenderFillRect(rend, &rect);
				if(success == -1){
					printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
				}
			}
		}
}

/**
*  Shows the label indicatong the current game's state.
*
*  @param screens - pointer to the game's screen array.
*  @param state - int representing the game state's widget index.
*  				  if the index does not hold for a corresponding state, none of the 3 label are shown
*/
void SPShowGameState(Screen** screens, int state) {
	if (screens && screens[GAME_SCREEN]) {
		int i;
		for (i = GS_CHECK; i < GS_CHECK + 3; i++) {
			if (screens[GAME_SCREEN]->widgets[i]) {
				screens[GAME_SCREEN]->widgets[i]->shown = i == state ? SHOWN : HIDE;
			}
		}
	}
}

/**
*  Updates the game state indicating label according to the current state.
*
*  @param screens - pointer to the game's screen array.
*  @param game - pointer to the game's struct.
*/
void SPUpdateGameState(Screen** screens, SPChessGame* game) {
	if (screens && game) {
		switch(game->gameState){
			case NORMAL:
				SPShowGameState(screens, NONE);
				break;
			case CHECK:
				SPShowGameState(screens, GS_CHECK);
				break;
			case CHECKMATE:
				SPShowGameState(screens, GS_CHECK_MATE);
				break;
			case DRAW:
				SPShowGameState(screens, GS_DRAW);
				break;
		}
	}
}

int SPUpdateBoard(Screen** screens, SPChessGame* game){
	if (!screens || !screens[GAME_SCREEN] || !game) {
		return QUIT;
	}

	SDL_Renderer* renderer = screens[GAME_SCREEN]->renderer;
	int success = SDL_RenderClear(renderer); //clear, this is for safety, no need to check,failure won't effect
	if(success == -1){
		printf("ERROR: unable to draw screen: %s. Quitting..\n",SDL_GetError());
	}
	SPDrawBoard(renderer);


	if (game->gameMode == 1 && game->currentPlayer != game->userColor) { // when game starts, its time to perform computer move
		int move = spMinimaxSuggestMove(game);
		if (move == -1) {
			printf("ERROR: Couldn't figure out computer's first move. Returning to main menu...\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									 "ERROR: Couldn't figure out computer's first move. Returning to main menu..", NULL);
			return SPOpenWindow(screens, MAIN_MENU_WINDOW); // this is the first step, we must quit the current game
		}
		else {
			spChessGameSetMove(game, move);
			// update undo availability
			screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = spArrayListIsEmpty(game->history) ? HIDE : SHOWN;
		}
	}


	int row;
	int column;
	int i;
	for (i = 0; i < N_COLUMNS * 4; i++) {
		if (game->locations[i]) { // update location of pieces on the board
			row = spChessGameGetRowFromPosition(game->locations[i]);
			column = spChessGameGetColumnFromPosition(game->locations[i]);
			Sticker* sticker = (Sticker*) screens[GAME_SCREEN]->widgets[i]->data;
			sticker->location.x = 10 + column * 80;
			sticker->location.y = 10 + row * 80;
			screens[0]->widgets[i]->shown = SHOWN; // ensure widget is shown
		}
		else {
			screens[0]->widgets[i]->shown = HIDE; // ensure widget is hidden
		}

	}

	SPUpdateGameState(screens,game);
	SDL_RenderPresent(renderer);

	return PRESSED;
}

int SPRestartGame(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !screens[GAME_SCREEN] || !game || screenIndex != GAME_SCREEN || widgetIndex != GS_RESTART_GAME) {
		return NONE;
	}
    
    spChessGameResetBoard(game);
    spArrayListClear(game->history); // clear undo history
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE; // update saved game indicator

	return SPUpdateBoard(screens,game);
}

int SPShowSaveBeforeQuitMassage(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!screens || !game) {
		return -1;
	}

    const SDL_MessageBoxButtonData buttons[] = { /* .flags, .buttonid, .text */
		{ 0, 0, "no" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
    };

    const SDL_MessageBoxColorScheme colorScheme = {
    		{   /* .colors (.r, .g, .b) */
    			/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
					{ 255, 0, 0 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
					{ 0, 255, 0 },
            	/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
					{ 255, 255, 0 },
            	/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
					{ 0, 0, 255 },
            	/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
					{ 255, 0, 255 }
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

    int buttonId;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonId) < 0) {
		printf("ERROR: Something went wrong! try again...\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: Something went wrong! try again", NULL);
        return PRESSED;
    }
    else if (buttonId == NO) { // user pressed no
    	if (widgetIndex == GS_QUIT) { // user requested quit
    		return QUIT;
    	}
    	else { // user requested main menu
			spChessGameResetGame(game);
			return SPOpenWindow(screens,MAIN_MENU_WINDOW);
		}
    }
    else if (buttonId == YES) {
    	return SPOpenLoadSaveGameWindow(screens,SAVE_GAME_WINDOW,screenIndex);
    }
    else if (buttonId == CANCEL) {
    	return PRESSED;
    }

    return 0;
}

int SPUndoMove(Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	unsigned int move = spArrayListGetFirst(game->history);
	unsigned int userMove = 0;

	spChessGameUndoMove(game);


	if (!spArrayListIsEmpty(game->history)) { // undo another move
		userMove = spArrayListGetFirst(game->history);
		spChessGameUndoMove(game);
	}

	//update undo button availability
	screens[screenIndex]->widgets[widgetIndex]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	//check and update game's state
	if (spChessCheckGameState(game, game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
								 "ERROR: Something went wrong while updating the game state(i.e check,draw.,ect) try again",
								 NULL);
		if (userMove) {
			spChessGameSetMove(game, userMove >> 8);
		}
		spChessGameSetMove(game, move >> 8); // TODO - Fixed?
	}

	return SPUpdateBoard(screens, game);
}

int SPShowEventErrorAndQuit(){
	printf("ERROR: an error occurred while waiting for an event: %s\n",SDL_GetError());
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							 "ERROR:  an error occurred while waiting for an event. Quitting..",
							 NULL);
	return QUIT;

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
	int moveToRestore = 0;
	int secondMoveToRestore = 0; // for later use

	SP_CHESS_GAME_MESSAGE message = spChessGameIsValidMove(game, move);
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("ERROR: Something went wrong! try again...\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move", "ERROR: Something went wrong! try again",
								 NULL);
	}
	else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move", "That's not your piece!", NULL);
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move", "Illegal move!", NULL);
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move", "Illegal move: king is still threatened!",
								 NULL);
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move", "Illegal move: king will be threatened!",
								 NULL);
	}
	else if(message == SP_CHESS_GAME_SUCCESS) {
		if (spArrayListIsFull(game->history)) { // in error case, we might want to undo the user move.
			moveToRestore = spArrayListGetLast(game->history);
		}

		message = spChessGameSetMove(game, move);
		if (message != SP_CHESS_GAME_SUCCESS) {
			printf("ERROR: Something went wrong while setting up a move! try again...\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									 "ERROR: Something went wrong while setting up a move! try again", NULL);
		}
		else if (game->gameMode == 1) {
			if (spArrayListIsFull(game->history)) {
				secondMoveToRestore = spArrayListGetLast(game->history);
			}

			move = spMinimaxSuggestMove(game);
			if (move == -1) {
				printf("ERROR: Couldn't figure out computer move. try again...\n");
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									"ERROR:  Couldn't figure out computer move. try again", NULL);
				spUndoAndRestoreHistory(game, moveToRestore); //undo the user's move
				return SP_CHESS_GAME_INVALID_ARGUMENT;
			}
			else if (move) { // move == 0 indicates computer can't perform any move
				spChessGameSetMove(game, move);
			}
		}

		// check and update gamestate
		if (spChessCheckGameState(game, game->currentPlayer) == SP_CHESS_GAME_INVALID_ARGUMENT) {
			printf("ERROR: Something went wrong while updating the game state (i.e check, draw, etc.). Try again\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
									 "ERROR: Something went wrong while updating the game state(i.e check, draw, etc.). Try again",
									 NULL);
			spUndoAndRestoreHistory(game, secondMoveToRestore); // if an error occurred
			spUndoAndRestoreHistory(game, moveToRestore); // undo the computer and the user's move
		}
	}

	return message;
}

int SPMovePiece(Widget* src, SDL_Event* event, Screen** screens, SPChessGame* game) {
	if (!src || !event || !screens || !game || game->gameState == CHECKMATE || game->gameState == DRAW) {
		return 0;  // oops... or gameOver, no moving possible
	}

	Sticker* piece = (Sticker*) src->data;

	//get the piece current location
	int currentRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int currentColumn = SCREEN_TO_BOARD_LOCATION(piece->location.x);

	int done = 0;
	int success;
	int movement = 0;
	int xDif, yDif;

	while (!done) { // event handling loop
		switch (event->type) {
			case SDL_MOUSEBUTTONDOWN:
				if (event->button.x > piece->location.x && event->button.y > piece->location.y &&
					event->button.x < piece->location.x + piece->location.w &&
					event->button.y < piece->location.y + piece->location.h) {
					xDif = event->button.x - piece->location.x;
					yDif = event->button.y - piece->location.y;
					movement = 1;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				movement = 0;
				done = 1;
				break;
			case SDL_MOUSEMOTION:
				if (movement) { // move the piece
					piece->location.x = event->motion.x - xDif;
					piece->location.y = event->motion.y - yDif;
				}
				break;
			default:
				break;
		}

		screens[GAME_SCREEN]->draw(screens[GAME_SCREEN], GAME_SCREEN); // draw the game screen

		success = SDL_WaitEvent(event);
		if(success == -1){
			return SPShowEventErrorAndQuit();
		}
	}

	// loop ended, get the new location
	piece->location.x = ROUNDED_POSITION(piece->location.x);
	piece->location.y = ROUNDED_POSITION(piece->location.y);
	int destinationRow = SCREEN_TO_BOARD_LOCATION(piece->location.y);
	int destinationColumn = SCREEN_TO_BOARD_LOCATION(piece->location.x);
	int move = setMoveCoordinatesToInt(currentRow, currentColumn,  destinationRow,  destinationColumn);

	SP_CHESS_GAME_MESSAGE massage = spChessGameHandleMove(game, move); // handle the move

	if (massage != SP_CHESS_GAME_SUCCESS) { // no move occurred, set the piece to it's old location
		piece->location.x = BOARD_TO_SCREEN_LOCATION(currentColumn);
		piece->location.y = BOARD_TO_SCREEN_LOCATION(currentRow);
		return PRESSED;
	}

	// check if enabling undo is needed
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history)) ? HIDE : SHOWN;

	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE; // game is not saved anymore

	return SPUpdateBoard(screens, game); // update the board
}

/**
*  Highlights a specific square on the renderer according a given step (containing the
*  information of where to highlight and in what color).
*  @param renderer - pointer the renderer.
*  @param step - the step.
*  @return 0 on success.
*  		   -1 otherwise.
*/
int SPHighlightMove(SDL_Renderer* renderer, int step) {
	if (!renderer) {
		return -1;
	}
	// get the information from the possible step
	char location = spChessGameGetDestinationPositionFromMove(step);
	unsigned int row = spChessGameGetRowFromPosition(location);
	unsigned int column = spChessGameGetColumnFromPosition(location);
	int threat = spChessGameStepWillThreaten(step);
	int capture = spChessGameStepWillCapture(step);
	int x = BOARD_TO_SCREEN_LOCATION(column);
	int y = BOARD_TO_SCREEN_LOCATION(row);
	char image[20];

	// set the right color to load
	if (threat && capture) {
		strcpy(image ,"pics/gs_purple.bmp");
	}
	else if (threat) {
		strcpy(image ,"pics/gs_red.bmp");
	}
	else if (capture) {
		strcpy(image ,"pics/gs_blue.bmp");
	}
	else {
		strcpy(image ,"pics/gs_green.bmp");
	}

	SDL_Rect rect = { .x = x, .y = y, .w = SQUARE_WIDTH, .h = SQUARE_HEIGHT }; // set rect in the right location

	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		printf("ERROR: unable to load image: %s\n", SDL_GetError());
		return -1;
	}

	// set a specific color (white: 255,255,255) to transparent
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // create a texture from the surface image
	if (texture == NULL) {
		printf("ERROR: unable to create texture: %s\n", SDL_GetError());
		return -1;
	}

	SDL_FreeSurface(surface); // surface not needed anymore
	int success = SDL_RenderCopy(renderer, texture, NULL, &rect); // draw the texture to the rect
	if (success == -1) {
		printf("ERROR: unable to create texture: %s\n", SDL_GetError());
		return -1;
	}
	SDL_DestroyTexture(texture);// texture done his job

	return 0;
}

int SPHighlightAllMoves(Widget* src, SDL_Event* event, Screen** screens, SPChessGame* game) {
	if (!src || !event || !screens || !game) {
		return -1;
	}

	Sticker* piece = (Sticker*) src->data;
	SDL_Renderer* renderer = screens[GAME_SCREEN]->renderer;

	// present the legend
	screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND]->draw(screens[GAME_SCREEN]->widgets[GS_GET_MOVES_LEGEND], renderer);

	// get the location of the piece
	unsigned int row =  SCREEN_TO_BOARD_LOCATION(piece->location.y);
	unsigned int column =  SCREEN_TO_BOARD_LOCATION(piece->location.x);
	unsigned char position  = spChessGameSetLocation(row, column);

	SPArrayList* list = spChessGameGetMoves(game,position); // get the list of possible moves
	if (!list) {
		printf("ERORR: couldn't highlight moves due an allocation error\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: could not highlight moves", NULL);
		return PRESSED;
	}

    int numOfMoves = spArrayListSize(list);
	int success;
	int i;
	int done = 0;
	while (!done) { // event loop, done when mouse button is up
		success = 0;
		switch (event->type) {
			case SDL_MOUSEBUTTONDOWN:
				for(i = 0; i < numOfMoves; i++) {
					success += SPHighlightMove(renderer,spArrayListGetAt(list, i));
				}

				if (success < 0) { //if some highlighting has failed
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
											 "ERROR: could not highlight some or all moves", NULL);
				}

				break;
			case SDL_MOUSEBUTTONUP:
				done = 1;
				break;
			default:
				break;
		}

		SDL_RenderPresent(renderer);
		success = SDL_WaitEvent(event);
		if(success == -1){
			return SPShowEventErrorAndQuit();
		}
	}

	spArrayListDestroy(list);

	return PRESSED;
}
