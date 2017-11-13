/*
 * main.c
 *
 *  Created on: Nov 13, 2017
 *      Author: Roi
 */


#include <stdio.h>

int main() {
	int fromBase = 0, toBase = 0;
	printf("Please enter the number's base:\n");
	scanf("%d", &fromBase);
	if (fromBase < 2 || fromBase > 16) {
		printf("Invalid input base\n");
		return 0;
	}
	printf("Please enter the desired base:\n");
	scanf("%d", &toBase);
	if (toBase < 2 || toBase > 16) {
		printf("Invalid desired base\n");
		return 0;
	}
	int toNum = toDecimalFromInput(fromBase);
}

int toDecimalFromInput(int base) {
	return 0;
}

void toNewBase(int num, int base) {

}
