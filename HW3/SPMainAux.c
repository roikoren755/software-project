//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include "SPFiarGame.h"

int spGetDifficulty() {
    printf("Please enter the difficulty level between [1-7]:\n");
    char maxDepth[10];
    int result = scanf("%9s", maxDepth);
    while (result == EOF || result == 0 || maxDepth[0] < '1' || maxDepth[0] > '7' || maxDepth[1]!='\0') {
        printf("Error: invalid level (should be between 1 to 7)\n");
        printf("Please enter the difficulty level between [1-7]:\n");
        result = scanf("%9s", maxDepth);

    }
    getc(stdin);
    int x = maxDepth[0] - '0';
    return x;
}

int spFiarGameGetLastMovePlayed(SPFiarGame* src) {
	if (!src || spArrayListIsEmpty(src->history)) {
		return -1;
	}
	return spArrayListGetFirst(src->history);
}
