/*
 * main.c
 *
 *  Created on: Nov 13, 2017
 *      Author: Roi
 */


#include <stdio.h>
#include <ctype.h>

int toDecimalFromInput(int base);
void toNewBase(int num, int base);
char A = 'A';
char F = 'F';
char zero = '0';
char nine = '9';
char endOfString = '\0';
int inputOutputLimit = 100;
int minBase = 2;
int maxBase = 16;

int main() {
	int fromBase = 0, toBase = 0, scanned = 0;
	printf("Please enter the number's base:\n");
	scanned = scanf("%d", &fromBase);
	if (scanned != 1) {
		printf("An error occurred!\n");
		return 0;
	}
	if (fromBase < minBase || fromBase > maxBase) {
		printf("Invalid input base\n");
		return 0;
	}
	printf("Please enter the desired base:\n");
	scanned = scanf("%d", &toBase);
	if (scanned != 1) {
		printf("An error occurred!\n");
		return 0;
	}
	if (toBase < minBase || toBase > maxBase) {
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
	char input[inputOutputLimit];
	int res = 0;
	scanf("%s", input);
	for (int i = 0; i < inputOutputLimit && input[i] != endOfString && !isspace(input[i]); i++) {
		char currentDigit = input[i];
		int digitVal = currentDigit - zero;
		int letterVal = currentDigit - A + 10;
		if ((digitVal >= 0 && digitVal <= 9 && digitVal >= base) ||
				(letterVal >= 10 && letterVal <= 15 && letterVal >= base) ||
				currentDigit < zero ||
				(currentDigit > nine && currentDigit < A) ||
				currentDigit > F) {
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
	char res[102];
	int len = 0;
	int num0  = num;
	int curDigit;
	while(num0){
    	curDigit = num0%base;
    	num0 = num0/base;
	   // printf("is: %d,%d\n",curDigit,num0);
        if(curDigit>9){
            res[len]='A'+curDigit-10;}
        else{res[len]='0'+curDigit;}
        len++;
    	}
    	for(int i = len-1;i>=0;i--)
    	    {printf("%c",res[i]);}
}
