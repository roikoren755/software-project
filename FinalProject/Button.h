#ifndef BUTTON_H_
#define BUTTON_H_

#include "SPChessGame.h"
#include "Widget.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct screen_t Screen;
typedef struct button_t Button;
struct button_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	int (*action)(Screen** screen ,SPChessGame* game,int screenIndex ,int widgetIndex);
};

typedef struct label_t Lable;
struct label_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
};

typedef struct sticker_t Sticker;
struct sticker_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	int (*leftAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game);
	int (*rightAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game);
};

Widget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	int (*action)(Screen** screen ,SPChessGame* game,int screenIndex ,int widgetIndex),
	int x, int y, int w, int h,int shown);
void destroyButton(Widget* src);
int handleButtonEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);
void drawButton(Widget*, SDL_Renderer*);

Widget* createLable(
	SDL_Renderer* renderer,
	const char* image,
	int x, int y, int w, int h,int shown);
void destroyLable(Widget* src);
int handleLableEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);
void drawLable(Widget*, SDL_Renderer*);

Widget* createSticker(
	SDL_Renderer* renderer,
	const char* image,
	int (*leftAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game),
	int (*rightAction)(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game),
	int x, int y, int w, int h,int shown);
void destroySticker(Widget* src);
int handleStickerEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex);
void drawSticker(Widget* src, SDL_Renderer* rend);



#endif /* BUTTON_H_ */
