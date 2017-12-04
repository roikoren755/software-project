/*
 * sp_nim.c
 *
 *  Created on: 29 Nov 2017
 *      Author: Nadav Gasner & Roi Koren
 */
#include <stdio.h>

int win_check(int *arr, int size){
    for (int i = 0; i < size; i++){
        if(arr[i])
            return 0;
    }
    return 1;
}

void play_computer_turn(int *arr, int size) {
	printf("%d %d", arr[0], size);
}
