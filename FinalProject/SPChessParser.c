//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessParser.h"
#include <string.h>
#include <stdlib.h>

#define DELIMITERS " \t\r\n"
#define GAME_MODE "game_mode"
#define DIFFICULTY "difficulty"
#define USER_COLOR "user_color"
#define LOAD "load"
#define DEFAULT "default"
#define PRINT_SETTINGS "print_settings"
#define QUIT "quit"
#define START "start"
#define MOVE "move"
#define TO "to"
#define GET_MOVES "get_moves"
#define SAVE "save"
#define UNDO "undo"
#define RESET "reset"

SPCommand spParserParseLine(const char* str) {
    SPCommand cmd; // no malloc! woot!
    int notMatched = 1;
    char line[SP_MAX_LINE_LENGTH + 1]; // prepare space for line
    strcpy(line, str);
    char* command = strtok(line, DELIMITERS); // first token separated by whitespace
    if (!command) {
        cmd.cmd = SP_INVALID_LINE;
        cmd.arguments[0] = '\0';
    }
    char* nextToken = strtok(NULL, DELIMITERS);
    if (notMatched && // haven't matched yet
        !strcmp(command, GAME_MODE) && // command is "game_mode"
        nextToken) { // and has an argument
        cmd.cmd = SP_GAME_MODE;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0; // we matched!
    }
    if (notMatched && // no match yet
        !strcmp(command, DIFFICULTY) && // command is "difficulty"
        nextToken) { // and has an argument
        cmd.cmd = SP_DIFFICULTY;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0; // matched!
    }
    if (notMatched && // still no match
        !strcmp(command, USER_COLOR) && // command is "user_color"
        nextToken) { // there's an argument
        cmd.cmd = SP_USER_COLOR;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0; // matched
    }
    if (notMatched && // no match
        !strcmp(command, LOAD) && // command is "laod"
        nextToken) { // argumenttttt
        cmd.cmd = SP_LOAD;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched && // no match yet
        !strcmp(command, DEFAULT)) { // command is "restart_game"
        cmd.cmd = SP_RESTART;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, PRINT_SETTINGS)) {
        cmd.cmd = SP_PRINT_SETTINGS;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, QUIT)) {
        cmd.cmd = SP_QUIT;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, START)) {
        cmd.cmd = SP_START;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, PRINT_SETTINGS)) {
        cmd.cmd = SP_PRINT_SETTINGS;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, MOVE) &&
        nextToken) {
        strcpy(cmd.arguments, nextToken);
        int offset = strlen(nextToken) + 1;
        nextToken = strtok(NULL, DELIMITERS);
        if (nextToken &&
            strcmp(nextToken, TO) &&
            nextToken = strtok(NULL, DELIMITERS)) {
            cmd.cmd = SP_MOVE;
            strcpy(cmd.arguments + offset, nextToken);
            notMatched = 0;
        }
    }
    if (notMatched &&
        !strcmp(command, GET_MOVES) &&
        nextToken) {
        cmd.cmd = SP_GET_MOVES;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, SAVE) &&
        nextToken) {
        cmd.cmd = SP_SAVE;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, UNDO)) {
        cmd.cmd = SP_GET_MOVES;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched &&
        !strcmp(command, RESET)) {
        cmd.cmd = SP_RESET;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }
    if (notMatched) { // got here and no match => invalid line
        cmd.cmd = SP_INVALID_LINE;
        cmd.validArg = false;
    }
    return cmd;
}