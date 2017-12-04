/*
 * main_aux.c
 *
 *  Created on: 29 Nov 2017
 *      Author: Nadav Gasner & Roi Koren
 */
#include <stdio.h>

void print_heap_states(int *arr, int size, int turn) {
	printf("In turn %d heap sizes are:", turn);
	for (int i = 0; i < size; i++) {
		printf(" h%d=%d", i + 1, arr[i]);
	}
	printf(".\n");
}

void play_user_turn(int *arr, int size) {
	int heap, amount;
	printf("Your turn: please enter the heap index and the number of removed objects.\n");
	scanf(" %d %d", &heap, &amount);
	while (!heap || heap > size || !amount || amount > arr[heap]) {
		printf("Error: Invalid input.\nPlease enter again the heap index and the number of removed objects.\n");
		scanf(" %d %d", &heap, &amount);
	}
	printf("You take %d objects from heap %d.\n", amount, heap);
	arr[heap] -= amount;
}
