#include "Widget.h"

void destroyWidget(Widget* src)
{
	if (src != NULL)
		src->destroy(src);
}

Screen* createScreen(int widgetsSize){
	Screen* screen = (Screen*) malloc(sizeof(Screen));
	if (screen == NULL){
			return NULL;
		}
	int i;
	for( i = 0;i<N_MAX_WIDGETS; i++){
		screen->widgets[i] = NULL;
	}
	screen->widgetsSize = widgetsSize;

	return screen;
}

void SPDestroyScreen(Screen* src){
	if(src!= NULL){

		SDL_DestroyRenderer(src->renderer);
		SDL_DestroyWindow(src->window);
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
