#ifndef WIDGET_H_
#define WIDGET_H_

#include <SDL.h>
#include <SDL_video.h>
#include <stdlib.h>
#include "SPChessGame.h"


#define N_MAX_WIDGETS 43

typedef struct screen_t Screen;
/**
* Widget: A struct containing draw, destroy and handle event function,
* pointer to data, witch in the chess program can be either button, label or sticker.
* The int shown indicates whether the widget should be presented or hiden.
*/
typedef struct widget_t Widget;
struct widget_t {
	int shown;
	void (*draw)(Widget*, SDL_Renderer*);
	int (*handleEvent)(Widget*, SDL_Event*, Screen**, SPChessGame*, int, int);
	void (*destroy)(Widget*);
	void* data;
} ;

/**
* Screen: A struct used to represent a window and all the data needed for it.
* contains an array of widgets, window, renderer, drawing function and additional data.
*/
struct screen_t {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Widget* widgets[N_MAX_WIDGETS];
	int shown;
	int previousWindow;
	int nextWindow;
	int widgetsSize;
	int scrollBarPosition;
	void (*draw)(Screen*, int);
};

/***
 * Destroy's widget, freeing all used memory
 * @param src - Pointer to Widget to destroy
 */
void destroyWidget(Widget* src);

/***
 * Creates a new instance of screen, with the given parameters
 *
 * @param screenName - the title of the screen.
 * @param widgetsSize - the number of widgets the screen requires.
 * @param shown - indicates whether the window is shown when initialized.
 * @param previousWindow - an index of a screen to be related as the created screen's previous screen.
 * @param nextWindow - an index of a screen to be related as the created screen's next screen.
 * @param draw - a drawing function for the screen.
 *
 * @return NULL if an allocation error occurred.
 *         a pointer to the created screen, otherwise
 */
Screen* createScreen(int width, int height, char* screenName, int widgetsSize, int shown, int previousWindow,
					 int nextWindow, void (*draw)(Screen*, int));

/***
 * Destroy's screen, freeing all used memory
 * @param src - Pointer to screen to destroy
 */
void spDestroyScreen(Screen* src);

/***
 * Destroy's an array of screen, freeing all used memory
 * @param arr - Pointer to screens array to destroy
 * @param size - the array's number of elements
 */
void spDestroyScreensArr(Screen** arr, int size);

#endif /* WIDGET_H_ */
