//
// Created by Roi Koren on 27/02/2018.
//

#ifndef SOFTWARE_PROJECT_SPCHESSGAME_H
#define SOFTWARE_PROJECT_SPCHESSGAME_H

#include "SPArrayList.h"
#include <stdbool.h>

#define WHITE 1
#define BLACK 0
#define N_ROWS 8
#define N_COLUMNS 8


typedef struct sp_chess_game_t {
	int game_mode;
	int difficulty;
	int user_color;
    char gameBoard[N_ROWS][N_COLUMNS];
	char locations[N_COLUMNS * 4];
    int currentPlayer;
    bool whiteKingThreaten;
    bool blackKingThreaten;
    SPArrayList* history;
} SPChessGame;

typedef enum sp_chess_game_message_t {
    SP_CHESS_GAME_INVALID_ARGUMENT,
    SP_CHESS_GAME_INVALID_COMMAND,
    SP_CHESS_GAME_SUCCESS,
    SP_CHESS_GAME_INVALID_POSITION,
    SP_CHESS_GAME_NO_PIECE_IN_POSITION,
    SP_CHESS_GAME_ILLEGAL_MOVE,
    SP_CHESS_GAME_MOVE_WILL_THREATEN,
    SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED,
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_ALOCATION_ERROR,
} SP_CHESS_GAME_MESSAGE;

SPChessGame* spChessGameCreate(int historySize);

SPChessGame* spChessGameCopy(SPChessGame* src);

/**
 * given an in range move, checks if the move is valid regarding the piece at the
 * current position.
 *
 * @param src - The source game
 * @param move - The specified move.
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* src, int move);
/**
 * given a move, Checks if a the piece that after the move will be
 * at the given location will be threaten (after that move).
 * this function does not check whether the move is valid regarding the movement abilities
 * of the pieces.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move,char location){

/**
 * Checks if a piece is threatened by an opponent.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
bool spChessGameIsPieceThreaten(SPChessGame* src, char pieceLocation);

bool spChessGameCheckDiagonalMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckStraightLineMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckKnightMove(SPChessGame* src , char targetLoc , char threatLoc);

bool spChessGameCheckKingThreat(SPChessGame* src , char targetLoc , char threatLoc);

/**
 * returns all possible moves of the piece located at <x,y>.
 * This function assumes there is a piece in that location.
 *
 * @param src - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
SPArrayList* spChessGameGetMoves(SPChessGame* src, int position);

SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* src,SPArrayList* steps, char position, int verDir, int horDir,int color) ;

SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* src,SPArrayList* steps, char position,int color) ;

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move);

SP_CHESS_GAME_MESSAGE spChessGameUndoPrevMove(SPChessGame* src);

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* src);

void spChessGameDestroy(SPChessGame* src);


char spChessGameGetDestPositionFromMove(int move);

char spChessGameGetCurrPositionFromMove(int move);

int spChessGameGetColumnFromPosition(char position);

int spChessGameGetRowFromPosition(char position);

<<<<<<< HEAD
int setMoveCoordinatesToInt(int curRow,int curCol, int destRow, int destCol);

int setStepCoordinatesToInt(int destRow,int destCol, int capture, int threaten);


=======
>>>>>>> 975194953dad0156def63248e1c17ca0e4307bc7
#endif //SOFTWARE_PROJECT_SPCHESSGAME_H
