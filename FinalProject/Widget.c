#include "Widget.h"

void destroyWidget(Widget* src)
{
	if (src != NULL)
		src->destroy(src);
}


Screen* createScreen(int width,int height,
		char* screenName,
		int widgetsSize,
		int shown,
		int previousWindow,
		int nextWindow,
		void (*draw)(Screen*)){

	Screen* screen = (Screen*) malloc(sizeof(Screen));
	if (screen == NULL){
			return NULL;
		}
	int i;
	for( i = 0;i<N_MAX_WIDGETS; i++){ //safety
		screen->widgets[i] = NULL;
	}
	screen->widgetsSize = widgetsSize;

	SDL_Window* window = SDL_CreateWindow(
						screenName,
						SDL_WINDOWPOS_CENTERED,
						SDL_WINDOWPOS_CENTERED,
						width,
						height,
						//SDL_WINDOW_OPENGL);
						SDL_WINDOW_SHOWN);


	if (window == NULL ) {
		printf("ERROR: unable to create window: %s\n", SDL_GetError());
		free(screen);
		return NULL;
	}
	
	printf("window for %s created\n",screenName);


	// create a renderer for the window
	SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (rend == NULL) {
		printf("%s-", screenName);
		printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);	
		free(screen);
		return 0;
	}

	screen->shown = shown;
	if(!shown){
		SDL_HideWindow(window);
	}
	screen->window = window;
	screen->renderer = rend;
	screen->draw = draw;
	screen->scrollBarPosition = 0;
	screen->previousWindow = previousWindow;
	screen->nextWindow = nextWindow;


	return screen;
}

void SPDestroyScreen(Screen* src){
	if(src!= NULL){

		if(src->renderer != NULL){
			SDL_DestroyRenderer(src->renderer);	
		}
		if(src->window != NULL){
			SDL_DestroyWindow(src->window);
		}

		int i;
		for( i = 0; i<src->widgetsSize; i++){
			destroyWidget(src->widgets[i]);
		}


		free(src);
	}

}

void SPDestroyScreensArr(Screen** arr,int size){
	int i;
	for( i = 0; i<size; i++){
		SPDestroyScreen(arr[i]);
	}
}
