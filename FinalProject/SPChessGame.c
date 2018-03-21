//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
#include <stdlib.h>
#include <math.h>

#define STARTING_ROW "RNBQKBNR"
#define FIRST_ROW 0
#define LAST_ROW 7
#define LEFT_MOST_COL 0
#define RIGHT_MOST_COL 7
#define PAWN(color) ('M'+color*('a'-'A'))
#define KNIGHT(color) ('N'+color*('a'-'A'))
#define BISHOP(color) ('B'+color*('a'-'A'))
#define ROOK(color) ('R'+color*('a'-'A'))
#define QUEEN(color) ('Q'+color*('a'-'A'))
#define KING(color) ('K'+color*('a'-'A'))
#define BLANK '_'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define SEPARATOR '-'
#define FIRST_COLUMN 'A'
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define KING_LOC(color) (4+color*3*N_COLUMNS)
#define QUEEN_LOC(color) (3+color*3*N_COLUMNS)
#define LEFT_ROOK_LOC(color) (0+color*3*N_COLUMNS)
#define RIGHT_ROOK_LOC(color) (7+color*3*N_COLUMNS)
#define LEFT_KNIGHT_LOC(color) (1+color*3*N_COLUMNS)
#define RIGHT_KNIGHT_LOC(color) (6+color*3*N_COLUMNS)
#define LEFT_BISHOP_LOC(color) (2+color*3*N_COLUMNS)
#define RIGHT_BISHOP_LOC(color) (5+color*3*N_COLUMNS)
#define CHECK_COLOR(color,piece) ((1-2*color)*(piece) < (1-2*color)*('Z'))
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define STAY 0
#define CAPTURES 1
#define HISTORY_SIZE 3
#define COL_NUM_TO_LETTER(column) (column+'A')

/***
 * Check whether a piece at threatLocation can make a DIAGONAL move to targetLocation,
 * without having to jump over a piece enroute.
 * @param src - Game to check move on
 * @param targetLocation - Location you want to get to
 * @param threatLocation - Location you are at
 * @return
 * -1 if src is NULL, or one of the positions isn't on the board
 *  0 if there isn't a legal DIAGONAL move
 *  1 otherwise
 */
int spChessGameCheckDiagonalMove(SPChessGame* src, char targetLocation, char threatLocation) {
    if (!src) { // src is NULL
        return -1;
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    if (targetColumn < 0 || targetColumn > N_COLUMNS - 1 || targetRow < 0 || targetRow > N_ROWS - 1 ||
        threatColumn < 0 || threatColumn > N_COLUMNS - 1 || threatRow < 0 || threatRow > N_ROWS - 1) {
        return -1; // Out of bounds
    }

    char piece = src->gameBoard[threatRow][threatColumn];
    if (!piece) {
        return -1;
    }

    int distance = abs(threatRow - targetRow);
    if (abs(threatColumn - targetColumn) == distance) { // Move is an actual diagonal move
        int columnDirection = threatColumn > targetColumn ? RIGHT : LEFT; // Where are we going?
        int rowDirection = threatRow > targetRow ? DOWN : UP; // And now?
        char temp;
        for (int i = 1; i < distance; i++) {
            temp = src->gameBoard[targetRow + i * rowDirection][targetColumn + i * columnDirection];
            if (temp) { // There's something blocking the way!
                return 0;
            }
        }

        return 1; // Gotcha
    }

    return 0; // Or not?
}

/***
 * Checks whether a piece at threatLocation can make a STRAIGHT LINE move to targetLocation,
 * without having to jump over any piece on the way.
 * @param src - Game to check move in
 * @param targetLocation - Finishing location
 * @param threatLocation - Starting location?
 * @return
 * -1 if src is NULL or one of the locations isn't on the board
 *  0 if no legal move can be made
 *  1 otherwise
 */
int spChessGameCheckStraightLineMove(SPChessGame* src, char targetLocation, char threatLocation) {
    if (!src) { // src is NULL
        return -1;
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    if (targetColumn < 0 || targetColumn > N_COLUMNS - 1 || targetRow < 0 || targetRow > N_ROWS - 1 ||
        threatColumn < 0 || threatColumn > N_COLUMNS - 1 || threatRow < 0 || threatRow > N_ROWS - 1) {
        return -1; // Offside! He's bleeping OFFSIDE!!!
    }

    char piece = src->gameBoard[threatRow][threatColumn];
    if (!piece) {
        return -1;
    }

    char temp;
    int direction;

    if (targetColumn == threatColumn && targetRow != threatRow) { // Horizontal move
        direction = threatColumn > targetColumn ? RIGHT : LEFT;
        for (int i = 1; i < abs(threatRow - targetRow); i++) {
            temp = src->gameBoard[targetRow][targetColumn + i * direction];
            if (temp) { // Blocked
                return 0;
            }
        }

        return 1;
    }

    if (targetRow == threatRow && targetColumn != threatColumn) { // Vertical move
        direction = threatRow > targetRow ? DOWN : UP;
        for (int i = 1; i < abs(threatRow - targetRow); i++) {
            temp = src->gameBoard[targetRow + i * direction][targetColumn];
            if (temp) { // Blockage
                return 0;
            }
        }

        return 1; // All good
    }

    return 0; // Not a STRAIGHT LINE move
}

/***
 * Checks whether a move can be made from threatLocation to targetLocation by a knight.
 * @param src - Game to check on
 * @param targetLocation - Finish
 * @param threatLocation - Start
 * @return
 * -1 if src is NULL or a position is out of bounds
 *  0 if the move cannot be made, not by a Knight Who Says Ni
 *  1 otherwise
 */
int spChessGameCheckKnightMove(SPChessGame* src, char targetLocation, char threatLocation) {
    if (!src) { // src is NULL
        return -1;
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    if (targetColumn < 0 || targetColumn > N_COLUMNS - 1 || targetRow < 0 || targetRow > N_ROWS - 1 ||
        threatColumn < 0 || threatColumn > N_COLUMNS - 1 || threatRow < 0 || threatRow > N_ROWS - 1) {
        return -1; // Not in my back yard!
    }

    int columnDistance = abs(targetColumn - threatColumn);
    int rowDistance = abs(targetRow - threatRow);
    if ((columnDistance == 1 && rowDistance == 2) ||
        (columnDistance == 2 && rowDistance == 1)) {
        return 1; // Welp, he got me!
    }

    return 0; // And I raaaaaaan.....
}

/***
 * Checks whether a king can make the move from threatLocation to targetLocation.
 * @param src - Game to check on
 * @param targetLocation - Finish line
 * @param threatLocation - Start line
 * @return
 * -1 if src is NULL or one of the location's isn't on the board
 *  0 if the move can't be made
 *  1 otherwise
 */
int spChessGameCheckKingThreat(SPChessGame* src, char targetLocation, char threatLocation) {
    if (!src) { // src is NULL
        return -1;
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    if (targetColumn < 0 || targetColumn > N_COLUMNS - 1 || targetRow < 0 || targetRow > N_ROWS - 1 ||
        threatColumn < 0 || threatColumn > N_COLUMNS - 1 || threatRow < 0 || threatRow > N_ROWS - 1) {
        return -1; // A position is off the board
    }

    if (abs(targetColumn - threatColumn) <= 1 && abs(targetRow - threatRow) <= 1) {
        return 1; // Yep
    }

    return 0; // Nope
}

/***
 * Checks whether the piece on src's game board, at location, will be threatened after making move.
 * @param src - Game to check on
 * @param move - Move to make
 * @param location - Location to check
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL, move isn't a legal move, or an error ocurred along the way
 * SP_CHESS_GAME_MOVE_WILL_THREATEN if piece at location will be threatened after making move
 * SP_CHESS_GAME_SUCCESS otherwise
 */
int spChessGameCheckPotentialThreat(SPChessGame* src, int move, char location) {
    if (!src) { // src is NULL
        return -1;
    }

    int lastMove = 0;
    int full = 0;
    int threatened = 0;
    if (spArrayListIsFull(src->history)) { // A move will be lost if we add another to the game's history
        lastMove = spArrayListGetLast(src->history); // So save it for later
        full = 1;
    }

    SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(src, move); // Make the move
    if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) { // Oops, can't guarantee what happened
        return -1;
    }

    if (spChessGameIsPieceThreatened(src, location)) { //
        threatened = 1;
    }

    gameMessage = spChessGameUndoMove(src);
    if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) {
        return -1;
    }

    if (full) {
        SP_ARRAY_LIST_MESSAGE message = spArrayListAddLast(src->history, lastMove);
        if (message != SP_ARRAY_LIST_SUCCESS) {
            return -1;
        }
    }

    return threatened;
}

/***
 * Prepares an int to represent a move, so as to be accepted by spChessGameSetMove(...)
 * @param currentRow
 * @param currentColumn
 * @param destinationRow
 * @param destinationColumn
 * @return
 * An int representing the move indicated by the arguments
 */
int setMoveCoordinatesToInt(int currentRow, int currentColumn, int destinationRow, int destinationColumn) {
    int move = 0; // format is 4 bits current row, 4 current column, 4 destination row, 4 destination column
    move = (currentRow | move) << 4;
    move = (currentColumn | move) << 4;
    move = (destinationRow | move) << 4;
    move = (destinationColumn | move) << 4;
    return move;
}

/***
 * Prepares an int to represent a step, such as the ones returned in the SPArrayList returned by
 * spChessGameGetMoves(...)
 * @param destinationRow
 * @param destinationColumn
 * @param threatened - Moved piece is threatened after move (0 or 1)
 * @param captures - Move captures an opponent's piece (0 or 1)
 * @return
 * An int representing the step
 */
int setStepCoordinatesToInt(int destinationRow, int destinationColumn, int threatened, int captures) {
    int step = 0; // format is 4 bits destination row, 4 destination column, 4 if will be threatened, 4 if captures
    step = (destinationRow | step) << 4;
    step = (destinationColumn | step) << 4;
    step = (threatened | step) << 4;
    step = (captures | step) << 4;
    return step;
}

/***
 * Adds a single step, indicated by move, to steps, by verifying the king isn't threatened after the move,
 * and adding the information as to whether the move captures another piece and/or results in the moving piece
 * being threatened.
 * @param src - Game where the move is made
 * @param steps - SPArrayList to add the step to
 * @param move - move to get step from, assumed to be a legal move!
 * @param color - moving player's color
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT if src or steps are NULL, or an error ocurred while adding the step to steps
 * SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameAddSingleStepToList(SPChessGame* src, SPArrayList* steps, int move, int color) {
    if (!src || !steps) { // Something is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char destinationPosition = spChessGameGetDestinationPositionFromMove(move << 8);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    char piece = src->gameBoard[destinationRow][destinationColumn];
    int threatened = spChessGameCheckPotentialThreat(src, move, destinationPosition) == 1; // Piece will be threatened?
    int kingNotThreatened = spChessGameCheckPotentialThreat(src, move, src->locations[KING_LOC(color)]) == 0; // King won't?
    int step;
    int add = 0;

    if (kingNotThreatened) {
        if (!piece) {
            step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, !CAPTURES);
            add = 1;
        }
        else if (!(CHECK_COLOR(color,piece))) {
            step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, CAPTURES);
            add = 1;
        }
    }

    if (add && spArrayListAddLast(steps, step) != SP_ARRAY_LIST_SUCCESS) { // Add? at least try to
        return SP_CHESS_GAME_INVALID_ARGUMENT; // OOPS
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* src, SPArrayList* steps, char position, int verticalDirection, int horizontalDirection, int color) {
    if (!src || !steps) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int destinationRow;
    int destinationColumn;
    int move;
    char piece;

    int availbleRows = verticalDirection == LEFT ? currentRow + 1 : N_ROWS - currentRow;
    if (!verticalDirection) {
        availbleRows = N_ROWS; // if verticalDirection == 0 we want the # of for loop iterations decided by availbleColumns only
    }

    int availbleColumns = horizontalDirection == UP ? currentColumn + 1 : N_COLUMNS - currentColumn;
    if (!horizontalDirection) {
        availbleColumns = N_COLUMNS;
    }

    if (!verticalDirection && !horizontalDirection) {
        return SP_CHESS_GAME_SUCCESS;
    }

    for (int i = 1; i < min(availbleRows, availbleColumns); i++) {
        destinationRow = currentRow + i * verticalDirection;
        destinationColumn = currentColumn + i * horizontalDirection;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

        if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }

        piece = src->gameBoard[destinationRow][destinationColumn];
        if (piece) {
            return SP_CHESS_GAME_SUCCESS;
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
    if (!src || !steps) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int move;
    int destinationRow;
    int destinationColumn;

    for (int i = -1; i < 2; i += 2) {
        for (int j = -1; j < 2; j += 2) {
            destinationRow = currentRow + i;
            destinationColumn = currentColumn + 2 * i;
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
                if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }

            destinationRow = currentRow + 2 * i;
            destinationColumn = currentColumn + i;
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
                if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddKingStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
    if (!src || !steps) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int move;
    int destinationColumn;
    int destinationRow;

    for (int i = -1; i < 2; i++) {
        for (int i = -1; i < 2; i++) {
            destinationColumn = currentColumn + i;
            destinationRow = currentRow + i;
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
                if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameAddPawnStepsToList(SPChessGame* src, SPArrayList* steps, char position, int color) {
    if (!src || !steps) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentRow = spChessGameGetRowFromPosition(position);
    int currentColumn = spChessGameGetColumnFromPosition(position);
    int direction = color == WHITE ? UP : DOWN;  // if pawn is white, can move up, if black otherwise
    int move;
    int destinationRow;
    int destinationColumn;

    for (int i = 1; i < 3; i++) {
        destinationRow = currentRow + i * direction;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, currentColumn);

        if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
            if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
        else {
            break; // if 1 step isn't valid, 2 steps also aren't
        }
    }

    destinationRow = currentRow + direction;
    for (int i = -1; i < 2; i+=2) {
        destinationColumn = currentColumn + i;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

        if (spChessGameIsValidMove(src, move) == SP_CHESS_GAME_SUCCESS) {
            if (spChessGameAddSingleStepToList(src, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) {
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

int spChessGameGetRowFromStep(int step) {
    return step >> 12; //Get bits [12-15]
}

int spChessGameGetColFromStep(int step) {
    return (step << 20) >> 28; //Get bits [8-11]
}

int spChessGameStepWillThreaten(int step) {
    return (step << 24) >> 28; //Get bits [4-7]
}

int spChessGameStepWillCapture(int step) {
    return (step << 28) >> 28; //Get 4 right most bits
}

/***
 * Resets src's game board to the starting set-up
 * @param src
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if src is NULL
 *         SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* src) {
    if (!src) { // src is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char* starting_row = STARTING_ROW;

    for (int i = 0; i < N_COLUMNS; i++) {
        src->gameBoard[1][i] = PAWN(BLACK); // Put black pawns on the board
        src->gameBoard[6][i] = PAWN(WHITE); // Put white pawns on the board
        src->gameBoard[0][i] = starting_row[i]; // Put other pieces on the board
        src->gameBoard[7][i] = CAPITAL_TO_LOW(starting_row[i]); // For whitey too

        for (int j = 0; j < N_ROWS - 4; j++) {
            src->gameBoard[j + 2][i] = '\0'; // Everything else is empty
        }

        int location = FIRST_ROW << 4;
        location |= i; // Set location as 4 bits row and 4 bits column for black STARTING_ROW
        src->locations[i] = location;

        location = (FIRST_ROW + 1) << 4;
        location |= i; // Set location for pawns
        src->locations[i + N_COLUMNS] = location;

        location = LAST_ROW << 4;
        location |= i; // Set location for white STARTING_ROW
        src->locations[i + 3 * N_COLUMNS] = location;

        location = (LAST_ROW - 1) << 4;
        location |= i; // Set location for white pawns
        src->locations[i + 2 * N_COLUMNS] = location;
    }

    return SP_CHESS_GAME_SUCCESS;
}

SPChessGame* spChessGameCreate() {
    SPChessGame* game = (SPChessGame*) malloc(sizeof(SPChessGame));
    if (!game) { // malloc failed
        return 0;
    }

    game->currentPlayer = WHITE; // Defaults
    game->userColor = WHITE;
    game->difficulty = 2;
    game->gameMode = 1;

    spChessGameResetBoard(game);

    game->history = spArrayListCreate(HISTORY_SIZE);
    if (!game->history) { // Failed creating spArrayList for history
        free(game);
        return 0;
    }

    return game;
}

SPChessGame* spChessGameCopy(SPChessGame* src) {
    if (!src) { // src is NULL
        return 0;
    }

    SPChessGame* ret = (SPChessGame*) malloc(sizeof(SPChessGame));
    if (!ret) { // malloc failed
        return 0;
    }

    ret->currentPlayer = src->currentPlayer; // Copy what needs to be copied
    ret->difficulty = src->difficulty;
    ret->userColor = src->userColor;
    ret->gameMode = src->gameMode;

    for (int i = 0; i < N_ROWS; i++) {
        for (int j = 0; j < N_COLUMNS; j++) {
            if (j < 2) {
                ret->locations[i + j] = src->locations[i + j]; // Copy locations
            }

            ret->gameBoard[i][j] = src->gameBoard[i][j]; // And game board
        }
    }

    ret->history = spArrayListCopy(src->history); // Copy history
    if (!ret->history) {
        free(ret);
        return 0;
    }

    return ret;
}

void spChessGameDestroy(SPChessGame* src) {
    if (src) {
        spArrayListDestroy(src->history);
        free(src);
    }
}

SP_CHESS_GAME_MESSAGE spChessCheckGameState(SPChessGame* src , int color) {
    int kingThreatened = spChessGameIsPieceThreatened(src,src->locations[KING_LOC(color)]);
    int gameOver = 1;
    int index;

    for (int i = 0; i < 2 * N_COLUMNS; i++) {
        index = i + color * 2 * N_COLUMNS;

        if (src->locations[index]) {
            SPArrayList* possibleMoves = spChessGameGetMoves(src, src->locations[index]);
            if (!possibleMoves) {
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }

            if (!spArrayListIsEmpty(possibleMoves)) { // There's a piece that can move
                gameOver = 0; // Game isn't over
            }

            spArrayListDestroy(possibleMoves);
        }
    }

    if (gameOver) {
        if (kingThreatened) {
            return SP_CHESS_GAME_CHECKMATE;
        }
        else {
            return SP_CHESS_GAME_DRAW;
        }
    }
    if (kingThreatened) {
        return SP_CHESS_GAME_CHECK;
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* src, int move) {
    if (!src) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    move <<= 8;
    char currentPosition = spChessGameGetCurrentPositionFromMove(move);
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);

    if (currentColumn < 0 || currentColumn > N_COLUMNS - 1 || currentRow < 0 || currentRow > N_ROWS - 1 ||
            destinationColumn < 0 || destinationColumn > N_COLUMNS - 1 || destinationRow < 0 || destinationRow > N_ROWS - 1) { // Positions not on board
        return SP_CHESS_GAME_INVALID_POSITION;
    }

    if (currentColumn == destinationColumn && currentRow == destinationRow) { // Can't stay in place
        return SP_CHESS_GAME_ILLEGAL_MOVE;
    }

    char piece = src->gameBoard[currentRow][currentColumn];
    int color = CHECK_COLOR(WHITE, piece);
    if (!piece || !CHECK_COLOR(color, piece)) {
        return SP_CHESS_GAME_NO_PIECE_IN_POSITION;
    }

    char captured = src->gameBoard[destinationRow][destinationColumn];
    if (CHECK_COLOR(color, captured) == color) { // Can't capture own piece
        return SP_CHESS_GAME_ILLEGAL_MOVE;
    }

    char kingLocation = src->locations[KING_LOC(color)];
    int kingThreatened = spChessGameIsPieceThreatened(src, kingLocation);
    int legalMove = 0;

    if (piece == KING(color)) {
        kingLocation = destinationPosition;
        if (abs(destinationRow - currentRow) <= 1 && abs(destinationColumn - currentColumn) <= 1) {
            legalMove = 1;
        }
    }

    else if (piece == PAWN(color)) {
        int direction = color ? UP : DOWN;  // if pawn is white, can move up, if black otherwise
        int twoStepsRow = color ? LAST_ROW - 1 : FIRST_ROW + 1;

        if (!captured) { // we can only move vertically
            if (destinationColumn == currentColumn) {
                if (destinationRow == currentRow + direction) { // move one step
                    legalMove = 1;
                }
                else if (currentRow == twoStepsRow && destinationRow == currentRow + direction * 2
                   && !src->gameBoard[currentRow + direction][destinationColumn]) { // check conditions for 2 steps
                    legalMove = 1;
                }
            }
        }
        else if (abs(destinationColumn - currentColumn) == 1) { // got here if the move captures, check other conditions
            if (destinationRow == currentRow + direction) {
                legalMove = 1;
            }
        }
    }

    else if (abs(destinationRow - currentRow) == abs(destinationColumn - currentColumn)) { // diagonal move
        if ((piece == QUEEN(color) || piece == BISHOP(color)) &&
                spChessGameCheckDiagonalMove(src, destinationPosition, currentPosition)) { // piece can perform diagonal move
            legalMove = 1;
        }
    }

    else if (destinationRow == currentRow || destinationColumn == currentColumn) { // straight line move
        if ((piece == QUEEN(color) || piece == ROOK(color)) &&
                spChessGameCheckStraightLineMove(src, destinationPosition, currentPosition)) { // piece can perform straight line move
            legalMove = 1;
        }
    }

    else if (piece == KNIGHT(piece)) {
        if (spChessGameCheckKnightMove(src, destinationPosition, currentPosition)) {
            legalMove = 1;
        }
    }

    int kingWillBeThreatened = spChessGameCheckPotentialThreat(src, move, kingLocation);
    if (legalMove) {
        if (kingWillBeThreatened) {
            if (kingThreatened) {
                return SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED;
            }
            else {
                return SP_CHESS_GAME_KING_BECOMES_THREATENED;
            }
        }
        else {
            return SP_CHESS_GAME_SUCCESS;
        }
    }

    return SP_CHESS_GAME_ILLEGAL_MOVE;
}

int spChessGameIsPieceThreatened(SPChessGame* src, char location) {
    if (!src) {
        return -1;
    }

    int column = spChessGameGetColumnFromPosition(location);
    int row = spChessGameGetRowFromPosition(location);

    if (row < 0 || row > N_ROWS - 1 || column < 0 || column > N_COLUMNS - 1) {
        return -1;
    }

    char piece = src->gameBoard[row][column];
    int color = CHECK_COLOR(WHITE, piece);

    if (spChessGameCheckStraightLineMove(src, location, src->locations[LEFT_ROOK_LOC(!color)]) == 1 ||
            spChessGameCheckStraightLineMove(src, location, src->locations[RIGHT_ROOK_LOC(!color)]) == 1) {
        return 1;
    }

    if (spChessGameCheckKnightMove(src, location, src->locations[LEFT_KNIGHT_LOC(!color)]) == 1 ||
            spChessGameCheckKnightMove(src, location, src->locations[RIGHT_KNIGHT_LOC(!color)]) == 1) {
        return 1;
    }

    if (spChessGameCheckDiagonalMove(src, location, src->locations[LEFT_BISHOP_LOC(!color)]) == 1 ||
            spChessGameCheckDiagonalMove(src, location, src->locations[RIGHT_BISHOP_LOC(!color)]) == 1) {
        return 1;
    }

    if (spChessGameCheckDiagonalMove(src, location, src->locations[QUEEN_LOC(!color)]) == 1 ||
            spChessGameCheckStraightLineMove(src, location, src->locations[QUEEN_LOC(!color)]) == 1) {
        return 1;
    }

    if (spChessGameCheckKingThreat(src, location, src->locations[KING_LOC(!color)]) == 1) {
        return 1;
    }

    int protectedRow = color ? FIRST_ROW : LAST_ROW; //pawn cant get there
    if (row == protectedRow) {
        return 0;
    }

    int direction = color ? UP : DOWN; //check pawn threat
    char opponentPawn = PAWN(!color);

    if ((column != LEFT_MOST_COL && src->gameBoard[row + direction][column - 1] == opponentPawn) ||
            (column != RIGHT_MOST_COL && src->gameBoard[row + direction][column + 1] == opponentPawn)) {
        return 1;
    }

    return 0;
}

SPArrayList* spChessGameGetMoves(SPChessGame* src, char position) {
    if (!src) {
        return NULL;
    }

    SPArrayList* steps = spArrayListCreate(30); // TODO - find a better way to limit number of moves?
    if (!steps) {
        return 0;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    char piece = src->gameBoard[currentRow][currentColumn];
    int color = CHECK_COLOR(WHITE,piece);

    if (piece == QUEEN(color) || piece == BISHOP(color)) {
        spChessGameAddStepsToList(src, steps, position, DOWN, LEFT, color);
        spChessGameAddStepsToList(src, steps, position, UP, LEFT, color);
        spChessGameAddStepsToList(src, steps, position, DOWN, RIGHT, color);
        spChessGameAddStepsToList(src, steps, position, UP, RIGHT, color);
    }

    if (piece == QUEEN(color) || piece == ROOK(color)) {
        spChessGameAddStepsToList(src, steps, position, STAY, LEFT, color);
        spChessGameAddStepsToList(src, steps, position, DOWN, STAY, color);
        spChessGameAddStepsToList(src, steps, position, UP, STAY, color);
        spChessGameAddStepsToList(src, steps, position, STAY, RIGHT, color);
    }

    if (piece == PAWN(color)) {
        spChessGameAddPawnStepsToList(src, steps, position, color);
    }

    if (piece == KNIGHT(color)) {
        spChessGameAddKnightStepsToList(src, steps, position, color);
    }

    if (piece == KING(color)) {
        spChessGameAddKingStepsToList(src, steps, position, color);
    }

    return steps;
}

SP_CHESS_GAME_MESSAGE spChessPrintMoves(SPArrayList* list) {
    if (!list) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int numOfMoves = spArrayListSize(list);
    int step;
    int destinationRow;
    int destinationCol;

    for (int i = 0 ; i < numOfMoves; i++) {
        step = spArrayListGetAt(list, i);
        destinationRow = spChessGameGetRowFromStep(step);
        destinationCol = spChessGameGetColFromStep(step);
        printf("<%d,%c>", 8 - destinationRow, COL_NUM_TO_LETTER(destinationCol));

        if (spChessGameStepWillThreaten(step)) {
            printf("*");
        }
        if (spChessGameStepWillCapture(step)) {
            printf("^");
        }
        printf("\n");
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* src, int move) {
    if (!src) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    move <<= 8; // Prepare move for history
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    char currentPosition = spChessGameGetCurrentPositionFromMove(move);
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);
    char captured = src->gameBoard[destinationRow][destinationColumn]; // Get (potentially) captured piece
    move |= captured; // Add captured piece to move for history

    SP_ARRAY_LIST_MESSAGE message;
    if (spArrayListIsFull(src->history)) {
        message = spArrayListRemoveLast(src->history); // Make room for new move in history, if needed
        if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

    message = spArrayListAddFirst(src->history, move); // Add move to history
    if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // Shouldn't happen
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    for (int i = 0; i < N_COLUMNS * 4; i++) {
        if (src->locations[i] == destinationPosition) { // Change captured piece's location
            src->locations[i] = 0;
        }

        if (src->locations[i] == currentPosition) { // Change moved piece's location
            src->locations[i] = destinationPosition;
        }
    }

    src->gameBoard[destinationRow][destinationColumn] = src->gameBoard[currentRow][currentColumn]; // Set board accordingly
    src->gameBoard[currentRow][currentColumn] = '\0';

    src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE; // Change current player

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameUndoMove(SPChessGame* src) {
    if (!src) { // src is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    if (spArrayListSize(src->history) == 0) { // no move to undo
        return SP_CHESS_GAME_NO_HISTORY;
    }

    int move = spArrayListGetFirst(src->history); // get last move made
    SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(src->history); // remove move from history
    if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char captured = CLEAN_EXCESS_BYTES(move); // Get captured piece
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move); // Location of captured piece
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    char currentPosition = spChessGameGetCurrentPositionFromMove(move); // Location of moved piece
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);

    src->gameBoard[currentRow][currentColumn] = src->gameBoard[destinationRow][destinationColumn]; // Set board
    src->gameBoard[destinationRow][destinationColumn] = captured;
    int currentPlayer = src->currentPlayer = src->currentPlayer == WHITE ? BLACK : WHITE; // And current player

    for (int i = 0; i < N_COLUMNS * 4; i++) { // Update location for moved piece
        if (src->locations[i] == destinationPosition) {
            src->locations[i] = currentPosition;
        }
    }

    if (captured == PAWN(currentPlayer)) { // Update location if pawn was captured
        int startIndex = currentPlayer ? 2 * N_COLUMNS : N_COLUMNS; // Pawn locations by color
        for (int i = 0; i < N_COLUMNS; i++) {
            if (!src->locations[i + startIndex]) {
                src->locations[i + startIndex] = destinationPosition;
            }
        }
    }

    int index = -1;
    if (captured == KNIGHT(currentPlayer)) { // Update location if knight was captured
        if (!src->locations[RIGHT_KNIGHT_LOC(currentPlayer)]) {
            index = RIGHT_KNIGHT_LOC(currentPlayer);
        }
        else if (!src->locations[LEFT_KNIGHT_LOC(currentPlayer)]) {
            index = LEFT_KNIGHT_LOC(currentPlayer);
        }
    }

    if (captured == ROOK(currentPlayer)) { // Rook taken
        if (!src->locations[RIGHT_ROOK_LOC(currentPlayer)]) {
            index = RIGHT_ROOK_LOC(currentPlayer);
        }
        else if (!src->locations[LEFT_ROOK_LOC(currentPlayer)]) {
            index = LEFT_ROOK_LOC(currentPlayer);
        }
    }

    if (captured == BISHOP(currentPlayer)) { // Bishop
        if (!src->locations[RIGHT_BISHOP_LOC(currentPlayer)]) {
            index = RIGHT_BISHOP_LOC(currentPlayer);
        }
        else if (!src->locations[LEFT_BISHOP_LOC(currentPlayer)]) {
            index = LEFT_BISHOP_LOC(currentPlayer);
        }
    }

    if (captured == QUEEN(currentPlayer)) { // QUEEN???
        index = QUEEN_LOC(currentPlayer);
    }
    if (index != -1) {
        src->locations[index] = destinationPosition;
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameFprintBoard(SPChessGame* src, FILE* file) {
    if (!src || !file) { // src or file are NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    for (int i = 0; i < N_ROWS; i++) {
        fprintf(file, "%d|", N_ROWS - i); // Print row number
        for (int j = 0; j < N_COLUMNS; j++) {
            char currChar = src->gameBoard[i][j];
            fprintf(file, " %c", currChar ? currChar : BLANK); // Print piece if it's there
        }
        fprintf(file, " |\n"); // End row
    }

    fprintf(file, "  "); // Some spaces

    for (int i = 0; i < 2 * N_COLUMNS + 1; i++) {
        fprintf(file, "%c", SEPARATOR); // Close the board at the bottom
    }

    fprintf(file, "\n  "); // WHITE SPACE

    for (int i = 0; i < N_COLUMNS; i++) {
        fprintf(file, " %c", FIRST_COLUMN + i); // Column letters
    }

    fprintf(file, "\n"); // Voila

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* src) {
    return spChessGameFprintBoard(src, stdout);
}

int spChessGameGetLastMovePlayed(SPChessGame* game) {
    if (!game || spArrayListIsEmpty(game->history)) {
        return -1;
    }

    return spArrayListGetFirst(game->history) >> 8;
}

SP_CHESS_GAME_MESSAGE spChessGameResetGame(SPChessGame* game) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    game->currentPlayer = WHITE; // Defaults
    game->userColor = WHITE;
    game->difficulty = 2;
    game->gameMode = 1;

    spChessGameResetBoard(game);

    while (!spArrayListIsEmpty(game->history)) {
        if (spArrayListRemoveFirst(game->history) != SP_ARRAY_LIST_SUCCESS) {
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}
