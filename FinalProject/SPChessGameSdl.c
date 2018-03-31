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
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	SDL_RenderFillRect (rend, & frameRect);

	frameRect.x+=5;
	frameRect.y+=5;
	frameRect.h-=(5*2);
	frameRect.w-=(5*2);
	    // Color from within the rectangle
	SDL_SetRenderDrawColor (rend, 255, 255, 102, 0);
	// Fill in the rectangle
	SDL_RenderFillRect (rend, & frameRect);
	int i,j;
	for( i = 0; i<8; i++){ //Draw board
			for( j = 10+(1-i%2)*80 ; j<640; j+=160){
				SDL_Rect rect = { .x = j, .y = 10+i*80, .w = 80, .h = 80 };
				SDL_SetRenderDrawColor(rend, 204, 102, 0, 0);
				SDL_RenderFillRect(rend, &rect);
			}
		}
	return 0;
}

int SPUpdateBoard(Screen** screens, SPChessGame* game){
	SDL_Renderer* rend = screens[0]->renderer;
	SPDrawBoard(rend);
	int raw,col;
	int i;
	for( i = 0; i<32; i++){

		if(game->locations[i]){
			raw = spChessGameGetRowFromPosition(game->locations[i]);
			col = spChessGameGetColumnFromPosition(game->locations[i]);
//printf("%c-<%d,%d>\n",game->gameBoard[raw][col],col,raw);
			Sticker * sticker = (Sticker *)screens[0]->widgets[i]->data;
			sticker->location.x = 10+col*80;
			sticker->location.y = 10+raw*80;
			screens[0]->widgets[i]->draw(screens[0]->widgets[i],rend);

		}

	}
	SDL_RenderPresent(rend);

	return 0;
}

void SPShowGameState(Screen** screens, int state){
	int i;
	for(i=GS_CHECK; i<3; i++){
		screens[GAME_SCREEN]->widgets[GS_CHECK]->shown = (i==state?1:0);
	}
}

int SPUpdateGameState(Screen** screens, SPChessGame* game){

	SP_CHESS_GAME_MESSAGE message = spChessCheckGameState(game, game->currentPlayer);
	if (message != SP_CHESS_GAME_SUCCESS) {
		if (message == SP_CHESS_GAME_CHECK) {
			SPShowGameState(screens,GS_CHECK);
		}
		else if (message == SP_CHESS_GAME_CHECKMATE) {
			SPShowGameState(screens,GS_CHECK_MATE);
			}
		else if (message == SP_CHESS_GAME_DRAW) {
			SPShowGameState(screens,DRAW);
			}
		else {
				printf("ERROR: Something went wrong while checking game state. Quitting...\n");
				return -1;
			}
		}
	else{
		SPShowGameState(screens,NONE); //dont show any state

	}
	return 1;
}




int SPDrawScreen(Screen** screens, int window){
	SDL_Renderer* rend = screens[window]->renderer;
	SDL_SetRenderDrawColor(rend, 255, 204, 153, 0);
	SDL_RenderClear(rend);

	if(window == GAME_SCREEN){
		SPDrawBoard(rend);
	}
	//SDL_Delay(50);
	//printf("%d.drawing window %d\n",var++,window);

	Widget * widget;
	int i = 0;
	for(i=0; i<screens[window]->widgetsSize; i++){
		widget = screens[window]->widgets[i];
		if(widget && widget->shown){
			widget->draw(widget, rend);
		}
	}

	SDL_Delay(10);
	SDL_RenderPresent(rend);

	return 1;
}



int SPRestartGame(Screen** screens ,SPChessGame* game,int j){
    spChessGameResetBoard(game);
    SPUpdateBoard(screens,game);
	return 0;
}
int SPOpenSaveGameWindow(Screen** screens ,SPChessGame* game,int j){
	return 0;
}
int SPOpenLoadGameWindow(Screen** screens ,SPChessGame* game,int j){
	return 0;
}

int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int j){
	printf("%d.opening main menu\n",var++);
	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
}

int SPOpenGetModeWindow(Screen** screens ,SPChessGame* game,int j){
	printf("%d.opening get mode\n",var++);

	return SPOpenWindow(screens,GET_MODE_WINDOW);
}

int SPOpenGetDifficultyWindow(Screen** screens ,SPChessGame* game,int j){
	printf("%d.opening GetDifficulty\n",var++);
	return SPOpenWindow(screens,GET_DIFFICULTY_WINDOW);
}

int SPOpenGetColorWindow(Screen** screens ,SPChessGame* game,int j){
	printf("%d.opening GetColorWindow\n",var++);
	return SPOpenWindow(screens,GET_COLOR_WINDOW);
}

int SPOpenGameScreen(Screen** screens ,SPChessGame* game,int j){
	printf("%d.opening GameScreen\n",var++);
	return SPOpenWindow(screens,GAME_SCREEN);
}

int SPQuit(Screen** screens ,SPChessGame* game,int j){
	return 1;
}

int SPUndoMove(Screen** screens ,SPChessGame* game,int j){
	spChessGameUndoMove(game);

	//update undo button availability
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;

	SPUpdateBoard(screens , game);
	return 0;
}

int SPSetGameMode(Screen** screens ,SPChessGame* game,int j){
	game->gameMode = j;//the game mode is the same as the widget position
	return SPOpenWindow(screens,j==1?GET_DIFFICULTY_WINDOW:GAME_SCREEN);
}
int SPSetGameDifficulty(Screen** screens ,SPChessGame* game,int j){
	game->difficulty = j; //the difficulty is the same as the widget position
	return SPOpenWindow(screens,GET_COLOR_WINDOW);
}

int SPSetGameColor(Screen** screens ,SPChessGame* game,int j){
	game->userColor = j; //the color is the same as the widget position
	return SPOpenWindow(screens,GAME_SCREEN);
}


int SPLoadChosenGame(Screen** screens ,SPChessGame* game,int j){
	return 0;
}

int SPOpenWindow(Screen** screens,int window){
	int i;
	for(i = 0; i<NUM_SCREENS; i++){
		if(i == window){
			screens[i]->shown = 1;
			printf("%d.open window: %d\n",var++,i);
			SDL_ShowWindow(screens[i]->window);
		}
		else{
			screens[i]->shown = 0;
			SDL_HideWindow(screens[i]->window);
		}
	}
	return 0;
}


int SPMovePiece(Widget* src, SDL_Event* event,Screen** screens, SPChessGame* game){
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
		SPDrawScreen(screens,GAME_SCREEN);

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

	if(massage!=SP_CHESS_GAME_SUCCESS ){ //no move occurred
		printf("%d.no move, raw-%d,col-%d,\n",var++,curRow,curCol);

		piece->location.x = BOARD_TO_SCREEN_LOCATION(curCol);
		piece->location.y = BOARD_TO_SCREEN_LOCATION(curRow);
		return 0;
	}

	int success = SPUpdateGameState(screens,game);

	if(!success){
		//TODO undo
	}

	// check if enabling undo is required
	screens[GAME_SCREEN]->widgets[GS_UNDO]->shown = (spArrayListIsEmpty(game->history))?HIDE:SHOWN;
	if(spArrayListIsEmpty(game->history)){
		printf("empty, shown=%d\n",screens[GAME_SCREEN]->widgets[GS_UNDO]->shown);
	}
	else{printf("undo available, shown=%d\n",screens[GAME_SCREEN]->widgets[GS_UNDO]->shown);}

	SPUpdateBoard(screens,game);
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
