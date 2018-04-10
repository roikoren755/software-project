/*
 * SPChessMainSdl.c
 *
 *  Created on: 10 באפר 2018
 *      Author: user
 */
#include "SPChessMainSdl.h"


Screen* SPCreateLoadSaveGameWindow(int screenIndex);

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
	return 1;
}

Screen* SPGameCreateMainMenuWindow(){
	Screen* mainMenuWindow = createScreen(450,400,
			"Main Menu",
			3,
			SHOWN,
			NONE,
			GET_MODE_WINDOW,
			SPDefaultDrawScreen);

		if (mainMenuWindow == NULL){
			return NULL;
		}
		SDL_Renderer* rend = mainMenuWindow->renderer;

		mainMenuWindow->widgets[MM_NEW_GAME] = createButton(rend,
									"pics/new_game.bmp",SPOpenNextWindow,100,100,250,50,SHOWN);
		mainMenuWindow->widgets[MM_LOAD_GAME] = createButton(rend,
									"pics/load_game.bmp",SPOpenNextWindow,100,175,250,50,SHOWN);
		mainMenuWindow->widgets[MM_QUIT] = createButton(rend,
									"pics/quit_mm.bmp",SPQuit,100,250,250,50,SHOWN);

	return mainMenuWindow;
}

Screen* SPGameCreateGetModeWindow(){
	Screen* getModeWindow = createScreen(600,400,
			"Select Mode:",
			4,
			HIDE,
			MAIN_MENU_WINDOW,
			GET_DIFFICULTY_WINDOW,
			SPDefaultDrawScreen);
		if (getModeWindow == NULL){
			return NULL;
		}

	SDL_Renderer* rend = getModeWindow->renderer;

		getModeWindow->widgets[GM_MASSAGE] = createLable(rend,
									"pics/gm_select_game_mode.bmp",100,62,400,50,SHOWN);
		getModeWindow->widgets[GM_1PLAYER] = createButton(rend,
									"pics/gm_1_player.bmp",SPOpenNextWindow,175,137,250,50,SHOWN);
		getModeWindow->widgets[GM_2PLAYER] = createButton(rend,
									"pics/gm_2_player.bmp",SPOpenNextWindow,175,212,250,50,SHOWN);
		getModeWindow->widgets[GM_BACK_TO_MM] = createButton(rend,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);

		return getModeWindow;
}

Screen* SPCreateGetDifficultyWindow(){
	Screen* getDifficultyWindow = createScreen(600,490,
			"Select Difficulty:",
			8,
			HIDE,
			GET_MODE_WINDOW,
			GET_COLOR_WINDOW,
			SPDefaultDrawScreen);
		if (getDifficultyWindow == NULL){
			return NULL;
		}

	SDL_Renderer* rend = getDifficultyWindow->renderer;

		getDifficultyWindow->widgets[GD_MASSAGE] = createLable(rend,
									"pics/gd_select_game_difficulty.bmp",100,25,400,50,SHOWN);

		char * difficulties[5] = {"pics/gd_amateur.bmp","pics/gd_easy.bmp",
				"pics/gd_moderate.bmp","pics/gd_hard.bmp","pics/gd_expert.bmp"};
		int i;
		for(i=0; i<5; i++){
			getDifficultyWindow->widgets[GD_DIFFICULTY(i+1)] = createButton(rend,
					difficulties[i],SPOpenNextWindow,175,100+i*60,250,50,SHOWN);
		}
		getDifficultyWindow->widgets[GD_BACK] = createButton(rend,
									"pics/back.bmp",SPOpenPreviousWindow,50,415,250,50,SHOWN);
		getDifficultyWindow->widgets[GD_BACK_TO_MM] = createButton(rend,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,415,250,50,SHOWN);


		return getDifficultyWindow;

}

Screen* SPCreateGetColorWindow(){
	Screen* getColorWindow = createScreen(600,400,
			"Select Color:",
			5,
			HIDE,
			GET_DIFFICULTY_WINDOW,
			GAME_SCREEN,
			SPDefaultDrawScreen);
		if (getColorWindow == NULL){
			return NULL;
		}

		SDL_Renderer* rend = getColorWindow->renderer;

		getColorWindow->widgets[GC_MASSAGE] = createLable(rend,
									"pics/gc_select_user_color.bmp",100,62,400,50,SHOWN);
		getColorWindow->widgets[GC_WHITE] = createButton(rend,
									"pics/gc_white.bmp",SPOpenNextWindow,175,137,250,50,SHOWN);
		getColorWindow->widgets[GC_BLACK] = createButton(rend,
									"pics/gc_black.bmp",SPOpenNextWindow,175,212,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK] = createButton(rend,
									"pics/back.bmp",SPOpenPreviousWindow,50,287,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK_TO_MM] = createButton(rend,
									"pics/back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);


		return getColorWindow;

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
			screens[i]->shown = SHOWN;
			SDL_ShowWindow(screens[i]->window);
		}
		else{
			screens[i]->shown = HIDE;
			SDL_HideWindow(screens[i]->window);
		}
	}
	return PRESSED;
}




int SPOpenLoadSaveGameWindow(Screen** screens ,int screenIndex , int previousScreen){
	screens[screenIndex]->previousWindow = previousScreen; //update where to return
	screens[screenIndex]->scrollBarPosition = 0;
	return SPOpenWindow(screens,screenIndex);
}


int SPOpenNextWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
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
		if(widgetIndex==BLACK){ //need to update computer first move before game starts
			SPUpdateBoard(screens,game);
		}
	}

	return  SPOpenWindow(screens,screens[screenIndex]->nextWindow);
}

int SPOpenPreviousWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	return SPOpenWindow(screens,screens[screenIndex]->previousWindow);
}

int SPQuit(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if((screenIndex == GAME_SCREEN)&&(!screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown)){
		return SPShowSaveBeforeQuitMassage(screens , game, screenIndex , widgetIndex);
	}
	return QUIT;
}

int SPOpenMainMenuWindow(Screen** screens ,SPChessGame* game,int screenIndex ,int widgetIndex){
	if((screenIndex == GAME_SCREEN)&&(!screens[GAME_SCREEN]->widgets[GS_SAVED_GAME_INDICATOR]->shown)){
		return SPShowSaveBeforeQuitMassage(screens , game, screenIndex , widgetIndex);
	}

	if(screenIndex == GAME_SCREEN){ //for case where game is saved
		spChessGameResetGame(game);
	}
	return SPOpenWindow(screens,MAIN_MENU_WINDOW);
}


