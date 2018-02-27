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
    SP_INVALID_LINE
} SP_COMMAND;

// TODO - better way to save arguments? we have file paths, board positions and integers...
typedef struct command_t {
    SP_COMMAND cmd;
    char arguments[2000];
} SPCommand;

SPCommand spParserParseLine(const char* str);

int spParserGetPositiveInt(const SPCommand* command);

int* spParserGetLocationForGetMoves(const SPCommand* command);

int** spParserGetMove(const SPCommand* command);
#endif //SOFTWARE_PROJECT_SPCHESSPARSER_H
