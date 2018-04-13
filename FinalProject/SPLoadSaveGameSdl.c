/*
 * SPLoadSaveGameSdl.c
 *
 *  Created on: 10 ���� 2018
 *      Author: user
 */

#include "SPLoadSaveGameSdl.h"


Screen* SPCreateLoadSaveGameWindow(int screenIndex){
	//set right string
	char screenName[5];
	if(screenIndex==LOAD_GAME_WINDOW ) {strcpy(screenName,"Load");
	}
	else if(screenIndex==SAVE_GAME_WINDOW ) {strcpy(screenName,"Save");
	}

	char temp_str[35];
	sprintf(temp_str,"%s Game",screenName);

	//Allocate screen struct
	Screen* window = createScreen(600,550,
			temp_str,
			LOAD_SAVE_N_WIDGETS,
			HIDE,
			GAME_SCREEN,
			NONE,
			SPDrawLoadSaveScreen);
		if (window == NULL){
			return NULL;
		}

		SDL_Renderer* renderer = window->renderer;

		//Allocate screen's widgets
		sprintf(temp_str,"pics/lsg_select_game_to_%s.bmp",screenName);
		window->widgets[LSG_MASSAGE] = createLabel(renderer,
									temp_str,0,0,600,100,SHOWN);

		window->widgets[LSG_BOTTOM_COVER] = createLabel(renderer,
									"pics/lg_bottom_cover.bmp",0,400,600,50,SHOWN);
		window->widgets[LSG_BACK] = createButton(renderer,
									"pics/back.bmp",SPOpenPreviousWindow,50,450,250,50,SHOWN);
		window->widgets[LSG_UP_ARRAW] = createButton(renderer,
									"pics/up_arrow.bmp",SPMoveScrollbar,565,100,35,35,SHOWN);
		window->widgets[LSG_DOWN_ARRAW] = createButton(renderer,
									"pics/down_arrow.bmp",SPMoveScrollbar,565,400-35,35,35,SHOWN);

		int i;
		int (*action)(Screen**,SPChessGame*,int,int) =
				(strcmp(screenName,"Load")==0)?SPLoadChosenGame:SPSaveChosenGame;
		for(i=0; i<NUM_SLOTS; i++){
			sprintf(temp_str,"pics/lsg_slot%d.bmp",i+1);
			window->widgets[LSG_SLOT(i+1)] = createButton(renderer,
										temp_str,action,157,NONE,250,50,SHOWN);
		}

		for(i=0; i<NUM_SLOTS; i++){
			window->widgets[LSG_SLOT_INDICATOR(i+1)] = createLabel(renderer,
									"pics/lsg_indicator.bmp",200,NONE,50,30,HIDE);
		}

		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(window);
		if(!success){
			SPDestroyScreen(window);
			return NULL;
		}

		return window;

}

void SPDrawLoadSaveScreen(Screen* screen, int screenIndex){
	if(!screen || screenIndex<LOAD_GAME_WINDOW){
		printf("ERROR: unable to draw, screen resources lost\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: unable to draw, screen resources lost", NULL);
	}

	SDL_Renderer* renderer = screen->renderer;
	int success = SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}
	success = SDL_RenderClear(renderer);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}

	Widget * widget;
	int i,j;

	SDL_Rect scrollRect = { .x = 0, .y = 100, .w = 600, .h = 300 }; // draw a yellow rect to contain the slots
	success = SDL_SetRenderDrawColor(renderer, CHESS_YELLOW_COLOR);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}
	SDL_RenderFillRect (renderer, &scrollRect);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}

	SDL_Rect scrollbar = { .x = 565, .y = 135, .w = 35, .h = 300 };
	SDL_SetRenderDrawColor(renderer, 224,224,224,0); //draw scrollbar in gray
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}
	SDL_RenderFillRect (renderer, &scrollbar);
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}

	float scrollbarIndicatorPos,scrollbarMaxPos = SCROLLBAR_MAX_POSITION;
	if(scrollbarMaxPos==0){
		scrollbarIndicatorPos = 135;
	}
	else{               //calculate relative position of scrollbar Indicator
		scrollbarIndicatorPos = 135+screen->scrollBarPosition*(180/scrollbarMaxPos);
	}
	SDL_Rect scrollbarIndicator = { .x = 565, .y = (int)scrollbarIndicatorPos, .w = 35, .h = 50 };
	SDL_SetRenderDrawColor(renderer, 160,160,160,0); // darker gray
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());

	}
	SDL_RenderFillRect (renderer, &scrollbarIndicator); //draw the indicator
	if(success == -1){
		printf("ERROR: unable to draw screen: %s\n",SDL_GetError());
	}

	//find the right spot for the highest slot
	int firstShownSlotIndex = screen->scrollBarPosition/SLOT_HEIGHT+1;
	int firstShownSlotPosition = 100-screen->scrollBarPosition%SLOT_HEIGHT;

	//update location for the buttons and the 'used' labels, indicating wether the slot is used or not
	Button* slotButton;
	Label * slotIndicatorLabel;
	for(j=0,i=firstShownSlotIndex; i<=firstShownSlotIndex+300/SLOT_HEIGHT; j++,i++){
		widget = screen->widgets[LSG_SLOT(i)];
		if(widget){
			slotButton = (Button *)widget->data;
			slotButton->location.y = firstShownSlotPosition+SLOT_HEIGHT*j;
			widget->draw(widget, renderer);
		}
		widget = screen->widgets[LSG_SLOT_INDICATOR(i)];
		if(widget&&widget->shown){
			slotIndicatorLabel = (Label *)widget->data;
			slotIndicatorLabel->location.y = firstShownSlotPosition+SLOT_HEIGHT*j+10;
			widget->draw(widget, renderer);
		}

	}


	for(i=0; i<NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS; i++){//draw other widgets
		widget = screen->widgets[i];
		if(widget && widget->shown){
			widget->draw(widget, renderer);
		}
	}

	SDL_Delay(10);
	SDL_RenderPresent(renderer);

}

int SPMoveScrollbar(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if(!game){  //sanity check
		printf("ERROR, Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"ERROR, Resources for game were lost. quitting", NULL);
		return QUIT;
	}

		//move the scrollbar position
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
		//draw the window
		SPDrawLoadSaveScreen(screens[screenIndex],screenIndex);
		return PRESSED;
}


int SPLoadChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	int slotIndex = WIDGET_TO_SLOT_INDEX(widgetIndex);
	
	if(!screens[screenIndex]->widgets[LSG_SLOT_INDICATOR(slotIndex)]->shown){
		return PRESSED;    //slot has no file
	}
	
	char fileName[30];
	sprintf(fileName,"games_saved/game_slot%d.txt",slotIndex); //load the game
	SP_CHESS_GAME_MESSAGE massage = spChessLoadGame(game,fileName);
	if(massage != SP_CHESS_GAME_SUCCESS){
		printf("ERROR: could not load the game\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: could not load the game", NULL);
		return PRESSED;
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Load Game",
									"Game Loaded Successfully", NULL);

	SPUpdateLoadSaveSlots(screens);
	//show the game saved indicator
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = SHOWN;
	//update the board
	SPUpdateBoard(screens, game);
	return 	SPOpenWindow(screens,GAME_SCREEN);

}

int SPSaveChosenGame(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	int slotIndex = WIDGET_TO_SLOT_INDEX(widgetIndex);
	char fileName[30];
	sprintf(fileName,"games_saved/game_slot%d.txt",slotIndex);
	SP_CHESS_GAME_MESSAGE massage = spChessSaveGame(game,fileName);
	if(massage != SP_CHESS_GAME_SUCCESS){
		printf("ERROR: could not save the game\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: could not save the game", NULL);
		return PRESSED;
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Save Game",
									"Game Saved Successfully", NULL);

	//update slots indicators
	screens[LOAD_GAME_WINDOW]->widgets[LSG_SLOT_INDICATOR(slotIndex)]->shown =  SHOWN;
	screens[screenIndex]->widgets[LSG_SLOT_INDICATOR(slotIndex)]->shown =  SHOWN;
	//show the game saved indicator
	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = SHOWN;
	return PRESSED;
}

void SPUpdateLoadSaveSlots(Screen** screens){
	int i;
	char fileName[30];
	for(i=0; i<NUM_SLOTS; i++){  //for every file name, check if exist
		sprintf(fileName,"games_saved/game_slot%d.txt",i+1);
		int shown = (access(fileName,F_OK) != -1) ? SHOWN : HIDE;
		screens[LOAD_GAME_WINDOW]->widgets[LSG_SLOT_INDICATOR(i+1)]->shown =  shown;
		screens[SAVE_GAME_WINDOW]->widgets[LSG_SLOT_INDICATOR(i+1)]->shown =  shown;
	}
}
