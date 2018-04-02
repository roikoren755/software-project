/*
 * SPChessGameSdl.c
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */

#include "SPChessGameSdl.h"
#include <string.h>

int var=0;

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
	return 0;
}

int SPUpdateBoard(Screen** screens, SPChessGame* game){
	SDL_Renderer* rend = screens[0]->renderer;
	SDL_RenderClear(rend);
	SPDrawBoard(rend);
	int raw,col;
	int i;
	for( i = 0; i<32; i++){
		if(game->locations[i]){
			raw = spChessGameGetRowFromPosition(game->locations[i]);
			col = spChessGameGetColumnFromPosition(game->locations[i]);
			Sticker * sticker = (Sticker *)screens[0]->widgets[i]->data;
			sticker->location.x = 10+col*80;
			sticker->location.y = 10+raw*80;

			screens[0]->widgets[i]->shown = SHOWN; //ensure widget is shown
			//screens[0]->widgets[i]->draw(screens[0]->widgets[i],rend);
		}
		else{
			screens[0]->widgets[i]->shown = HIDE; //ensure widget is hidden
		}

	}
	int success = SPUpdateGameState(screens,game);

	if(!success){
		//TODO undo
	}

	SDL_RenderPresent(rend);

	return 0;
}

void SPShowGameState(Screen** screens, int state){
	int i;
	for(i=GS_CHECK; i<GS_CHECK+3; i++){
		screens[GAME_SCREEN]->widgets[i]->shown = (i==state)?SHOWN:HIDE;
	}
}

int SPUpdateGameState(Screen** screens, SPChessGame* game){
	switch(game->gameState){
		case NORMAL:
			SPShowGameState(screens,NONE);
			break;
		case GS_CHECK:
			SPShowGameState(screens,NONE);
			break;
		case CHECKMATE:
			SPShowGameState(screens,GS_CHECK_MATE);
			break;
		case DRAW:
			SPShowGameState(screens,GS_DRAW);
			break;
	}
	return 1;
}


void SPDrawWidgets(Screen* screen){
	Widget * widget;
	int i = 0;

	for(i=0; i<screen->widgetsSize; i++){
		widget = screen->widgets[i];
		if(widget && widget->shown){
			widget->draw(widget, screen->renderer);
		}
	}

}

void SPDefaultDrawScreen(Screen* screen){
	if(screen==NULL){
		return;
	}
	SDL_Renderer* rend = screen->renderer;
	SDL_SetRenderDrawColor(rend, BACKGROUND_COLOR);
	SDL_RenderClear(rend);

	SPDrawWidgets(screen);

	SDL_Delay(10);
	SDL_RenderPresent(rend);

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

void SPDrawLoadSaveScreen(Screen* screen){
	SDL_Renderer* rend = screen->renderer;
	SDL_SetRenderDrawColor(rend, BACKGROUND_COLOR);
	SDL_RenderClear(rend);

	Widget * widget;
	int i,j;

	SDL_Rect scrollRect = { .x = 0, .y = 100, .w = 600, .h = 300 };
	SDL_SetRenderDrawColor(rend, CHESS_YELLOW_COLOR);
	SDL_RenderFillRect (rend, & scrollRect);

	SDL_Rect scrollbar = { .x = 565, .y = 135, .w = 35, .h = 300 };
	SDL_SetRenderDrawColor(rend, 224,224,224,0); //draw scrollbar in gray
	SDL_RenderFillRect (rend, & scrollbar);

	float scrollbarIndicatorPos,scrollbarMaxPos = SCROLLBAR_MAX_POSITION;
	if(scrollbarMaxPos==0){
		scrollbarIndicatorPos = 135;
	}
	else{               //calculate relative position of scrollbar Indicator
		scrollbarIndicatorPos = 135+screen->scrollBarPosition*(180/scrollbarMaxPos);
	}
	SDL_Rect scrollbarIndicator = { .x = 565, .y = (int)scrollbarIndicatorPos, .w = 35, .h = 50 };
	SDL_SetRenderDrawColor(rend, 160,160,160,0); // darker gray
	SDL_RenderFillRect (rend, &scrollbarIndicator);


	int firstShownSlotIndex = screen->scrollBarPosition/SLOT_HEIGHT+1;
	int firstShownSlotPosition = 100-screen->scrollBarPosition%SLOT_HEIGHT;

	for(j=0,i=firstShownSlotIndex; i<=firstShownSlotIndex+300/SLOT_HEIGHT; j++,i++){
		widget = screen->widgets[LSG_SLOT(i)];
		if(widget){
			widget->data->location.y = firstShownSlotPosition+SLOT_HEIGHT*j;
			widget->draw(widget, rend);
		}
		widget = screen->widgets[LSG_SLOT_INDICATOR(i)];
		if(widget&&widget->shown){
			widget->data->location.y = firstShownSlotPosition+SLOT_HEIGHT*j+10;
			widget->draw(widget, rend);
		}

	}


	for(i=0; i<NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS; i++){//draw other widgets
		widget = screen->widgets[i];
		if(widget && widget->shown){
			widget->draw(widget, rend);
		}
	}

	SDL_Delay(10);
	SDL_RenderPresent(rend);

}

int SPMoveScrollbar(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){

		if(widgetIndex == LSG_UP_ARRAW){
				screens[screenIndex]->scrollBarPosition -= 7;
		}
		else if(widgetIndex == LSG_DOWN_ARRAW){
					screens[screenIndex]->scrollBarPosition += 7;
		}

		if(screens[screenIndex]->scrollBarPosition <= 0){ //make sure we are in bounds
					screens[screenIndex]->scrollBarPosition = 0;
		}
		if(screens[screenIndex]->scrollBarPosition >= SCROLLBAR_MAX_POSITION){
					screens[screenIndex]->scrollBarPosition = SCROLLBAR_MAX_POSITION;
		}
		SPDrawLoadSaveScreen(screens[screenIndex]);

	return 0;
}

int SPOpenPreviousWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	return SPOpenWindow(screens,screens[screenIndex]->previousWindow);
}

int SPRestartGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
    spChessGameResetBoard(game);
    SPUpdateBoard(screens,game);
	return 0;
}

int SPOpenSaveGameWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	screens[SAVE_GAME_WINDOW]->previousWindow = screenIndex; //update where to return
	return SPOpenWindow(screens,SAVE_GAME_WINDOW);
	return 0;
}
int SPOpenLoadGameWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	screens[LOAD_GAME_WINDOW]->previousWindow = screenIndex; //update where to return
	return SPOpenWindow(screens,LOAD_GAME_WINDOW);
}

int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	printf("%d.opening main menu\n",var++);
	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
}

int SPOpenGetModeWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	printf("%d.opening get mode\n",var++);
	spChessGameResetBoard(game);   //this function only called when the button 'new game' is pressed.
	SPUpdateBoard(screens,game);
	return SPOpenWindow(screens,GET_MODE_WINDOW);
}

int SPOpenGetDifficultyWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	printf("%d.opening GetDifficulty\n",var++);
	return SPOpenWindow(screens,GET_DIFFICULTY_WINDOW);
}

int SPOpenGetColorWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	printf("%d.opening GetColorWindow\n",var++);
	return SPOpenWindow(screens,GET_COLOR_WINDOW);
}

int SPOpenGameScreen(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	printf("%d.opening GameScreen\n",var++);
	return SPOpenWindow(screens,GAME_SCREEN);
}

int SPQuit(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	return 1;
}

int SPUndoMove(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	spChessGameUndoMove(game);

	//update undo button availability
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	SPUpdateBoard(screens , game);
	return 0;
}

int SPSetGameMode(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	game->gameMode = widgetIndex;//the game mode is the same as the widget index
	return SPOpenWindow(screens,widgetIndex==1?GET_DIFFICULTY_WINDOW:GAME_SCREEN);
}
int SPSetGameDifficulty(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	game->difficulty = widgetIndex; //the difficulty is the same as the widget position
	return SPOpenWindow(screens,GET_COLOR_WINDOW);
}

int SPSetGameColor(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	game->userColor = widgetIndex; //the color is the same as the widget position
	return SPOpenWindow(screens,GAME_SCREEN);
}


int SPLoadChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	int slotIndex = WIDGET_TO_SLOT_INDEX(widgetIndex);
	if(!screens[screenIndex]->widgets[LSG_SLOT_INDICATOR(slotIndex)]->shown){
		return 0;    //slot has no file
	}
	char fileName[30];
	sprintf(fileName,"game_slot%d.txt",slotIndex);
	SP_CHESS_GAME_MESSAGE massage = spChessLoadGame(game,fileName);
	if(massage != SP_CHESS_GAME_SUCCESS){
		printf("ERROR: could not load the game\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: could not load the game", NULL);
	}
	SPUpdateLoadSaveSlots(screens);
	return 	SPUpdateBoard(screens, game);

}

int SPSaveChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	int slotIndex = WIDGET_TO_SLOT_INDEX(widgetIndex);
	char fileName[30];
	sprintf(fileName,"game_slot%d.txt",slotIndex);
	SP_CHESS_GAME_MESSAGE massage = spChessSaveGame(game,fileName);
	if(massage != SP_CHESS_GAME_SUCCESS){
		printf("ERROR: could not save the game\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: could not save the game", NULL);
	}
	SPUpdateLoadSaveSlots(screens);
	return 0;
}

void SPUpdateLoadSaveSlots(Screen** screens){
	int i;
	char fileName[30];
	for(i=0; i<NUM_SLOTS; i++){
		sprintf(fileName,"game_slot%d.txt",i+1);
		int shown = (access(fileName,F_OK) != -1) ? SHOWN : HIDE;
		screens[LOAD_GAME_WINDOW]->widgets[LSG_SLOT_INDICATOR(i+1)]->shown =  shown;
		screens[SAVE_GAME_WINDOW]->widgets[LSG_SLOT_INDICATOR(i+1)]->shown =  shown;
	}
}

int SPOpenWindow(Screen** screens,int window){
	int i;
	for(i = 0; i<NUM_SCREENS; i++){
		if(i == window){
			screens[i]->shown = SHOWN;
			printf("%d.open window: %d\n",var++,i);
			SDL_ShowWindow(screens[i]->window);
		}
		else{
			screens[i]->shown = HIDE;
			SDL_HideWindow(screens[i]->window);
		}
	}
	return 0;
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
	else if (message == SP_CHESS_GAME_KING_BECOMES_THREATENED) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"Illegal move: king will be threatened!", NULL);
	}
	else if(message == SP_CHESS_GAME_SUCCESS){
		int full = 0, lastMove = 0;
		if (spArrayListIsFull(game->history)) { // in error case, we might want to undo the user move.
			lastMove = spArrayListGetLast(game->history); // A move will be lost if we add another to the game's history
			full = 1; 									// So save it for later
		}
		message = spChessGameSetMove(game, move);
		if (message != SP_CHESS_GAME_SUCCESS) {
				printf("ERROR: Something went wrong while setting up a move! try again...\n");
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
											"ERROR: Something went wrong while setting up a move! try again", NULL);
		}
		else if(game->gameMode == 1) {
				move = spMinimaxSuggestMove(game);
				printf("%d__",move);
				if (move == -1) {
					printf("ERROR: Couldn't figure out computer move. try again...\n");
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
																"ERROR:  Couldn't figure out computer move move. try again", NULL);
				    int success = spUndoAndRestoreHistory(game, lastMove, full);
				    if(!success){//TODO
				    }
				 	return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
				else {
					char piece = spChessGameGetPieceAtPosition(game, spChessGameGetCurrentPositionFromMove(move << 8));
					spPrintComputerMove(piece, move);
					spChessGameSetMove(game, move);
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
		return 0;
	}



	// check if enabling undo is required
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;
//	if(spArrayListIsEmpty(game->history)){
//		printf("empty, shown=%d\n",screens[GAME_SCREEN]->widgets[GS_UNDO]->shown);
//	}
//	else{printf("undo available, shown=%d\n",screens[GAME_SCREEN]->widgets[GS_UNDO]->shown);}

	SPUpdateBoard(screens,game);
	printLocations(game);
	return 0;
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

	return 0;

}

int SPHighlightMove(SDL_Renderer* rend , int step){
	   char location = spChessGameGetDestinationPositionFromMove(step);
	   unsigned int row = spChessGameGetRowFromPosition(location);
	   unsigned int column = spChessGameGetColumnFromPosition(location);
	   int threat = spChessGameStepWillThreaten(step);
	   int capture = spChessGameStepWillCapture(step);
	   int x = BOARD_TO_SCREEN_LOCATION(column);
	   int y = BOARD_TO_SCREEN_LOCATION(row);
	   char image[10];

		printf("%d.highlighting:<%d,%d>\n",var++,x,y);

	   if(threat&capture){
		   strcpy(image ,"gs_purple.bmp");
	   }
	   else if(threat){
		   strcpy(image ,"gs_red.bmp");
	   }
	   else if(capture){
		   strcpy(image ,"gs_blue.bmp");
	   }
	   else{
		   strcpy(image ,"gs_green.bmp");
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

	   	return 0;

}

int SPCloseWindow(Screen** screens ,SPChessGame* game,int j){
	return 0;
}

int SPNoOperation(Screen** screens ,SPChessGame* game,Widget * src){
	return 0;
}

void printLocations(SPChessGame* game){
	printf("%d.",var++);
	for(int i=0;i<32;i++){
		printf("<%d,%d>\n",
					spChessGameGetRowFromPosition(game->locations[i]),
					spChessGameGetColumnFromPosition(game->locations[i]));

	}
}
