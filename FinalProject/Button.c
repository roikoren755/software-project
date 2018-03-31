#include "Button.h"


Widget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	int (*action)(Screen** screen ,SPChessGame* game,int j),
	int x, int y, int w, int h,int shown)
{
	// allocate data
	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL)
		return NULL;

	Button* data = (Button*) malloc(sizeof(Button));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		free(res);
		free(data);
		return NULL;
	}
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));


	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		free(res);
		free(data);
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	res->destroy = destroyButton;
	res->draw = drawButton;
	res->handleEvent = handleButtonEvent;
	res->data = data;
	res->shown =  shown;

	return res;
}

void destroyButton(Widget* src)
{
	Button* button = (Button*) src->data;
	SDL_DestroyTexture(button->texture);
	free(button);
	free(src);
}

int handleButtonEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int j)
{
	if (e->type == SDL_MOUSEBUTTONUP) {
		Button* button = (Button*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &button->location)) {
			printf("button pressed at <%d,%d>\n",button->location.x,button->location.y);
			SDL_Delay(10); //TODO
			return button->action(screens,game, j);
		}
	}
	return 0;
}

void drawButton(Widget* src, SDL_Renderer* render)
{
	Button* button = (Button*) src->data;
	SDL_RenderCopy(render, button->texture, NULL, &button->location);
}

Widget* createLable(
	SDL_Renderer* renderer,
	const char* image,
	int x, int y, int w, int h,int shown){
	// allocate data
	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL)
		return NULL;

	Lable* data = (Lable*) malloc(sizeof(Lable));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		free(res);
		free(data);
		return NULL;
	}
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));


	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		free(res);
		free(data);
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	// store button & widget details
	data->texture = texture;
	data->location = location;
	res->destroy = destroyLable;
	res->draw = drawLable;
	res->handleEvent = handleLableEvent;
	res->data = data;
	res->shown = shown;

	return res;
}

void destroyLable(Widget* src){
	Lable* lable = (Lable*) src->data;
	SDL_DestroyTexture(lable->texture);
	free(lable);
	free(src);
}
int handleLableEvent(Widget* src, SDL_Event* e,Screen** screen, SPChessGame* game,int j){
	return 0;
}
void drawLable(Widget* src, SDL_Renderer* rend){
	Lable* lable = (Lable*) src->data;
	SDL_RenderCopy(rend, lable->texture, NULL, &lable->location);
}

Widget* createSticker(
	SDL_Renderer* renderer,
	const char* image,
	int (*leftAction)(Widget* src, SDL_Event* e,Screen** screens, SPChessGame* game),
	int (*rightAction)(Widget* src, SDL_Event* e,Screen** screens, SPChessGame* game),
	int x, int y, int w, int h,int shown){
	// allocate data
		Widget* res = (Widget*) malloc(sizeof(Widget));
		if (res == NULL)
			return NULL;

		Sticker* data = (Sticker*) malloc(sizeof(Sticker));
		if (data == NULL) {
			free(res);
			return NULL;
		}

		SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

		// we use the surface as a temp var
		SDL_Surface* surface = SDL_LoadBMP(image);
		if (surface == NULL) {
			free(res);
			free(data);
			return NULL;
		}
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));


		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL) {
			free(res);
			free(data);
			SDL_FreeSurface(surface);
			return NULL ;
		}

		// surface is not needed after texture is created
		SDL_FreeSurface(surface);

		// store button & widget details
		data->texture = texture;
		data->location = location;
		data->leftAction = leftAction;
		data->rightAction = rightAction;
		res->destroy = destroySticker;
		res->draw = drawSticker;
		res->handleEvent = handleStickerEvent;
		res->data = data;
		res->shown = shown;
		return res;

}
void destroySticker(Widget* src){
	Sticker* piece = (Sticker*) src->data;
	SDL_DestroyTexture(piece->texture);
	free(piece);
	free(src);
}
int handleStickerEvent(Widget* src, SDL_Event* e,Screen** screens, SPChessGame* game,int j){
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		Sticker* sticker = (Sticker*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &sticker->location)) {
			if(e->button.button == SDL_BUTTON_LEFT){
				return sticker->leftAction(src,e,screens,game);
			}
			if(e->button.button == SDL_BUTTON_RIGHT){
				return sticker->rightAction(src,e,screens,game);
			}
		}
	}
	return 0;
}
void drawSticker(Widget* src, SDL_Renderer* rend){
	Sticker* sticker = (Sticker*) src->data;
	SDL_RenderCopy(rend, sticker->texture, NULL, &sticker->location);
}
