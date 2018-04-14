#include "Widget.h"

void destroyWidget(Widget* src) {
	if (src) {
		src->destroy(src);
	}
}

Screen* createScreen(int width, int height, char* screenName, int widgetsSize, int shown, int previousWindow,
					 int nextWindow, void (*draw)(Screen*, int)) {

	Screen* screen = (Screen*) malloc(sizeof(Screen));
	if (!screen) {
		printf("ERROR: could not allocate data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: could not allocate data", NULL);
		return NULL;
	}

	int i;
	for(i = 0; i < N_MAX_WIDGETS; i++) { // safety
		screen->widgets[i] = NULL;
	}
	screen->widgetsSize = widgetsSize;

	SDL_Window* window = SDL_CreateWindow(screenName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
										  SDL_WINDOW_SHOWN);
	if (!window) {
		printf("ERROR: unable to create window: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: unable to create window", NULL);
		free(screen);
		return NULL;
	}

	screen->shown = shown; // hide window if necessary
	if (!shown) {
		SDL_HideWindow(window);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // create a renderer for the window
	if (!renderer) {
		printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: unable to create renderer", NULL);
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

void spDestroyScreen(Screen* src) {
	if (src) {
		if(src->renderer) {
			SDL_DestroyRenderer(src->renderer);	
		}
		if(src->window) {
			SDL_DestroyWindow(src->window);
		}

		int i;
		for (i = 0; i < src->widgetsSize; i++) {
			destroyWidget(src->widgets[i]);
		}

		free(src);
	}
}

void spDestroyScreensArr(Screen** arr, int size) {
	if (arr) {
		int i;
		for (i = 0; i < size; i++) {
			spDestroyScreen(arr[i]);
		}
	}
}
