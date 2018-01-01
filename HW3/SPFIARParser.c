#include "SPFIARParser.h"
#include <string.h>
#include <stdlib.h>

#define DELIMITERS " \t\r\n"
#define MAXIMUM_COMMAND_LENGTH 1024
#define UNDO_MOVE "undo_move"
#define ADD_DISC "add_disc"
#define SUGGEST_MOVE "suggest_move"
#define QUIT "quit"
#define RESTART_GAME "restart_game"
#define SP_FIAR_GAME_N_COLUMNS 7

bool spParserIsInt(const char* str) {
	char currentChar = *str;
	if (currentChar != '-' && (currentChar < '0' || currentChar > '9')) {
		return false;
	}
	str++;
	currentChar = *str;
	while (currentChar) {
		if (currentChar < '0' || currentChar > '9') {
			return false;
		}
		str++;
		currentChar = *str;
	}
	return true;
}

SPCommand spParserPraseLine(const char* str) {
	SPCommand cmd;
	int notMatched = 1;
	char line[MAXIMUM_COMMAND_LENGTH + 1];
	strcpy(line, str);
	char* command = strtok(line, DELIMITERS);
	while (command && !*command) {
		command = strtok(NULL, DELIMITERS);
	}
	char* integer = strtok(NULL, DELIMITERS);
	if (notMatched && !strcmp(command, UNDO_MOVE) && !integer) {
		cmd.cmd = SP_UNDO_MOVE;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched && !strcmp(command, ADD_DISC)) {
		cmd.cmd = SP_ADD_DISC;
		if (!integer || !spParserIsInt(integer)) {
			cmd.validArg = false;
		}
		else {
			cmd.validArg = true;
			cmd.arg = atoi(integer);
		}
		notMatched = 0;
	}
	if (notMatched && !strcmp(command, SUGGEST_MOVE) && !integer) {
		cmd.cmd = SP_SUGGEST_MOVE;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched && !strcmp(command, QUIT) && !integer) {
		cmd.cmd = SP_QUIT;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched && !strcmp(command, RESTART_GAME) && !integer) {
		cmd.cmd = SP_RESTART;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched) {
		cmd.cmd = SP_INVALID_LINE;
		cmd.validArg = false;
	}
	return cmd;
}
