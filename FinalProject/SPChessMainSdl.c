/*
 * SPChessMainSdl.c
 *
 *  Created on: 10 ���� 2018
 *      Author: user
 */
#include "SPChessMainSdl.h"
#include "SPChessGameSdl.h"

int SPGameCreateScreens(Screen** screens){
	screens[GAME_SCREEN] = SPGameCreateGameScreen();
	if(!screens[GAME_SCREEN]){
		return 0;
	}
	screens[MAIN_MENU_WINDOW] = SPGameCreateMainMenuWindow();
	if(!screens[MAIN_MENU_WINDOW]){
		return 0;
	}
	screens[GET_MODE_WINDOW] = SPGameCreateGetModeWindow();
	if(!screens[GET_MODE_WINDOW]){
		return 0;
	}
	screens[GET_DIFFICULTY_WINDOW] = SPCreateGetDifficultyWindow();
	if(!screens[GET_DIFFICULTY_WINDOW]){
		return 0;
	}
	screens[GET_COLOR_WINDOW] = SPCreateGetColorWindow();
	if(!screens[GET_COLOR_WINDOW]){
		return 0;
	}
	screens[LOAD_GAME_WINDOW] = SPCreateLoadSaveGameWindow(LOAD_GAME_WINDOW);
	if(!screens[LOAD_GAME_WINDOW]){
		return 0;
	}
	screens[SAVE_GAME_WINDOW] = SPCreateLoadSaveGameWindow(SAVE_GAME_WINDOW);
	if(!screens[SAVE_GAME_WINDOW]){
		return 0;
	}
	
	SPUpdateLoadSaveSlots(screens); //need to know witch slots are used when initializing

	return 1;
}

int SPCheckWidgetsInit(Screen* screen){
	int i;
	for(i=0; i<screen->widgetsSize; i++){
		if(!screen->widgets[i]){
			printf("ERROR: could not create widget. Quitting..\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
								"ERROR: could not create widget. Quitting..\n", NULL);
			return 0;
		}
	}
	return 1;
}

Screen* SPGameCreateMainMenuWindow(){
	//Allocate screen struct
	Screen* mainMenuWindow = createScreen(450,400,
			"Main Menu",
			MAIN_MENU_N_WIDGETS,
			SHOWN,
			NONE,
			GET_MODE_WINDOW,
			SPDrawScreen);

		if (mainMenuWindow == NULL){
			return NULL;
		}
		SDL_Renderer* renderer = mainMenuWindow->renderer;

		//Allocate screen's widgets
		mainMenuWindow->widgets[MM_NEW_GAME] = createButton(renderer,
									"pics/new_game.bmp",SPOpenNextWindow,100,100,250,50,SHOWN);
		mainMenuWindow->widgets[MM_LOAD_GAME] = createButton(renderer,
									"pics/load_game.bmp",SPOpenNextWindow,100,175,250,50,SHOWN);
		mainMenuWindow->widgets[MM_QUIT] = createButton(renderer,
									"pics/quit_mm.bmp",SPQuit,100,250,250,50,SHOWN);
		
		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(mainMenuWindow);
		if(!success){
			return NULL;
		}

		return mainMenuWindow;
}

Screen* SPGameCreateGetModeWindow(){
	//Allocate screen struct
	Screen* getModeWindow = createScreen(600,400,
			"Select Mode:",
			GET_MODE_N_WIDGETS,
			HIDE,
			MAIN_MENU_WINDOW,
			GET_DIFFICULTY_WINDOW,
			SPDrawScreen);
	if (getModeWindow == NULL){
			return NULL;
	}

	SDL_Renderer* renderer = getModeWindow->renderer;

	//Allocate screen's widgets
		getModeWindow->widgets[GM_MASSAGE] = createLabel(renderer,
									"pics/gm_select_game_mode.bmp",100,62,400,50,SHOWN);
		getModeWindow->widgets[GM_1PLAYER] = createButton(renderer,
									"pics/gm_1_player.bmp",SPOpenNextWindow,175,137,250,50,SHOWN);
		getModeWindow->widgets[GM_2PLAYER] = createButton(renderer,
									"pics/gm_2_player.bmp",SPOpenNextWindow,175,212,250,50,SHOWN);
		getModeWindow->widgets[GM_BACK_TO_MM] = createButton(renderer,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);

		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(getModeWindow);
		if(!success){
			return NULL;
		}

		return getModeWindow;
}

Screen* SPCreateGetDifficultyWindow(){
	//Allocate screen struct
	Screen* getDifficultyWindow = createScreen(600,490,
			"Select Difficulty:",
			GET_DIFFICULTY_N_WIDGETS,
			HIDE,
			GET_MODE_WINDOW,
			GET_COLOR_WINDOW,
			SPDrawScreen);
		if (getDifficultyWindow == NULL){
			return NULL;
		}

	SDL_Renderer* renderer = getDifficultyWindow->renderer;

	//Allocate screen's widgets
		getDifficultyWindow->widgets[GD_MASSAGE] = createLabel(renderer,
									"pics/gd_select_game_difficulty.bmp",100,25,400,50,SHOWN);

		char * difficulties[5] = {"pics/gd_amateur.bmp","pics/gd_easy.bmp",
				"pics/gd_moderate.bmp","pics/gd_hard.bmp","pics/gd_expert.bmp"};
		int i;
		for(i=0; i<5; i++){
			getDifficultyWindow->widgets[GD_DIFFICULTY(i+1)] = createButton(renderer,
					difficulties[i],SPOpenNextWindow,175,100+i*60,250,50,SHOWN);
		}
		getDifficultyWindow->widgets[GD_BACK] = createButton(renderer,
									"pics/back.bmp",SPOpenPreviousWindow,50,415,250,50,SHOWN);
		getDifficultyWindow->widgets[GD_BACK_TO_MM] = createButton(renderer,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,415,250,50,SHOWN);


		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(getDifficultyWindow);
		if(!success){
			return NULL;
		}


		return getDifficultyWindow;

}

Screen* SPCreateGetColorWindow(){
	//Allocate screen struct
	Screen* getColorWindow = createScreen(600,400,
			"Select Color:",
			GET_COLOR_N_WIDGETS,
			HIDE,
			GET_DIFFICULTY_WINDOW,
			GAME_SCREEN,
			SPDrawScreen);
		if (getColorWindow == NULL){
			return NULL;
		}

		SDL_Renderer* renderer = getColorWindow->renderer;

		//Allocate screen's widgets
		getColorWindow->widgets[GC_MASSAGE] = createLabel(renderer,
									"pics/gc_select_user_color.bmp",100,62,400,50,SHOWN);
		getColorWindow->widgets[GC_WHITE] = createButton(renderer,
									"pics/gc_white.bmp",SPOpenNextWindow,175,137,250,50,SHOWN);
		getColorWindow->widgets[GC_BLACK] = createButton(renderer,
									"pics/gc_black.bmp",SPOpenNextWindow,175,212,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK] = createButton(renderer,
									"pics/back.bmp",SPOpenPreviousWindow,50,287,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK_TO_MM] = createButton(renderer,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);

		//check if widgets were created successfully
		int success = SPCheckWidgetsInit(getColorWindow);
		if(!success){
			return NULL;
		}


		return getColorWindow;

}

/**
 * Display a massage informing there is a drawing error
*/
void SPShowDrawError(){
	printf("ERROR: unable to draw screen. Quitting..\n");
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
									"ERROR: unable to draw screen. Quitting..", NULL);

}

int SPDrawScreen(Screen* screen,int screenIndex){
	if(screen==NULL){
		printf("ERROR: unable to draw, screen resources lost\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Invalid Move",
										"ERROR: unable to draw, screen resources lost", NULL);
		return QUIT;
	}

	SDL_Renderer* rend = screen->renderer;
	rend  = NULL;
	int success = SDL_SetRenderDrawColor(rend, BACKGROUND_COLOR); //set the right color
	if(success == -1){
		printf("ERROR: unable to draw background for screen: %s\n",SDL_GetError());
	}
	success = SDL_RenderClear(rend); //clear, this is for safety, no need to check,failure won't effect
	if(success == -1){
		printf("ERROR: unable to clear renderer for screen: %s\n",SDL_GetError());
	}
	if(screenIndex==GAME_SCREEN){
		SPDrawBoard(rend);
	}

	SPDrawWidgets(screen);

	SDL_Delay(10);

	SDL_RenderPresent(rend);

	return CONTINUE;

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

int SPOpenWindow(Screen** screens,int window){

	int i;
	for(i = 0; i<NUM_SCREENS; i++){
		if(i == window){
			screens[i]->shown = SHOWN; //show our window
			SDL_ShowWindow(screens[i]->window);
		}
		else{
			screens[i]->shown = HIDE; //hide all the other
			SDL_HideWindow(screens[i]->window);
		}
	}
	return PRESSED;
}




int SPOpenLoadSaveGameWindow(Screen** screens ,int screenIndex , int previousScreen){
	screens[screenIndex]->previousWindow = previousScreen; //update where to return
	screens[screenIndex]->scrollBarPosition = 0; //align scrollbar
	return SPOpenWindow(screens,screenIndex);
}


int SPOpenNextWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	//opening the load/save window if necessary
	if((screenIndex == GAME_SCREEN && widgetIndex==GS_LOAD_GAME) ||
		(screenIndex == MAIN_MENU_WINDOW && widgetIndex==MM_LOAD_GAME)){
		return SPOpenLoadSaveGameWindow(screens,LOAD_GAME_WINDOW,screenIndex);
	}
	if(screenIndex == GAME_SCREEN && widgetIndex==GS_SAVE_GAME){
		return SPOpenLoadSaveGameWindow(screens,SAVE_GAME_WINDOW,screenIndex);
	}

	if(screenIndex == GET_MODE_WINDOW){
		game->gameMode = widgetIndex;//the game mode is the same as the widget index
		//if game mode is 2 players need to jump to game screen
		screens[screenIndex]->nextWindow = (widgetIndex==1?GET_DIFFICULTY_WINDOW:GAME_SCREEN); //if game mode is 2 players need to jump to game screen
	}

	if(screenIndex == GET_DIFFICULTY_WINDOW){
		game->difficulty = widgetIndex; //the difficulty is the same as the widget position
	}

	if(screenIndex == GET_COLOR_WINDOW){
		game->userColor = widgetIndex; //the color is the same as the widget position
		if(widgetIndex==BLACK){ //white starts, so need to update computer first move before game starts
			SPUpdateBoard(screens,game);
		}
	}

	return  SPOpenWindow(screens,screens[screenIndex]->nextWindow);
}

int SPOpenPreviousWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if(!game || widgetIndex > N_MAX_WIDGETS){  //sanity check
		printf("Erorr, Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"Erorr, Resources for game were lost. quitting", NULL);

		return QUIT;
	}

	return SPOpenWindow(screens,screens[screenIndex]->previousWindow);
}

int SPQuit(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	//checks if warning the user from quitting without saving necessary
	if((screenIndex == GAME_SCREEN)&&(!screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown)){
		return SPShowSaveBeforeQuitMassage(screens , game, screenIndex , widgetIndex);
	}
	return QUIT;
}

int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){

	if((screenIndex == GAME_SCREEN)&&  //checks if needs to warn the user from quiting without saving
			(screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown)==HIDE){
		return SPShowSaveBeforeQuitMassage(screens , game, screenIndex , widgetIndex);
	}

	if(screenIndex == GAME_SCREEN){ //for case where game is saved
		spChessGameResetGame(game);
    	screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown = HIDE; //game will not be save anymore
		SPUpdateBoard(screens,game);
	}
	
	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
}


