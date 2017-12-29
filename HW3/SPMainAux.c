//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>

int spGetDifficulty() {
    printf("Please enter the difficulty level between [1-7]:\n");
    int maxDepth = 0;
    int result = scanf("%d", &maxDepth);
    while (result == EOF || result == 0 || maxDepth < 1 || maxDepth > 7) {
        printf("Error: invalid level (should be between 1 to 7)\n");
        printf("Please enter the difficulty level between [1-7]:\n");
        result = scanf("%d", &maxDepth);
    }
    getc(stdin);
    return maxDepth;
}
