/*
 * SPLoadSaveGameSdl.c
 *
 *  Created on: 10 באפר 2018
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

		SDL_Renderer* rend = window->renderer;

		//Allocate screen's widgets
		sprintf(temp_str,"pics/lsg_select_game_to_%s.bmp",screenName);
		window->widgets[LSG_MASSAGE] = createLable(rend,
									temp_str,0,0,600,100,SHOWN);

		window->widgets[LSG_BOTTOM_COVER] = createLable(rend,
									"pics/lg_bottom_cover.bmp",0,400,600,50,SHOWN);
		window->widgets[LSG_BACK] = createButton(rend,
									"pics/back.bmp",SPOpenPreviousWindow,50,450,250,50,SHOWN);
		window->widgets[LSG_UP_ARRAW] = createButton(rend,
									"pics/up_arrow.bmp",SPMoveScrollbar,565,100,35,35,SHOWN);
		window->widgets[LSG_DOWN_ARRAW] = createButton(rend,
									"pics/down_arrow.bmp",SPMoveScrollbar,565,400-35,35,35,SHOWN);

		int i;
		int (*action)(Screen**,SPChessGame*,int,int) =
				(strcmp(screenName,"Load")==0)?SPLoadChosenGame:SPSaveChosenGame;
		for(i=0; i<NUM_SLOTS; i++){
			sprintf(temp_str,"pics/lsg_slot%d.bmp",i+1);
			window->widgets[LSG_SLOT(i+1)] = createButton(rend,
										temp_str,action,157,NONE,250,50,SHOWN);
		}

		for(i=0; i<NUM_SLOTS; i++){
			window->widgets[LSG_SLOT_INDICATOR(i+1)] = createLable(rend,
									"pics/lsg_indicator.bmp",200,NONE,50,30,HIDE);
		}

		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(window);
		if(!success){
			return NULL;
		}

		return window;

}

int SPDrawLoadSaveScreen(Screen* screen, int screenIndex){
	if(!screen || screenIndex<LOAD_GAME_WINDOW){
		printf("ERROR: unable to draw, screen resources lost\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: unable to draw, screen resources lost", NULL);
		return QUIT;
	}

	SDL_Renderer* rend = screen->renderer;
	int success = SDL_SetRenderDrawColor(rend, BACKGROUND_COLOR);
	if(success == -1){
		SPShowDrawError();
		return QUIT;
	}
	success = SDL_RenderClear(rend);
	if(success == -1){
		SPShowDrawError();
		return QUIT;
	}

	Widget * widget;
	int i,j;

	SDL_Rect scrollRect = { .x = 0, .y = 100, .w = 600, .h = 300 }; // draw a yellow rect to contain the slots
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
	SDL_RenderFillRect (rend, &scrollbarIndicator); //draw the indicator

	//find the right spot for the highest slot
	int firstShownSlotIndex = screen->scrollBarPosition/SLOT_HEIGHT+1;
	int firstShownSlotPosition = 100-screen->scrollBarPosition%SLOT_HEIGHT;

	//update location for the buttons and the 'used' labels, indicating wether the slot is used or not
	Button* slotButton;
	Lable * slotIndicatorLable;
	for(j=0,i=firstShownSlotIndex; i<=firstShownSlotIndex+300/SLOT_HEIGHT; j++,i++){
		widget = screen->widgets[LSG_SLOT(i)];
		if(widget){
			slotButton = (Button *)widget->data;
			slotButton->location.y = firstShownSlotPosition+SLOT_HEIGHT*j;
			widget->draw(widget, rend);
		}
		widget = screen->widgets[LSG_SLOT_INDICATOR(i)];
		if(widget&&widget->shown){
			slotIndicatorLable = (Lable *)widget->data;
			slotIndicatorLable->location.y = firstShownSlotPosition+SLOT_HEIGHT*j+10;
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

	return CONTINUE;
}

int SPMoveScrollbar(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if(!game){  //sanity check
		printf("Erorr, Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"Erorr, Resources for game were lost. quitting", NULL);
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
		return SPDrawLoadSaveScreen(screens[screenIndex],screenIndex);
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
