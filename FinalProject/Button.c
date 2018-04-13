#include "Button.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * creates a texture from an image's given path, and relates it to a renderer.
*  @param image - a path to an image.
*  @param renderer - a pointer to a renderer
 * @return NULL if an error occurred.
 *         a pointer to the created texture, otherwise
 */
SDL_Texture* createTexture(const char* image, SDL_Renderer* renderer){
	if (!image || !renderer) {
		printf("ERROR: Could not find image or renderer to create texture from\n");
	}

	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		printf("ERROR: unable to load image: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		printf("ERROR: Could not create texture\n");
		SDL_FreeSurface(surface);
		return NULL ;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	return texture;
}

Widget* createButton(SDL_Renderer* renderer, const char* image,
	int (*action)(Screen** screen ,SPChessGame* game, int screenIndex ,int widgetIndex),
	int x, int y, int w, int h, int shown) {
	if (!renderer || !image || !action) {
		printf("ERROR: Could not create button\n");
	}

	// allocate data
	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL) {
		printf("ERROR: Could not create button\n");
		return NULL;
	}

	Button* data = (Button*) malloc(sizeof(Button));
	if (data == NULL) {
		printf("ERROR: Could not create button\n");
		free(res);
		return NULL;
	}

	SDL_Texture* texture = createTexture(image, renderer);
	if (texture == NULL) {
		printf("ERROR: Could not create button\n");
		free(res);
		free(data);
		return NULL ;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	res->destroy = destroyButton;
	res->draw = drawButton;
	res->handleEvent = handleButtonEvent;
	res->data = (void*) data;
	res->shown = shown;

	return res;
}

void destroyButton(Widget* src) {
	if (src) {
		Button* button = (Button*) src->data;
		SDL_DestroyTexture(button->texture);
		free(button);
		free(src);
	}
}

int handleButtonEvent(Widget* src, SDL_Event* e, Screen** screens, SPChessGame* game, int screenIndex,
					  int widgetIndex) {
	if (!src || !e || !game || !screens || !screens[screenIndex] || !screens[screenIndex]->widgets ||
		!screens[screenIndex]->widgets[widgetIndex] || !screens[screenIndex]->widgets[widgetIndex]->shown) {
		printf("ERROR: Something went wrong while handling button event\n");
		return NONE;
	}

	if (e->type == SDL_MOUSEBUTTONUP) {  //check if button was pressed
		Button* button = (Button*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &button->location)) {
			return button->action(screens, game, screenIndex, widgetIndex);
		}
	}

	return NONE;
}

void drawButton(Widget* src, SDL_Renderer* renderer) {
	if (!src || !renderer) {
		printf("ERROR: Could not draw button\n");
		return;
	}

	Button* button = (Button*) src->data;
	SDL_RenderCopy(renderer, button->texture, NULL, &button->location);
}

Widget* createLabel(SDL_Renderer* renderer, const char* image, int x, int y, int w, int h, int shown) {
	if (!renderer || !image) {
		printf("ERROR: Could not create label\n");
		return NULL;
	}

	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL) {
		printf("ERROR: Could not create label\n");
		return NULL;
	}

	Label* data = (Label*) malloc(sizeof(Label));
	if (data == NULL) {
		printf("ERROR: Could not create label\n");
		free(res);
		return NULL;
	}

	SDL_Texture* texture = createTexture(image, renderer);
	if (texture == NULL) {
		printf("ERROR: Could not create label\n");
		free(res);
		free(data);
		return NULL ;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	// store button & widget details
	data->texture = texture;
	data->location = location;
	res->destroy = destroyLabel;
	res->draw = drawLabel;
	res->handleEvent = handleLabelEvent;
	res->data = (void*) data;
	res->shown = shown;

	return res;
}

void destroyLabel(Widget* src) {
	if (src) {
		Label* label = (Label*) src->data;
		SDL_DestroyTexture(label->texture);
		free(label);
		free(src);
	}
}

int handleLabelEvent(Widget* src, SDL_Event* e, Screen** screens, SPChessGame* game, int screenIndex, int widgetIndex) {
	if (!game) {  //sanity check
		printf("ERROR: Resources for game were lost. quitting..\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "ERROR: Resources for game were lost. quitting",
								 NULL);
		return QUIT;
	}

	if (!src || !e || !screens || !screens[screenIndex] || !screens[screenIndex]->widgets ||
	   !screens[screenIndex]->widgets[widgetIndex] || !screens[screenIndex]->widgets[widgetIndex]->shown) {
		printf("ERROR: Could not handle label event\n");
		return NONE;
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		Label* label = (Label*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &label->location)) {
			return PRESSED;
		}
	}

	return NONE;
}

void drawLabel(Widget* src, SDL_Renderer* renderer) {
	if (!src || !renderer) {
		printf("ERROR: Could not draw label\n");
		return;
	}

	Label* label = (Label*) src->data;
	SDL_RenderCopy(renderer, label->texture, NULL, &label->location);
}

Widget* createSticker(SDL_Renderer* renderer, const char* image,
					  int (*leftAction)(Widget* src, SDL_Event* e, Screen** screens, SPChessGame* game),
					  int (*rightAction)(Widget* src, SDL_Event* e, Screen** screens, SPChessGame* game),
					  int x, int y, int w, int h, int shown) {
	if (!renderer || !image || !leftAction || !rightAction) {
		printf("ERROR: Could not create sticker\n");
		return NULL;
	}

	Widget* res = (Widget*) malloc(sizeof(Widget));
	if (res == NULL) {
		printf("ERROR: Could not create sticker\n");
		return NULL;
	}

	Sticker* data = (Sticker*) malloc(sizeof(Sticker));
	if (data == NULL) {
		printf("ERROR: Could not create sticker\n");
		free(res);
		return NULL;
	}

	SDL_Texture* texture = createTexture(image,renderer);
	if (texture == NULL) {
		printf("ERROR: Could not create sticker\n");
		free(res);
		free(data);
		return NULL ;
	}

	SDL_Rect location = { .x = x, .y = y, .w = w, .h = h };

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->leftAction = leftAction;
	data->rightAction = rightAction;
	res->destroy = destroySticker;
	res->draw = drawSticker;
	res->handleEvent = handleStickerEvent;
	res->data = (void*) data;
	res->shown = shown;
	return res;

}

void destroySticker(Widget* src) {
	if (src) {
		Sticker* piece = (Sticker*) src->data;
		SDL_DestroyTexture(piece->texture);
		free(piece);
		free(src);
	}
}

int handleStickerEvent(Widget* src, SDL_Event* e, Screen** screens, SPChessGame* game, int screenIndex,
					   int widgetIndex) {
	if (!src || !e || !game || !screens || !screens[screenIndex] || !screens[screenIndex]->widgets ||
		!screens[screenIndex]->widgets[widgetIndex] || !screens[screenIndex]->widgets[widgetIndex]->shown) {
		return NONE;
	}

	if (e->type == SDL_MOUSEBUTTONDOWN) {   //check if pressed
		Sticker* sticker = (Sticker*) src->data;
		SDL_Point point = { .x = e->button.x, .y = e->button.y };
		if (SDL_PointInRect(&point, &sticker->location)) {
			if(e->button.button == SDL_BUTTON_LEFT) {   //check if right clicked or left clicked
				return sticker->leftAction(src, e, screens, game);
			}

			else if(e->button.button == SDL_BUTTON_RIGHT) {
				return sticker->rightAction(src, e, screens, game);
			}
		}
	}

	return NONE;
}

void drawSticker(Widget* src, SDL_Renderer* renderer){
	if (!src || !renderer) {
		printf("ERROR: Could not draw sticker\n");
		return;
	}
	Sticker* sticker = (Sticker*) src->data;
	SDL_RenderCopy(renderer, sticker->texture, NULL, &sticker->location);
}
