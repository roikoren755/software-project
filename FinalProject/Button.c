#include "Button.h"

/**
 * creates a texture from an image's given path, and relates it to a renderer.
*  @param image - a path to an image.
*  @param renderer - a pointer to a renderer
 * @return NULL if an error occurred.
 *         a pointer to the created texture, otherwise
 */
SDL_Texture* createTexture(const char* image,SDL_Renderer* renderer){

	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		printf("ERROR: unable to load image: %s\n", SDL_GetError());
		return NULL;
	}
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));


	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	return texture;
}

Widget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	int (*action)(Screen** screen ,SPChessGame* game, int screenIndex ,int widgetIndex),
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

	SDL_Texture* texture = createTexture(image,renderer);
	if (texture == NULL) {
		free(res);
		free(data);
		return NULL ;
	}

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	res->destroy = destroyButton;
	res->draw = drawButton;
	res->handleEvent = handleButtonEvent;
	res->data = (void *)data;
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
		SPChessGame* game,int screenIndex ,int widgetIndex)
{
	if(!screens[screenIndex]->widgets[widgetIndex]->shown){
		return NONE;
	}
	if (e->type == SDL_MOUSEBUTTONUP) {  //check if button was pressed
		Button* button = (Button*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &button->location)) {
			return button->action(screens,game,  screenIndex , widgetIndex);
		}
	}
	return NONE;
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

	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL)
		return NULL;

	Lable* data = (Lable*) malloc(sizeof(Lable));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	SDL_Texture* texture = createTexture(image,renderer);
	if (texture == NULL) {
		free(res);
		free(data);
		return NULL ;
	}

	// store button & widget details
	data->texture = texture;
	data->location = location;
	res->destroy = destroyLable;
	res->draw = drawLable;
	res->handleEvent = handleLableEvent;
	res->data = (void *)data;
	res->shown = shown;

	return res;
}

void destroyLable(Widget* src){
	Lable* lable = (Lable*) src->data;
	SDL_DestroyTexture(lable->texture);
	free(lable);
	free(src);
}

int handleLableEvent(Widget* src, SDL_Event* e,Screen** screens, SPChessGame* game,int screenIndex ,int widgetIndex){
	if(!game){  //sanity check
		printf("ERROR, Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error",
							"ERROR, Resources for game were lost. quitting", NULL);
		return QUIT;
	}

	if(!screens[screenIndex]->widgets[widgetIndex]->shown){
		return NONE;
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		Lable* lable = (Lable*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &lable->location)) {
			return PRESSED;
		}
	}
	return NONE;
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

		Widget* res = (Widget*) malloc(sizeof(Widget));
		if (res == NULL)
			return NULL;

		Sticker* data = (Sticker*) malloc(sizeof(Sticker));
		if (data == NULL) {
			free(res);
			return NULL;
		}

		SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

		SDL_Texture* texture = createTexture(image,renderer);
		if (texture == NULL) {
			free(res);
			free(data);
			return NULL ;
		}

		// store button & widget details
		data->texture = texture;
		data->location = location;
		data->leftAction = leftAction;
		data->rightAction = rightAction;
		res->destroy = destroySticker;
		res->draw = drawSticker;
		res->handleEvent = handleStickerEvent;
		res->data = (void *)data;
		res->shown = shown;
		return res;

}
void destroySticker(Widget* src){
	Sticker* piece = (Sticker*) src->data;
	SDL_DestroyTexture(piece->texture);
	free(piece);
	free(src);
}
int handleStickerEvent(Widget* src, SDL_Event* e,Screen** screens,
		SPChessGame* game,int screenIndex ,int widgetIndex){
	if(!screens[screenIndex]->widgets[widgetIndex]->shown){
		return NONE;
	}
	if (e->type == SDL_MOUSEBUTTONDOWN) {   //check if pressed
		Sticker* sticker = (Sticker*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &sticker->location)) {
			if(e->button.button == SDL_BUTTON_LEFT){   //check if right clicked or left clicked
				return sticker->leftAction(src,e,screens,game);
			}
			if(e->button.button == SDL_BUTTON_RIGHT){
				return sticker->rightAction(src,e,screens,game);
			}
		}
	}
	return NONE;
}
void drawSticker(Widget* src, SDL_Renderer* rend){
	Sticker* sticker = (Sticker*) src->data;
	SDL_RenderCopy(rend, sticker->texture, NULL, &sticker->location);
}
