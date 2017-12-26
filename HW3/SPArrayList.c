/*
 * SPArrayList.c
 *
 *  Created on: 18 Dec 2017
 *      Author: user
 */
#include "SPArrayList.h"
#include <string.h>
#include <stdlib.h>

SPArrayList* spArrayListCreate(int maxSize) {
	if (maxSize <= 0) {
		return NULL;
	}
	SPArrayList* list = malloc(sizeof(SPArrayList));
	if (!list){
		return NULL;
	}
	list->actualSize = 0;
	list->maxSize = maxSize;
	list->elements = malloc(maxSize * sizeof(int));
	if (!list->elements) {
		free(list);
		return NULL;
	}
	return list;
}

SPArrayList* spArrayListCopy(SPArrayList* src) {
	if (!src) {
		return NULL;
	}
	SPArrayList* list = malloc(sizeof(SPArrayList));
	if (!list) {
		return NULL;
	}
	list->maxSize = src->maxSize;
	list->actualSize = src->actualSize;
	list->elements = malloc(list->maxSize * sizeof(int));
	if (!list->elements) {
		free(list);
		return NULL;
	}
	for (int i = 0; i < list->maxSize; i++) {
		list->elements[i] = src->elements[i];
	}
	return list;
}

void spArrayListDestroy(SPArrayList* src) {
	if (!src) {
		return;
	}
	free(src->elements);
	free(src);
	return;
}

SP_ARRAY_LIST_MESSAGE spArrayListClear(SPArrayList* src) {
	if (!src) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	src->actualSize = 0;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddAt(SPArrayList* src, int elem, int index) {
	if (!src || index > src->actualSize || index < 0) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->maxSize == src->actualSize) {
		return SP_ARRAY_LIST_FULL;
	}

	for (int i = src->actualSize; i > index; i--) {
		src->elements[i] = src->elements[i - 1];
	}
	src->elements[index] = elem;
	src->actualSize++;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddFirst(SPArrayList* src, int elem){
	if (!src) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->maxSize == src->actualSize) {
		return SP_ARRAY_LIST_FULL;
	}
	for (int i = src->actualSize; i > 0; i--) {
		src->elements[i] = src->elements[i - 1];
	}
	src->elements[0] = elem;
	src->actualSize++;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddLast(SPArrayList* src, int elem){
	if (!src) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->maxSize == src->actualSize) {
		return SP_ARRAY_LIST_FULL;
	}
	src->elements[src->actualSize] = elem;
	src->actualSize++;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveAt(SPArrayList* src, int index){
	if (!src || index >= src->actualSize || index < 0) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (!src->actualSize) {
		return SP_ARRAY_LIST_EMPTY;
	}

	for (int i = index; i < src->actualSize; i++) {
		src->elements[i] = src->elements[i + 1];
	}
	src->actualSize--;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveFirst(SPArrayList* src){
	if (!src) {
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (!src->actualSize) {
		return SP_ARRAY_LIST_EMPTY;
	}
	for (int i = 0; i < src->actualSize - 1; i++) {
		src->elements[i] = src->elements[i + 1];
	}
	src->actualSize--;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveLast(SPArrayList* src){
	if (!src) {
			return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (!src->actualSize) {
		return SP_ARRAY_LIST_EMPTY;
	}

	src->actualSize--;
	return SP_ARRAY_LIST_SUCCESS;
}

int spArrayListGetAt(SPArrayList* src, int index){
	if (!src || index >= src->actualSize || index < 0) {
		return 0;
	}
	return src->elements[index];
}

int spArrayListGetFirst(SPArrayList* src){
	if (!src || !src->actualSize) {
		return 0;
	}
	return src->elements[0];
}

int spArrayListGetLast(SPArrayList* src){
	if (!src || !src->actualSize) {
			return 0;
	}
	return src->elements[src->actualSize-1];
}

int spArrayListMaxCapacity(SPArrayList* src){
	if (!src) {
		return 0;
	}
	return src->maxSize;
}

int spArrayListSize(SPArrayList* src){
	if (!src) {
		return 0;
	}
	return src->actualSize;
}

bool spArrayListIsFull(SPArrayList* src){
	if (!src) {
		return false;
	}
	return src->actualSize == src->maxSize;
}

bool spArrayListIsEmpty(SPArrayList* src){
	if (!src) {
		return false;
	}
	return !src->actualSize;
}



