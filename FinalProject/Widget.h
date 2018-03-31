#ifndef WIDGET_H_
#define WIDGET_H_

#include <SDL.h>
#include <SDL_video.h>
#include <stdlib.h>
#include "SPChessGame.h"


#define N_MAX_WIDGETS 45
#define N_MAX_SONS 5
typedef struct screen_t Screen;
typedef struct button_t Button;

typedef struct widget_t Widget;
struct widget_t {
	int shown;
	void (*draw)(Widget*, SDL_Renderer*);
	int (*handleEvent)(Widget*, SDL_Event*,Screen**,SPChessGame*,int);
	void (*destroy)(Widget*);
	Button* data;
};

//typedef struct screen_t Screen;
struct screen_t {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Widget* widgets[N_MAX_WIDGETS];
	int shown;
	int widgetsSize;
};

Screen* createScreen();
// useful function for NULL-safe destroy
void destroyWidget(Widget* src);

void SPDestroyScreen(Screen* src);

void SPDestroyScreensArr(Screen** arr,int size);
#endif /* WIDGET_H_ */
