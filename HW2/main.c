/*

 * main.c
 *
 *  Created on: 28 Nov 2017
 *      Author: Nadav Gasner & Roi Koren
 */
#include <stdio.h>
#include "SPBufferset.h"
#include "sp_nim.h"
#include "main_aux.h"

#define MAX_NUM_OF_HEAPS 32

int main() {
	SP_BUFF_SET();

	int num_of_heaps;
	int game[MAX_NUM_OF_HEAPS];
    int computers_turn = 1;
    int turn = 1;

	printf("Enter the number of heaps:\n");
    scanf("%d", &num_of_heaps);
    if (num_of_heaps < 1 || num_of_heaps > 32) {
        printf("Error: the number of heaps must be between 1 and 32.\n");
        return 0;
    }

    printf("Enter the heap sizes:\n");
    for (int i = 0; i < num_of_heaps; i++) {
        scanf("%d", &game[i]);
        if (game[i] <= 0) {
            printf("Error: the size of heap %d should be positive.\n", i);
            return 0;
        }
    }

    while (!win_check(game, num_of_heaps)) {
        print_heap_states(game, num_of_heaps, turn);
        computers_turn? play_computer_turn(game, num_of_heaps): play_user_turn(game, num_of_heaps);
        computers_turn = !computers_turn;
        turn++;
    }
    computers_turn? printf("Computer wins!\n"): printf("You win!\n");
    return 0;
}




