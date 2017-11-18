/*
 * main.c
 *
 *  Created on: Nov 13, 2017
 *      Author: Roi
 */


#include <stdio.h>
#include <ctype.h>

int toDecimalFromInput(int base);

int main() {
	int fromBase = 0, toBase = 0, scanned = 0;
	printf("Please enter the number's base:\n");
	scanned = scanf("%d", &fromBase);
	if (scanned != 1) {
		printf("An error occurred!\n");
		return 0;
	}
	if (fromBase < 2 || fromBase > 16) {
		printf("Invalid input base\n");
		return 0;
	}
	printf("Please enter the desired base:\n");
	scanned = scanf("%d", &toBase);
	if (scanned != 1) {
		printf("An error occurred!\n");
		return 0;
	}
	if (toBase < 2 || toBase > 16) {
		printf("Invalid desired base\n");
		return 0;
	}
	printf("Please enter a number in base %d:\n", fromBase);
	int toNum = toDecimalFromInput(fromBase);
	if (toNum == -1) {
		return 0;
	}
}

int toDecimalFromInput(int base) {
	char input[1000];
	int res = 0;
	scanf("%s", input);
	for (int i = 0; i < 1000 && input[i] != '\0' && !isspace(input[i]); i++) {
		char currentDigit = input[i];
		int digitVal = currentDigit - '0';
		int letterVal = currentDigit - 'A' + 10;
		if ((digitVal >= 0 && digitVal <= 9 && digitVal >= base) ||
				(letterVal >= 10 && letterVal <= 15 && letterVal >= base) ||
				currentDigit < '0' ||
				(currentDigit > '9' && currentDigit < 'A') ||
				currentDigit > 'F') {
			printf("Invalid number!\n");
			return -1;
		}
		else {
			res *= base;
			res += (digitVal <= base)? digitVal: letterVal;
		}
	}
	return res;
}

void toNewBase(int num, int base) {

}
