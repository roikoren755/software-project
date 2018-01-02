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
	char currentChar = *str; // first char in str
	if (currentChar != '-' && (currentChar < '0' || currentChar > '9')) {
		return false; // should be either negative sign or digit
	}
	str++;
	while (*str) { // while current char isn't '\0'
		if (*str < '0' || *str > '9') {
			return false; // verify char is digit
		}
		str++; // next char
	}
	return true;
}

SPCommand spParserPraseLine(const char* str) {
	SPCommand cmd; // no malloc! woot!
	int notMatched = 1;
	char line[MAXIMUM_COMMAND_LENGTH + 1]; // prepare space for line
	strcpy(line, str);
	char* command = strtok(line, DELIMITERS); // first token separated by whitespace
	while (command && !*command) { // find first token to actually contain data, if one exists
		command = strtok(NULL, DELIMITERS);
	}
	if (!command) {
		cmd.cmd = SP_INVALID_LINE;
		cmd.validArg = false;
		notMatched = 0;
	}
	char* integer = strtok(NULL, DELIMITERS); // next token should be integer for add_disc
	char* remainder = strtok(NULL, DELIMITERS);
	if (notMatched && // haven't matched yet
			!strcmp(command, UNDO_MOVE) && // command is "undo_move"
			!integer) { // nothing after command
		cmd.cmd = SP_UNDO_MOVE;
		cmd.validArg = false;
		notMatched = 0; // we matched!
	}
	if (notMatched && // no match yet
			!strcmp(command, ADD_DISC) && // command is add_disc
			!remainder) { // only two non-empty tokens in command
		cmd.cmd = SP_ADD_DISC;
		if (!integer || !spParserIsInt(integer)) { // if next token isn't valid integer
			cmd.validArg = false;
		}
		else {
			cmd.validArg = true;
			cmd.arg = atoi(integer);
		}
		notMatched = 0; // matched!
	}
	if (notMatched && // still no match
			!strcmp(command, SUGGEST_MOVE) && // command is "sugggest_move"
			!integer) { // nothing after command
		cmd.cmd = SP_SUGGEST_MOVE;
		cmd.validArg = false;
		notMatched = 0; // matched
	}
	if (notMatched && // no match
			!strcmp(command, QUIT) && // command is "quit"
			!integer) { // nothing after command
		cmd.cmd = SP_QUIT;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched && // no match yet
			!strcmp(command, RESTART_GAME) && // command is "restart_game"
			!integer) { // nothing after command
		cmd.cmd = SP_RESTART;
		cmd.validArg = false;
		notMatched = 0;
	}
	if (notMatched) { // got here and no match => invalid line
		cmd.cmd = SP_INVALID_LINE;
		cmd.validArg = false;
	}
	return cmd;
}
