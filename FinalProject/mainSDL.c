#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPBufferset.h"
#include "SPChessGame.h"
#include "SPChessSettingsSdl.h"
#include "SPChessGameSdl.h"
#include "SPChessParser.h"
#include "Button.h"

/***

void clickButton(){
	SDL_ShowSimpleMessageBox(
		SDL_MESSAGEBOX_INFORMATION,
		"Title",
		"We did it",
		NULL);
}
 */
//Screen* SPGameCreateGameScreen(SPChessGame* game);

int main(int argc, char *argv[]) {
	SP_BUFF_SET();

	// initialize SDL2 for video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	SPChessGame* game = spChessGameCreate();
	int done = 0,i,j;

	Screen* screens[NUM_SCREENS];
	int success = SPGameCreateScreens(screens);
	if(!success){
		done = 1;
	}
	SPUpdateLoadSaveSlots(screens);
	SDL_Event e;
	printf("1\n");

	while(!done){
		SDL_WaitEvent(&e);
			for(i=0; i<NUM_SCREENS; i++){
				if(screens[i]->shown){
					for(j=0; j<screens[i]->widgetsSize; j++){
						if(screens[i]->widgets[j]&&screens[i]->widgets[j]->shown){
							done = screens[i]->widgets[j]->handleEvent(screens[i]->widgets[j],
									&e,screens,game,i,j);
						}
						if(done){
							break;
						}
					}
					break;
				}
			}

		for(i=0; i<NUM_SCREENS; i++){

			if(screens[i]->shown){
				screens[i]->draw(screens[i]);
				break;
			}
		}

	}

	spChessGameDestroy(game);

	SPDestroyScreensArr(screens,NUM_SCREENS);
	printf("%d\n",done);




	SDL_Quit();
	return 0;

}

