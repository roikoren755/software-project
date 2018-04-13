#ifndef BUTTON_H_
#define BUTTON_H_

#include "SPChessGame.h"
#include "Widget.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define QUIT 2
#define PRESSED 1
#define NONE 0

/**
* A button struct, containing texture, location, pointer to a renderer,
* and an action function. used as data within a widget
*/
typedef struct button_t Button;
struct button_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	int (*action)(Screen** screen ,SPChessGame* game,int screenIndex ,int widgetIndex);
};


/**
* A label struct, containing texture, location, pointer to a renderer,
* This struct has no action. also used for covering other widgets, unabling
* pressing them
*/
typedef struct label_t Lable;
struct label_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
};

/**
* A sticker struct, containing texture, location, pointer to a renderer,
* and to action, for left and right click.
* the action functions receive the widget itself and the event caused calling the function,
* in order to enable moving the sticker easily
*/
typedef struct sticker_t Sticker;
struct sticker_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	int (*leftAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game);
	int (*rightAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game);
};

/***
 * Create a new widget containing a button, with the given parameters
 * @param renderer - renderer for the widget to associate.
 * @param image - a path to the relevant image.
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
Widget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	int (*action)(Screen** screen ,SPChessGame* game,int screenIndex ,int widgetIndex),
	int x, int y, int w, int h,int shown);

/***
 * Destroy's button, freeing all used memory
 * @param src - Pointer to Widget containing the button to destroy
 */
void destroyButton(Widget* src);

/***
 * handles the button event:
 * checks if the button is pressed, if does, calls the function within the button struct.
 * @param src - Pointer to the Widget
 * @param e - the event to handle
 * @param screens - Pointer to the array containing the used screens for the game
 * @param game - Pointer to the chess game struct
 * @param screenIndex - the Index of the screen containing the source widget
 * @param widgetIndex - the Index of the widget within the screen widget array
 *
 * @return NONE if the button wasn't pressed.
 * 		   otherwise, calls the button's action function and returns its value
 */
int handleButtonEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);

/***
 * draws the button in the given renderer.
 * @param src - Pointer to the Widget
 * @param renderer - the renderer to draw in
 */
void drawButton(Widget* src , SDL_Renderer* renderer);

/***
 * Create a new widget containing a lable, with the given parameters
 * @param renderer - renderer for the widget to associate.
 * @param image - a path to the relevant image.
 * @param x - the x coordinate for the lable to be placed within the renderer.
 * @param y - the y coordinate for the lable to be placed within the renderer.
 * @param w - the width of the lable.
 * @param h - the height of the lable.
 * @param shown - indicates whether the lable is shown or not when initialized (using the defines SHOWN/HIDE).
 *
 * @return NULL if an allocation error occurred.
 *         a pointer to the created widget, otherwise
 */
Widget* createLable(
	SDL_Renderer* renderer,
	const char* image,
	int x, int y, int w, int h,int shown);


/***
 * Destroy's lable, freeing all used memory
 * @param src - Pointer to Widget containing the lable to destroy
 */
void destroyLable(Widget* src);

/***
 * handles the  event:
 * checks if the lable is pressed, if does, returns a signal to indicate it was pressed.
 * Lables are used also to cover buttons, if the lable is located before a button it covers
 * in the screen widgets array, pressing the lable in some point will cause the runSdl function to
 * break and ignore the button located in the same point.
 *
 * @param src - Pointer to the Widget
 * @param e - the event to handle
 * @param screens - Pointer to the array containing the used screens for the game
 * @param game - Pointer to the chess game struct
 * @param screenIndex - the Index of the screen containing the source widget
 * @param widgetIndex - the Index of the widget within the screen widget array
 *
 * @return 0 if the lable wasn't pressed (define: NONE).
 * 		   1 if game is NULL (define: QUIT).
 * 		   otherwise, returns a signal to indicate it was pressed (define: PRESSED)
 */
int handleLableEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);

/***
 * draws the lable in the given renderer.
 * @param src - Pointer to the Widget
 * @param renderer - the renderer to draw in
 */
void drawLable(Widget*, SDL_Renderer*);

/***
 * Create a new widget containing a sticker, with the given parameters
 * @param renderer - renderer for the widget to associate.
 * @param image - a path to the relevant image.
 * @param leftAction - a function for the sticker to call for if left clicked.
 * @param rightAction - a function for the sticker to call for if right clicked.
 * @param x - the x coordinate for the sticker to be placed within the renderer.
 * @param y - the y coordinate for the sticker to be placed within the renderer.
 * @param w - the width of the sticker.
 * @param h - the height of the sticker.
 * @param shown - indicates whether the button is shown or not when initialized (using the defines SHOWN/HIDE).
 *
 * @return NULL if an allocation error occurred.
 *         a pointer to the created widget, otherwise
 */
Widget* createSticker(
	SDL_Renderer* renderer,
	const char* image,
	int (*leftAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game),
	int (*rightAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game),
	int x, int y, int w, int h,int shown);


/***
 * Destroy's sticker, freeing all used memory
 * @param src - Pointer to Widget containing the lable to destroy
 */
void destroySticker(Widget* src);

/***
 * handles the sticker event:
 * checks if the sticker is pressed, if does, calls the function within the sticker struct,
 * corresponding to the mouse button
 * @param src - Pointer to the Widget
 * @param e - the event to handle
 * @param screens - Pointer to the array containing the used screens for the game
 * @param game - Pointer to the chess game struct
 * @param screenIndex - the Index of the screen containing the source widget
 * @param widgetIndex - the Index of the widget within the screen widgets array
 *
 * @return NONE if the button wasn't pressed.
 * 		   otherwise, calls the button's action function and returns its value
 */
int handleStickerEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);

/***
 * draws the sticker in the given renderer.
 * @param src - Pointer to the Widget
 * @param renderer - the renderer to draw in
 */
void drawSticker(Widget* src, SDL_Renderer* rend);



#endif /* BUTTON_H_ */
