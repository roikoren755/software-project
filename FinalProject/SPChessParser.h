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

// And a struct, too!
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
 * and returns a char representing those coordinates.
 * @param command - contains the argument to convert
 * @return A char with all bits 1's, if command->arguments doesn't start with valid coordinates.
 * 		   a char, whose 4 MSB are the bit representation of the row coordinate,
 * 		   and 4 LSB are the bit representation of the column coordinate, otherwise.
 */
char spParserGetLocationForGetMoves(SPCommand* command);

/***
 * Given a SPCommand* command, tries parsing its first two arguments to a pair of coordinates of a move,
 * and returns an int representing those two sets of coordinates.
 * @param command - contains the arguments to parse
 * @return An integer whose two least significant BYTES would be the result of calling
 * 		   the func8tion above with the appropriate arguments.
 */
int spParserGetMove(SPCommand* command);

#endif //SOFTWARE_PROJECT_SPCHESSPARSER_H
