//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSPARSER_H
#define SOFTWARE_PROJECT_SPCHESSPARSER_H

// Defines
#define SP_MAX_LINE_LENGTH 2048

// All the different commands
typedef enum {
    SP_GAME_MODE,
    SP_DIFFICULTY,
    SP_USER_COLOR,
    SP_LOAD,
    SP_DEFAULT,
    SP_PRINT_SETTINGS,
    SP_QUIT,
    SP_START,
    SP_MOVE,
    SP_GET_MOVES,
    SP_SAVE,
    SP_UNDO,
    SP_RESET,
    SP_INVALID_LINE,
} SP_COMMAND;

// TODO - better way to save arguments? we have file paths, board positions and integers...
typedef struct command_t {
    SP_COMMAND cmd;
    char arguments[2000];
} SPCommand;

/***
 * Checks if the given string starts with a legal command,
 * and has enough argument in the correct places,
 * and parses it into a SPCommand.
 * @param str - the string to parse
 * @return A parsed command, such that -
 *          * cmd is the correct command entered
 *          * arguments contains the minimum arguments needed, with a '\0' separating arguments
 */
SPCommand spParserParseLine(const char* str);

/***
 * Given a command, parses its first argument into a positive integer.
 * @param command - has argument to parse
 * @return -1 if first argument in cmd->arguments isn't a positive int
 *         The argument as an int, otherwise
 */
int spParserGetPositiveInt(const SPCommand* command);

/***
 * Given a SPCommand* command, tries converting its first argument to coordinates on the board,
 * and fills those coordinates into the first two elements of location.
 * @param command - contains the argument to convert
 * 		  location - an array of integers, of length at least two, to be filled with coordinates
 */
void spParserGetLocationForGetMoves(SPCommand* command, int* location);

/***
 * Given a SPCommand* command, tries parsing its first two arguments to a pair of coordinates of a move,
 * and fills locations' first two elements with those coordinates.
 * @param command - contains the arguments to parse
 * 		  locations - an array of arrays of integers, all of whom are of length at least two.
 */
void spParserGetMove(SPCommand* command, int** locations);

#endif //SOFTWARE_PROJECT_SPCHESSPARSER_H
