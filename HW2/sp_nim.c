/*
 * sp_nim.c
 *
 *  Created on: 29 Nov 2017
 *      Author: Nadav Gasner & Roi Koren
 */
#include <stdio.h>

int win_check(int *arr, int size){
	// Go through all heaps, if any isn't empty, return 0.
    for (int i = 0; i < size; i++){
        if(arr[i])
            return 0;
    }
    // If all are empty, return 1
    return 1;
}

void play_computer_turn(int *arr, int size) {
	// Calculate nim-sum
	int s_nim = 0;
	for (int i = 0; i < size; i++) {
		s_nim ^= arr[i];
	}
	// If nim-sum isn't 0, search for first winning heap, otherwise for first not-empty one
	int heap = 0;
	int heap_s_nim = s_nim ^ arr[heap];
	while (s_nim? heap_s_nim >= arr[heap]: arr[heap] == 0) {
		heap++;
		heap_s_nim = s_nim ^ arr[heap];
	}
	// If nim-sum is 0, remove 1, otherwise remove as per tactics
	int amount = s_nim? arr[heap] - heap_s_nim: 1;
	arr[heap] -= amount;
	printf("Computer takes %d objects from heap %d.\n", amount, heap + 1);
}
