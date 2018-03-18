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

/***
 * Checks if any of the 2 rightmost bytes of an int correspond to a failed parsing attempt of a location
 * @param location
 * @return an integer with all bits as 1 if one of the 2 rightmost bytes aren't a legal location
 *         location, otherwise
 */
int spParserDestroyLocationIfNeeded(int location) {
    int result = ~0; // Preparations
    int mask = 1;

    for (int i = 0; i < 2; i++) { // 2 rightmost bytes
        int destroy = 1;
        for (int j = 0; j < 8; j++) { // For bits in byte
            if (!(location & mask)) { // There's a 0 bit, so don't destroy
                destroy = 0;
            }

            mask <<= 1; // Move to next bit
        }

        if (destroy) { // Destroy?
            return result;
        }
    }

    return location; // No
}

/***
 * Tries parsing str as "<x,y>", a board location for the chess game
 * @param str
 * @return a char with all bits as 0 if str isn't a location argument
 *         a char whose 4 leftmost bits represent x, and 4 rightmost bits represent y
 */
char spParserGetBoardLocationFromString(char* str) {
    char location = 0;

    if (str[0] == '<' && str[2] == ',' && str[4] == '>' && // str is "<x,y>", with x and y as legal row and column
        str[1] >= '1' && str[1] <= '8' &&
        str[3] >= 'A' && str[3] <= 'H') {
        location = 7 - (str[1] - '1'); // Get row
        location <<= 4;
        location |= str[3] - 'H'; // And column
    }
    else { // str isn't!
        location = ~location; // Booo
    }

    return location;
}

SPCommand spParserParseLine(const char* str) {
    SPCommand cmd; // Preparations
    if (!str) { // But waaaaait....
        cmd.cmd = SP_INVALID_LINE;
        cmd.arguments[0] = '\0';
    }
    int notMatched = 1;
    char line[SP_MAX_LINE_LENGTH + 1];
    strcpy(line, str);

    char* command = strtok(line, DELIMITERS); // First white-space-saparated word
    if (!command) { // No such thing?
        cmd.cmd = SP_INVALID_LINE; // INVALID
        cmd.arguments[0] = '\0'; // No arguments
    }

    char* nextToken = strtok(NULL, DELIMITERS); // Next word

    if (notMatched && // No match yet
            !strcmp(command, GAME_MODE) && // First word matches GAME_MODE
            nextToken) { // And there's something after it
        cmd.cmd = SP_GAME_MODE;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Still no match
            !strcmp(command, DIFFICULTY) && // First word matches DIFFICULTY
            nextToken) { // And something's after it
        cmd.cmd = SP_DIFFICULTY;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Nothing yet
            !strcmp(command, USER_COLOR) && // USER_COLOR
            nextToken) { // And then some
        cmd.cmd = SP_USER_COLOR;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Nope
            !strcmp(command, LOAD) && // LOAD
            nextToken) { // And argument
        cmd.cmd = SP_LOAD;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Nothing
            !strcmp(command, DEFAULT)) { // DEFAULT
        cmd.cmd = SP_DEFAULT;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched && // Not yet
            !strcmp(command, PRINT_SETTINGS)) { // PRINT_SETTINGS
        cmd.cmd = SP_PRINT_SETTINGS;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched && // Wait for it
            !strcmp(command, QUIT)) { // QUIT
        cmd.cmd = SP_QUIT;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched && // Almost there!
            !strcmp(command, START)) { // START
        cmd.cmd = SP_START;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched && // Really close now
            !strcmp(command, MOVE) && // MOVE
            nextToken) { // With fries
        strcpy(cmd.arguments, nextToken); // Get first argument
        int offset = strlen(nextToken) + 1; // First string length
        nextToken = strtok(NULL, DELIMITERS); // Get next word
        char* nextNextToken = strtok(NULL, DELIMITERS); // And the argument after it

        if (nextToken && // There is a word
                    strcmp(nextToken, TO) && // And the word is TO
                    nextNextToken) { // And yet another argument!
            cmd.cmd = SP_MOVE;
            strcpy(cmd.arguments + offset, nextNextToken); // Copy second argument, right after the first one
            notMatched = 0;
        }
    }

    if (notMatched && // No
            !strcmp(command, GET_MOVES) && // GET MOVES
            nextToken) { // And from where, to boot!
        cmd.cmd = SP_GET_MOVES;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Just no
            !strcmp(command, SAVE) && // SAVE
            nextToken) {
        cmd.cmd = SP_SAVE;
        strcpy(cmd.arguments, nextToken);
        notMatched = 0;
    }

    if (notMatched && // Please stop asking
            !strcmp(command, UNDO)) { // UNDO
        cmd.cmd = SP_GET_MOVES;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched && // I will file a lawsuit
            !strcmp(command, RESET)) { // RESET
        cmd.cmd = SP_RESET;
        cmd.arguments[0] = '\0';
        notMatched = 0;
    }

    if (notMatched) { // Hello? Are you still there?
        cmd.cmd = SP_INVALID_LINE;
        cmd.arguments[0] = '\0';
    }

    return cmd;
}

int spParserGetNonNegativeInt(const SPCommand* command) {
    if (!command) { // Command is NULL
        return -1;
    }

    int result = 0;
    int i = 0;

    while (command->arguments[i]) { // Still haven't reached '\0'
        if (command->arguments[i] >= '0' && command->arguments[i] <= '9') { // Current char is a digit
            result *= 10; // Fix number
            result += command->arguments[i] - '0';
            i++;
        }
        else {
            return -1;
        }
    }
    return result;
}

char spParserGetLocation(const SPCommand* command) {
    return spParserGetBoardLocationFromString(command->arguments); // Just need that string
}

int spParserGetMove(const SPCommand* command) {
	int locations = 0; // Preparations
    locations |= spParserGetBoardLocationFromString(command->arguments); // First coordinates
    locations <<= 8; // Make room
    locations |= spParserGetBoardLocationFromString(command->arguments + 6); // Second coordinates
    return spParserDestroyLocationIfNeeded(locations); // Do they need destruction in their lives?
}
