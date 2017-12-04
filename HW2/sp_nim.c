/*
 * sp_nim.c
 *
 *  Created on: 29 Nov 2017
 *      Author: Nadav Gasner & Roi Koren
 */
int win_check(int *arr, int size){
    for (int i = 0; i < size; i++){
        if(!arr[i])
            return 0;
    }
    return 1;
}