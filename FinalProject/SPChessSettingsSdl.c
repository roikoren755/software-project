/*
 * SPChessSdl.c
 *
 *  Created on: 19 במרס 2018
 *      Author: user
 */


#include "SPChessSettingsSdl.h"
#include "Widget.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPBufferset.h"
#include "SPChessGame.h"
#include "Button.h"


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
	screens[LOAD_GAME_WINDOW] = SPCreateLoadSaveGameWindow("Load");
	if(!screens[LOAD_GAME_WINDOW]){
		return 0;
	}
	screens[SAVE_GAME_WINDOW] = SPCreateLoadSaveGameWindow("Save");
	if(!screens[SAVE_GAME_WINDOW]){
		return 0;
	}
	return 1;
}

Screen* SPGameCreateGameScreen(){
	Screen* gameScreen = createScreen(900,660,
			"Chess Game",
			N_MAX_WIDGETS,
			HIDE,
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
		strcpy(tempString,colors[color]);
		gameScreen->widgets[LEFT_ROOK_LOC(color)] = createSticker(rend,
				strcat(tempString,"_rook.bmp"),SPMovePiece,SPHighlightAllMoves,10+0*80,10+color*80*7,80,80,SHOWN);
		strcpy(tempString,colors[color]);
		gameScreen->widgets[LEFT_KNIGHT_LOC(color)] = createSticker(rend,
				strcat(tempString,"_knight.bmp"),SPMovePiece,SPHighlightAllMoves,10+1*80,10+color*80*7,80,80,SHOWN);
		strcpy(tempString,colors[color]);
		gameScreen->widgets[LEFT_BISHOP_LOC(color)] = createSticker(rend,
				strcat(tempString,"_bishop.bmp"),SPMovePiece,SPHighlightAllMoves,10+2*80,10+color*80*7,80,80,SHOWN);
		strcpy(tempString,colors[color]);
		gameScreen->widgets[QUEEN_LOC(color)] = createSticker(rend,
				strcat(tempString,"_queen.bmp"),SPMovePiece,SPHighlightAllMoves,10+3*80,10+color*80*7,80,80,SHOWN);;
		strcpy(tempString,colors[color]);
		gameScreen->widgets[KING_LOC(color)] = createSticker(rend,
				strcat(tempString,"_king.bmp"),SPMovePiece,SPHighlightAllMoves,10+4*80,10+color*80*7,80,80,SHOWN);;
		strcpy(tempString,colors[color]);
		gameScreen->widgets[RIGHT_BISHOP_LOC(color)] = createSticker(rend,
				strcat(tempString,"_bishop.bmp"),SPMovePiece,SPHighlightAllMoves,10+5*80,10+color*80*7,80,80,SHOWN);
		strcpy(tempString,colors[color]);
		gameScreen->widgets[RIGHT_KNIGHT_LOC(color)] = createSticker(rend,
				strcat(tempString,"_knight.bmp"),SPMovePiece,SPHighlightAllMoves,10+6*80,10+color*80*7,80,80,SHOWN);
		strcpy(tempString,colors[color]);
		gameScreen->widgets[RIGHT_ROOK_LOC(color)] = createSticker(rend,
				strcat(tempString,"_rook.bmp"),SPMovePiece,SPHighlightAllMoves,10+7*80,10+color*80*7,80,80,SHOWN);

	}
	int i;
	for( i = 8; i<16; i++){
		gameScreen->widgets[i] = createSticker(rend,
				"black_pawn.bmp",SPMovePiece,SPHighlightAllMoves,10+(i-8)*80,10+1*80,80,80,SHOWN);
		gameScreen->widgets[i+N_COLUMNS] = createSticker(rend,
				"white_pawn.bmp",SPMovePiece,SPHighlightAllMoves,10+(i-8)*80,10+6*80,80,80,SHOWN);
	}

	gameScreen->widgets[GS_RESTART_GAME] = createButton(rend,
								"restart_game.bmp",SPRestartGame,672,25,210,50,SHOWN);
	gameScreen->widgets[GS_SAVE_GAME] = createButton(rend,
								"save_game.bmp",SPOpenSaveGameWindow,672,100,210,50,SHOWN);
	gameScreen->widgets[GS_LOAD_GAME] = createButton(rend,
								"load_game_gs.bmp",SPOpenLoadGameWindow,672,175,210,50,SHOWN);
	gameScreen->widgets[GS_MAIN_MENU] = createButton(rend,
								"main_menu_gs.bmp",SPOpenMainMenuWindow,672,250,210,50,SHOWN);
	gameScreen->widgets[GS_QUIT] = createButton(rend,
								"quit_gs.bmp",SPQuit,672,325,210,50,SHOWN);

	gameScreen->widgets[GS_UNDO] = createButton(rend,
								"gs_Undo.bmp",SPUndoMove,672,475,210,50,HIDE);
	gameScreen->widgets[GS_GET_MOVES_LEGEND] = createLable(rend,
								"gs_GetMoves_legend.bmp",672,600,210,50,HIDE);
	gameScreen->widgets[GS_CHECK] = createLable(rend,
									"gs_Check.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_CHECK_MATE] = createLable(rend,
									"gs_Check-mate.bmp",672,540,210,50,HIDE);
	gameScreen->widgets[GS_DRAW] = createLable(rend,
									"gs_Draw.bmp",672,540,210,50,HIDE);




//	for(int i = 0; i<N_MAX_WIDGETS; i++){
//		if(!gameScreen->widgets[i]){
//			continue;
//		}
//		gameScreen->widgets[i]->draw(gameScreen->widgets[i], rend);
//	}


	//SDL_RenderPresent(rend);

	return gameScreen;
}

Screen* SPGameCreateMainMenuWindow(){
	Screen* mainMenuWindow = createScreen(450,400,
			"Main Menu",
			3,
			SHOWN,
			NONE,
			SPDefaultDrawScreen);

		if (mainMenuWindow == NULL){
			return NULL;
		}
		SDL_Renderer* rend = mainMenuWindow->renderer;

		mainMenuWindow->widgets[MM_NEW_GAME] = createButton(rend,
									"new_game.bmp",SPOpenGetModeWindow,100,100,250,50,SHOWN);
		mainMenuWindow->widgets[MM_LOAD_GAME] = createButton(rend,
									"load_game.bmp",SPOpenLoadGameWindow,100,175,250,50,SHOWN);
		mainMenuWindow->widgets[MM_QUIT] = createButton(rend,
									"quit_mm.bmp",SPQuit,100,250,250,50,SHOWN);

	return mainMenuWindow;
}

Screen* SPGameCreateGetModeWindow(){
	Screen* getModeWindow = createScreen(600,400,
			"Select Mode:",
			4,
			HIDE,
			MAIN_MENU_WINDOW,
			SPDefaultDrawScreen);
		if (getModeWindow == NULL){
			return NULL;
		}

	SDL_Renderer* rend = getModeWindow->renderer;

		getModeWindow->widgets[GM_MASSAGE] = createLable(rend,
									"gm_select_game_mode.bmp",100,62,400,50,SHOWN);
		getModeWindow->widgets[GM_1PLAYER] = createButton(rend,
									"gm_1_player.bmp",SPSetGameMode,175,137,250,50,SHOWN);
		getModeWindow->widgets[GM_2PLAYER] = createButton(rend,
									"gm_2_player.bmp",SPSetGameMode,175,212,250,50,SHOWN);
		getModeWindow->widgets[GM_BACK_TO_MM] = createButton(rend,
									"back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);


		return getModeWindow;
}

Screen* SPCreateGetDifficultyWindow(){
	Screen* getDifficultyWindow = createScreen(600,490,
			"Select Difficulty:",
			8,
			HIDE,
			GET_MODE_WINDOW,
			SPDefaultDrawScreen);
		if (getDifficultyWindow == NULL){
			return NULL;
		}

	SDL_Renderer* rend = getDifficultyWindow->renderer;

		getDifficultyWindow->widgets[GD_MASSAGE] = createLable(rend,
									"gd_select_game_difficulty.bmp",100,25,400,50,SHOWN);

		char * difficulties[5] = {"gd_amateur.bmp","gd_easy.bmp","gd_moderate.bmp","gd_hard.bmp","gd_expert.bmp"};
		int i;
		for(i=0; i<5; i++){
			getDifficultyWindow->widgets[GD_DIFFICULTY(i+1)] = createButton(rend,
					difficulties[i],SPSetGameDifficulty,175,100+i*60,250,50,SHOWN);
		}
		getDifficultyWindow->widgets[GD_BACK] = createButton(rend,
									"back.bmp",SPOpenPreviousWindow,50,415,250,50,SHOWN);
		getDifficultyWindow->widgets[GD_BACK_TO_MM] = createButton(rend,
									"back_to_main_menu.bmp",SPOpenMainMenuWindow,300,415,250,50,SHOWN);


		return getDifficultyWindow;

}

Screen* SPCreateGetColorWindow(){
	Screen* getColorWindow = createScreen(600,400,
			"Select Color:",
			5,
			HIDE,
			GET_DIFFICULTY_WINDOW,
			SPDefaultDrawScreen);
		if (getColorWindow == NULL){
			return NULL;
		}

		SDL_Renderer* rend = getColorWindow->renderer;

		getColorWindow->widgets[GC_MASSAGE] = createLable(rend,
									"gc_select_user_color.bmp",100,62,400,50,SHOWN);
		getColorWindow->widgets[GC_WHITE] = createButton(rend,
									"gc_white.bmp",SPSetGameColor,175,137,250,50,SHOWN);
		getColorWindow->widgets[GC_BLACK] = createButton(rend,
									"gc_black.bmp",SPSetGameColor,175,212,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK] = createButton(rend,
									"back.bmp",SPOpenPreviousWindow,50,287,250,50,SHOWN);
		getColorWindow->widgets[GC_BACK_TO_MM] = createButton(rend,
									"back_to_main_menu.bmp",SPOpenMainMenuWindow,300,287,250,50,SHOWN);


		return getColorWindow;

}

Screen* SPCreateLoadSaveGameWindow(char* screenName){
	char temp_str[30];
	sprintf(temp_str,"%s Game",screenName);
	Screen* window = createScreen(600,550,
			temp_str,
			NUM_SLOTS+NUM_SAVE_LOAD_SCREEN_DEFUALT_WIDGETS,
			HIDE,
			GAME_SCREEN,
			SPDrawLoadSaveScreen);
		if (window == NULL){
			return NULL;
		}

		SDL_Renderer* rend = window->renderer;

		sprintf(temp_str,"lsg_select_game_to_%s.bmp",screenName);
		window->widgets[LSG_MASSAGE] = createLable(rend,
									temp_str,0,0,600,100,SHOWN);

		window->widgets[LSG_BOTTOM_COVER] = createLable(rend,
									"lg_bottom_cover.bmp",0,400,600,50,SHOWN);
		window->widgets[LSG_BACK] = createButton(rend,
									"back.bmp",SPOpenPreviousWindow,50,450,250,50,SHOWN);
		window->widgets[LSG_UP_ARRAW] = createButton(rend,
									"up_arrow.bmp",SPMoveScrollbar,565,100,35,35,SHOWN);
		window->widgets[LSG_DOWN_ARRAW] = createButton(rend,
									"down_arrow.bmp",SPMoveScrollbar,565,400-35,35,35,SHOWN);

		int i;
		int (*action)(Screen**,SPChessGame*,int,int) =
				(strcmp(screenName,"Load")==0)?SPLoadChosenGame:SPSaveChosenGame;
		for(i=0; i<NUM_SLOTS; i++){
			sprintf(temp_str,"lsg_slot%d.bmp",i+1);
			window->widgets[LSG_SLOT(i+1)] = createButton(rend,
										temp_str,action,175,NONE,250,50,SHOWN);
		}

		for(i=0; i<NUM_SLOTS; i++){
			window->widgets[LSG_SLOT_INDICATOR(i+1)] = createLable(rend,
									"lsg_indicator.bmp",200,NONE,50,30,HIDE);
		}


		return window;

}
