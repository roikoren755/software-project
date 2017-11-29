/*

 * main.c
 *
 *  Created on: 28 áðåá 2017
 *      Author: user
 */
#include<stdio.h>
#include "SPBufferset.h"
#include "sp_nim.h"

#define N 32

int main() {
	SP_BUFF_SET();

	int g_size;
	int game[N];

	printf("Enter the number of heaps:\n");
    scanf("%d",&g_size);
    if(g_size<1 || g_size>32){
    	printf("Error: the number of heaps must be between 1 and 32.\n");
    	return 0;
    	}

    printf("Enter the heap sizes:\n");
    for(int i=0;i<g_size;i++){
        scanf("%d",&game[i]);
    	}
    for(int i=0;i<g_size;i++){
    	printf("%d ",game[i]);
    	}
}




