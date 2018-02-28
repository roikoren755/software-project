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
    SPCommand cmd;
    int notMatched = 1;
    char line[SP_MAX_LINE_LENGTH + 1];
    strcpy(line, str);
    char* command = strtok(line, DELIMITERS);
    if (!command) {
        cmd.cmd = SP_INVALID_LINE;
        cmd.arguments[0] = '\0';
    }
    char* nextToken = strtok(NULL, DELIMITERS);
    if (notMatched &&
            !strcmp(command, GAME_MODE) &&
            nextToken) {
        cmd.cmd = SP_GAME_MODE;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
            !strcmp(command, DIFFICULTY) &&
            nextToken) {
        cmd.cmd = SP_DIFFICULTY;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
            !strcmp(command, USER_COLOR) &&
            nextToken) {
        cmd.cmd = SP_USER_COLOR;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
            !strcmp(command, LOAD) &&
            nextToken) {
        cmd.cmd = SP_LOAD;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }
    if (notMatched &&
            !strcmp(command, DEFAULT)) {
        cmd.cmd = SP_DEFAULT;
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
        char* nextNextToken = strtok(NULL, DELIMITERS);
        if (nextToken &&
                    strcmp(nextToken, TO) &&
                    nextNextToken) {
            cmd.cmd = SP_MOVE;
            strcpy(cmd.arguments + offset, nextNextToken);
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
    if (notMatched) {
        cmd.cmd = SP_INVALID_LINE;
        cmd.arguments[0] = '\0';
    }
    return cmd;
}

int spParserGetPositiveInt(const SPCommand* command) {
    if (!command) {
        return -1;
    }
    int result = 0;
    int i = 0;
    while (command->arguments[i]) {
        if (command->arguments[i] >= '0' && command->arguments[i] <= '9') {
            result *= 10;
            result += command->arguments[i] - '0';
            i++;
        }
        else {
            return -1;
        }
    }
    return result;
}

void spParserGetBoardLocationFromString(char* str, int* location) {
    if (str[0] == '<' && str[2] == ',' && str[4] == '>' &&
            str[1] >= 'A' && str[1] <= 'H' &&
            str[3] >= '1' && str[3] <= '8') {
        location[0] = str[1] - 'A';
        location[1] = str[3] - '1';
    }
    else {
        location[0] = location[1] = -1;
    }
}

void spParserGetLocationForGetMoves(SPCommand* command, int* location) {
    spParserGetBoardLocationFromString(command->arguments, location);
}

void spParserGetMove(SPCommand* command, int** locations) {
    spParserGetBoardLocationFromString(command->arguments, locations[0]);
    spParserGetBoardLocationFromString(&command->arguments[6], locations[1]);
}
