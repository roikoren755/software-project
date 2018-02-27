//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSGAME_H
#define SOFTWARE_PROJECT_SPCHESSGAME_H

#include "SPArrayList.h"

typedef struct sp_chess_settings_t {
    int game_mode;
    int difficulty;
    int user_color;
} SPChessSettings;

typedef struct sp_chess_game_t {
    SPChessSettings settings;
    char gameBoard[8][8];
    int currentPlayer;
    SPArrayList* history;
} SPChessGame;

typedef enum sp_chess_game_message_t {
    SP_CHESS_GAME_INVALID_ARGUMENT,
    SP_CHESS_GAME_INVALID_COMMAND,
    SP_CHESS_GAME_SUCCESS,
    SP_CHESS_GAME_INVALID_POSITION,
    SP_CHESS_GAME_NO_PIECE_IN_POSITION,
    SP_CHESS_GAME_ILLEGAL_MOVE,
    SP_CHESS_GAME_ILLEGAL_MOVE_WILL_THREATEN,
    SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED
};

SPChessGame* spChessGameCreate(int historySize);

SPChessGame* spChessGameCopy(SPChessGame* src);

void spChessGameDestroy(SPChessGame* src);

#endif //SOFTWARE_PROJECT_SPCHESSGAME_H
