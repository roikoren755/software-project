/*
 * SPArrayList.c
 *
 *  Created on: 18 בדצמ 2017
 *      Author: user
 */
#include "SPArrayList.h"
#include <string.h>
#include <malloc.h>


SPArrayList* spArrayListCreate(int maxSize){

	SPArrayList* list = malloc(sizeof(SPArrayList));
	if (list ==0||maxSize<=0){
		return NULL;
	}
	list->maxSize = maxSize;
	list->elements = malloc(maxSize*sizeof(int));
	return list;
}

SPArrayList* spArrayListCopy(SPArrayList* src){
	SPArrayList* list = malloc(sizeof(SPArrayList));
		if (list ==0||src==NULL){
			return NULL;
		}
	list->maxSize = src->maxSize;
	list->actualSize = src->actualSize;
	list->elements = malloc(list->maxSize*sizeof(int));
	for (int i = 0; i < src->maxSize; i++){
		list->elements[i] = src->elements[i];
	}
	return list;
}

void spArrayListDestroy(SPArrayList* src){
	free(src->elements);
	free(src);
}

SP_ARRAY_LIST_MESSAGE spArrayListClear(SPArrayList* src){
	if (src == NULL){
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	src->actualSize = 0;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddAt(SPArrayList* src, int elem, int index){
	if (src==NULL||index>src->actualSize||index<0){
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->maxSize==src->actualSize){
		return SP_ARRAY_LIST_FULL;
	}

	for (int i = src->actualSize; i > index; i--){
		src->elements[i] = src->elements[i-1];
	}
	src->elements[index] = elem;
	src->actualSize += 1;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddFirst(SPArrayList* src, int elem){
		if (src==NULL){
			return SP_ARRAY_LIST_INVALID_ARGUMENT;
		}
		if (src->maxSize==src->actualSize){
			return SP_ARRAY_LIST_FULL;
		}

		for (int i = src->actualSize; i > 0; i--){
			src->elements[i] = src->elements[i-1];
		}
		src->elements[0] = elem;
		src->actualSize += 1;
		return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddLast(SPArrayList* src, int elem){
		if (src==NULL){
			return SP_ARRAY_LIST_INVALID_ARGUMENT;
		}
		if (src->maxSize==src->actualSize){
			return SP_ARRAY_LIST_FULL;
		}

		src->elements[src->actualSize] = elem;
		src->actualSize += 1;
		return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveAt(SPArrayList* src, int index){
	if (src==NULL||index >= src->actualSize||index < 0){
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->actualSize == 0){
		return SP_ARRAY_LIST_EMPTY;
	}

	for (int i = index; i < src->actualSize; i++){
		src->elements[i] = src->elements[i+1];
	}
	src->actualSize -= 1;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveFirst(SPArrayList* src){
	if (src==NULL){
			return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->actualSize == 0){
		return SP_ARRAY_LIST_EMPTY;
	}

	for (int i = 0; i < src->actualSize; i++){
		src->elements[i] = src->elements[i+1];
	}
	src->actualSize -= 1;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveLast(SPArrayList* src){
	if (src==NULL){
			return SP_ARRAY_LIST_INVALID_ARGUMENT;
	}
	if (src->actualSize == 0){
		return SP_ARRAY_LIST_EMPTY;
	}

	src->actualSize -= 1;
	return SP_ARRAY_LIST_SUCCESS;
}

int spArrayListGetAt(SPArrayList* src, int index){
	if (src==NULL||index >= src->actualSize||index < 0){
		return NULL;
	}
	return src->elements[index];
}

int spArrayListGetFirst(SPArrayList* src){
	if (src==NULL||0 >= src->actualSize){
		return NULL;
	}
	return src->elements[0];
}

int spArrayListGetLast(SPArrayList* src){
	if (src==NULL||0 >= src->actualSize){
			return NULL;
	}
	return src->elements[src->actualSize-1];
}

int spArrayListMaxCapacity(SPArrayList* src){
	if (src==NULL){
			return NULL;
		}
	return src->maxSize;
}

int spArrayListSize(SPArrayList* src){
	if (src==NULL){
				return NULL;
			}
	return src->actualSize;
}

bool spArrayListIsFull(SPArrayList* src){
	if (src==NULL||src->actualSize != src->maxSize){
		return false;
	}
	return true;
}

bool spArrayListIsEmpty(SPArrayList* src){
	if (src==NULL||src->actualSize != 0){
		return false;
	}
	return true;
}



