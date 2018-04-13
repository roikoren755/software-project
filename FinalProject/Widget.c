#include "Widget.h"

void destroyWidget(Widget* src)
{
	if (src != NULL)
		src->destroy(src);
}

/***
 * Create a new instance of screen, with the given parameters
 * @param screenName - the title of the screen.
 * @param widgetsSize - the number of widgets the screen needs
 * @param action - a function for the button to call for if pressed.
 * @param x - the x coordinate for the button to be placed within the renderer.
 * @param y - the y coordinate for the button to be placed within the renderer.
 * @param w - the width of the button.
 * @param h - the height of the button.
 * @param shown - indicates whether the button is shown or not when initialized (using the defines SHOWN/HIDE).
 *
 * @return NULL if an allocation error occurred.
 *         a pointer to the created widget, otherwise
 */
Screen* createScreen(int width,int height,
		char* screenName,
		int widgetsSize,
		int shown,
		int previousWindow,
		int nextWindow,
		void (*draw)(Screen*,int))
	{

	Screen* screen = (Screen*) malloc(sizeof(Screen));
	if (screen == NULL){
		printf("ERROR: could not allocate data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"ERROR: could not allocate data", NULL);
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
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"ERROR: unable to create window", NULL);
		free(screen);
		return NULL;
	}

	//hide window if necessary
	screen->shown = shown;
	if(!shown){
		SDL_HideWindow(window);
	}


	// create a renderer for the window
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"ERROR: unable to create renderer", NULL);
		SDL_DestroyWindow(window);	
		free(screen);
		return NULL;
	}

	screen->window = window;
	screen->renderer = renderer;
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
