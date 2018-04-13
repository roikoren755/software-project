//
// Created by Roi Koren on 27/02/2018.
//

#include "SPChessGame.h"
#include "SPMainAux.h"
#include <stdlib.h>

#define FIRST_ROW 0
#define LAST_ROW 7
#define LEFT_MOST_COL 0
#define RIGHT_MOST_COL 7
#define SEPARATOR '-'
#define FIRST_COLUMN 'A'
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define STAY 0
#define CAPTURES 1
#define HISTORY_SIZE 3

/***
 * Verifies the location represents a valid location on the board.
 * @param location - Location to verify
 * @return 0 if location does not represent a valid location.
 *         1 otherwise.
 */
int spChessGameVerifyLocation(char location) {
	int column = spChessGameGetColumnFromPosition(location);
    int row = spChessGameGetRowFromPosition(location);

    if (!location || column < 0 || column >= N_COLUMNS || row < 0 || row >= N_ROWS) {
    	return 0;
    }

    return 1;
}

/***
 * Check whether a piece at threatLocation can make a DIAGONAL move to targetLocation,
 * without having to jump over a piece enroute.
 * @param game - Game to check move on
 * @param targetLocation - Location you want to get to
 * @param threatLocation - Location you are at
 * @return
 * -1 if game is NULL, or one of the positions isn't on the board
 *  0 if there isn't a legal DIAGONAL move
 *  1 otherwise
 */
int spChessGameCheckDiagonalMove(SPChessGame* game, char targetLocation, char threatLocation) {
    if (!game) { // game is NULL
        return -1;
    }

    if (!spChessGameVerifyLocation(targetLocation) ||
        !spChessGameVerifyLocation(threatLocation)) {
        return -1; // Out of bounds
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    char piece = game->gameBoard[threatRow][threatColumn];
    if (!piece) {
        return -1;
    }

    int distance = abs(threatRow - targetRow);
    if (abs(threatColumn - targetColumn) == distance) { // Move is an actual diagonal move
        int columnDirection = threatColumn > targetColumn ? LEFT : RIGHT; // Where are we going?
        int rowDirection = threatRow > targetRow ? UP : DOWN; // And now?
        char temp;
        int i;
    	for (i = 1; i < distance; i++) {
            temp = game->gameBoard[threatRow + i * rowDirection][threatColumn + i * columnDirection];
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
 * @param game - Game to check move in
 * @param targetLocation - Finishing location
 * @param threatLocation - Starting location?
 * @return
 * -1 if game is NULL or one of the locations isn't on the board
 *  0 if no legal move can be made
 *  1 otherwise
 */
int spChessGameCheckStraightLineMove(SPChessGame* game, char targetLocation, char threatLocation) {
    if (!game) { // game is NULL
        return -1;
    }

    if (!spChessGameVerifyLocation(targetLocation) ||
        !spChessGameVerifyLocation(threatLocation)) {
        return -1; // Out of bounds
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    char piece = game->gameBoard[threatRow][threatColumn];
    if (!piece) {
        return -1;
    }

    char temp;
    int direction;

    if (targetColumn != threatColumn && targetRow == threatRow) { // Horizontal move
        direction = threatColumn > targetColumn ? LEFT : RIGHT;

    	int i;
    	for (i = 1; i < abs(targetColumn - threatColumn); i++) {
            temp = game->gameBoard[threatRow][threatColumn + i * direction];
            if (temp) { // Blocked
                return 0;
            }
        }

        return 1;
    }

    if (targetRow != threatRow && targetColumn == threatColumn) { // Vertical move
        direction = threatRow > targetRow ? UP : DOWN;

    	int i;
    	for (i = 1; i < abs(targetRow - threatRow); i++) {
            temp = game->gameBoard[threatRow + i * direction][threatColumn];
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
 * @param game - Game to check on
 * @param targetLocation - Finish
 * @param threatLocation - Start
 * @return
 * -1 if game is NULL or a position is out of bounds
 *  0 if the move cannot be made, not by a Knight Who Says Ni
 *  1 otherwise
 */
int spChessGameCheckKnightMove(SPChessGame* game, char targetLocation, char threatLocation) {
    if (!game) { // game is NULL
        return -1;
    }

    if (!spChessGameVerifyLocation(targetLocation) ||
        !spChessGameVerifyLocation(threatLocation)) {
        return -1; // Out of bounds
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    int columnDistance = abs(targetColumn - threatColumn);
    int rowDistance = abs(targetRow - threatRow);
    if ((columnDistance == 1 && rowDistance == 2) ||
        (columnDistance == 2 && rowDistance == 1)) {
        return 1; // Welp, he got me!
    }

    return 0; // And I raaaaaaan.....
}

/***
 * Checks whether a king at threatLocation is threatening piece at targetLocation.
 * @param game - Game to check on
 * @param targetLocation - Finish line
 * @param threatLocation - Start line
 * @return
 * -1 if game is NULL or one of the location's isn't on the board
 *  0 if the king isn't threatening targetLocation
 *  1 otherwise
 */
int spChessGameCheckKingThreat(SPChessGame* game, char targetLocation, char threatLocation) {
    if (!game) { // game is NULL
        return -1;
    }

    if (!spChessGameVerifyLocation(targetLocation) ||
        !spChessGameVerifyLocation(threatLocation)) {
        return -1; // Out of bounds
    }

    int targetColumn = spChessGameGetColumnFromPosition(targetLocation);
    int targetRow = spChessGameGetRowFromPosition(targetLocation);
    int threatColumn = spChessGameGetColumnFromPosition(threatLocation);
    int threatRow = spChessGameGetRowFromPosition(threatLocation);

    if (abs(targetColumn - threatColumn) <= 1 && abs(targetRow - threatRow) <= 1) {
        return 1; // Yep
    }

    return 0; // Nope
}

/**
 * Checks if a piece is threatened by an opponent.
 *
 * @param game - The source game
 * @param col - The specified column
 * @return
 * true  - if the a disc can be put in the target column
 * false - otherwise.
 */
int spChessGameIsPieceThreatened(SPChessGame* game, char location) {
    if (!game || !spChessGameVerifyLocation(location)) {
        return -1;
    }

    int column = spChessGameGetColumnFromPosition(location);
    int row = spChessGameGetRowFromPosition(location);

    char piece = game->gameBoard[row][column];
    int color = CHECK_COLOR(piece);
    int opponentColor = color ? BLACK : WHITE;

    if (spChessGameCheckStraightLineMove(game, location, game->locations[LEFT_ROOK_LOC(opponentColor)]) == 1 ||
        spChessGameCheckStraightLineMove(game, location, game->locations[RIGHT_ROOK_LOC(opponentColor)]) == 1) {
        return 1;
    }

    if (spChessGameCheckKnightMove(game, location, game->locations[LEFT_KNIGHT_LOC(opponentColor)]) == 1 ||
        spChessGameCheckKnightMove(game, location, game->locations[RIGHT_KNIGHT_LOC(opponentColor)]) == 1) {
        return 1;
    }

    if (spChessGameCheckDiagonalMove(game, location, game->locations[LEFT_BISHOP_LOC(opponentColor)]) == 1 ||
        spChessGameCheckDiagonalMove(game, location, game->locations[RIGHT_BISHOP_LOC(opponentColor)]) == 1) {
        return 1;
    }

    if (spChessGameCheckDiagonalMove(game, location, game->locations[QUEEN_LOC(opponentColor)]) == 1 ||
        spChessGameCheckStraightLineMove(game, location, game->locations[QUEEN_LOC(opponentColor)]) == 1) {
        return 1;
    }

    if (spChessGameCheckKingThreat(game, location, game->locations[KING_LOC(opponentColor)]) == 1) {
        return 1;
    }

    int protectedRow = color ? FIRST_ROW : LAST_ROW; //pawn cant get there
    if (row == protectedRow) {
        return 0;
    }

    int direction = color ? UP : DOWN; //check pawn threat
    char opponentPawn = PAWN(opponentColor);

    if ((column != LEFT_MOST_COL && game->gameBoard[row + direction][column - 1] == opponentPawn) ||
        (column != RIGHT_MOST_COL && game->gameBoard[row + direction][column + 1] == opponentPawn)) {
        return 1;
    }

    return 0;
}

/***
 * Checks whether the piece on game's game board, at location, will be threatened after making move.
 * @param game - Game to check on
 * @param move - Move to make
 * @param location - Location to check
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT if game is NULL, move isn't a legal move, or an error ocurred along the way
 * SP_CHESS_GAME_MOVE_WILL_THREATEN if piece at location will be threatened after making move
 * SP_CHESS_GAME_SUCCESS otherwise
 */
int spChessGameCheckPotentialThreat(SPChessGame* game, int move, char location) {
    if (!game) { // game is NULL
        return -1;
    }

    if (!spChessGameVerifyLocation(location)) {
        return -1; // Out of bounds
    }

    int lastMove = 0;
    int threatened = 0;
    if (spArrayListIsFull(game->history)) { // A move will be lost if we add another to the game's history
        lastMove = spArrayListGetLast(game->history); // So save it for later
    }

    SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(game, move); // Make the move
    if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) { // Oops, can't guarantee what happened
        return -1;
    }

    if (spChessGameIsPieceThreatened(game, location) == 1) { // Is anyone threatening me now?
        threatened = 1;
    }

    if (!spUndoAndRestoreHistory(game, lastMove)) { // Undo move, and restore "forgotten" move
        return -1;
    }

    return threatened;
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
    int step = spChessGameSetLocation(destinationRow, destinationColumn) << 4; // format is 8 bits destination location, 4 if will be threatened, 4 if captures
    step = (threatened | step) << 4;
    step |= captures;
    return step;
}

/***
 * Adds a single step, indicated by move, to steps, by verifying the king isn't threatened after the move,
 * and adding the information as to whether the move captures another piece and/or results in the moving piece
 * being threatened.
 * @param game - Game where the move is made
 * @param steps - SPArrayList to add the step to
 * @param move - move to get step from, assumed to be a legal move!
 * @param color - moving player's color
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT if game or steps are NULL, or an error ocurred while adding the step to steps
 * SP_CHESS_GAME_SUCCESS otherwise
 */
SP_CHESS_GAME_MESSAGE spChessGameAddSingleStepToList(SPChessGame* game, SPArrayList* steps, int move, int color) {
    if (!game || !steps) { // Something is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char destinationPosition = spChessGameGetDestinationPositionFromMove(move << 8);
    char currentPosition = spChessGameGetCurrentPositionFromMove(move << 8);
    if (!spChessGameVerifyLocation(currentPosition) ||
        !spChessGameVerifyLocation(destinationPosition)) { // One of the positions is out of bounds
    	return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentRow = spChessGameGetRowFromPosition(currentPosition);
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    char piece = game->gameBoard[destinationRow][destinationColumn];
    int threatened = spChessGameCheckPotentialThreat(game, move, destinationPosition) == 1; // Piece will be threatened?
    int kingNotThreatened = game->gameBoard[currentRow][currentColumn] == KING(color) ? // King won't?
                            !threatened : spChessGameCheckPotentialThreat(game, move, game->locations[KING_LOC(color)]) == 0;
    int step;
    int add = 0;

    if (kingNotThreatened) { // Legal move
        if (!piece) { // The doesn't capture
            step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, !CAPTURES);
            add = 1;
        }
        else if (color != CHECK_COLOR(piece)) { // Or that does
            step = setStepCoordinatesToInt(destinationRow, destinationColumn, threatened, CAPTURES);
            add = 1;
        }
    }

    if (add && spArrayListAddLast(steps, step) != SP_ARRAY_LIST_SUCCESS) { // Add? at least try to
        return SP_CHESS_GAME_INVALID_ARGUMENT; // OOPS
    }

    return SP_CHESS_GAME_SUCCESS;
}

/***
 * Adds all possible steps in verticalDirection and horizontalDirection, from position to steps.
 * Assumes piece at position is of color.
 * @param game - Game to check movement at.
 * @param steps - SPArrayList to add steps to
 * @param position - Starting position
 * @param verticalDirection - Moving left or right? Or neither?
 * @param horizontalDirection - Up or down? Neither?
 * @param color - Black or white?
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or steps ar NULL, if position is out of bounds or an error ocurred
 *                                        while adding step to steps.
 *         SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameAddStepsToList(SPChessGame* game, SPArrayList* steps, char position, int verticalDirection, int horizontalDirection, int color) {
    if (!game || !steps || !spChessGameVerifyLocation(position)) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int destinationRow;
    int destinationColumn;
    int move;

    int availbleRows = verticalDirection == LEFT ? currentRow + 1 : N_ROWS - currentRow;
    if (!verticalDirection) {
        availbleRows = N_ROWS; // if verticalDirection == STAY we want the # of for loop iterations decided by availbleColumns only
    }

    int availbleColumns = horizontalDirection == UP ? currentColumn + 1 : N_COLUMNS - currentColumn;
    if (!horizontalDirection) {
        availbleColumns = N_COLUMNS; // If horizontalDirection is STAY, # of for loops decided by availableColumns
    }

    if (!verticalDirection && !horizontalDirection) { // Both are STAY, no moves!
        return SP_CHESS_GAME_SUCCESS;
    }

	int i;
	for (i = 1; i < min(availbleRows, availbleColumns); i++) {
        destinationRow = currentRow + i * verticalDirection;
        destinationColumn = currentColumn + i * horizontalDirection;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn); // One step further
        if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // Valid move?
            if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // Error adding
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

/***
 * Adds all legal knight moves starting at position, assuming piece is of color, in game to steps
 * @param game - Game to check moves in
 * @param steps - SPArrayList to add steps to
 * @param position - Position moves start in
 * @param color - Piece color
 * @return SP_CHESS_GAME_INVALID_ARGUENT if game or steps are NULL, position is out of bounds, or an error occurred
 *                                       while adding step to steps
 *         SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameAddKnightStepsToList(SPChessGame* game, SPArrayList* steps, char position, int color) {
    if (!game || !steps || !spChessGameVerifyLocation(position)) { // Oops
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int move;
    int destinationRow;
    int destinationColumn;

	int i,j;
	for (i = -1; i < 2; i += 2) {
        for (j = -1; j < 2; j += 2) {
            destinationRow = currentRow + i;
            destinationColumn = currentColumn + 2 * j; // One knight move
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // That is valid
                if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // Couldn't add!
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }

            destinationRow = currentRow + 2 * i;
            destinationColumn = currentColumn + j; // Different knight move
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // Is valid
                if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // No add
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

/***
 * Adds king steps, starting at position, assuming piece is of color, to steps, chacking validity at game's game board.
 * @param game - Game to check moves at.
 * @param steps - SPArrayList to add steps to
 * @param position - Starting position
 * @param color - Piece's color
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or steps are NULL, position in out of bounds or an error occurred
 *                                        while adding single step to list.
 *         SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameAddKingStepsToList(SPChessGame* game, SPArrayList* steps, char position, int color) {
    if (!game || !steps || !spChessGameVerifyLocation(position)) { // Oops...
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    int move;
    int destinationColumn;
    int destinationRow;

	int i,j;
	for (i = -1; i < 2; i++) {
        for ( j = -1; j < 2; j++) {
            destinationColumn = currentColumn + j;
            destinationRow = currentRow + i; // One step in one direction
            move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

            if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // Valid!
                if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // NOOOOO
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

/***
 * Adds pawn steps to steps, starting at position, assuming piece is of color, checking validity at game's game board.
 * @param game - Game to check validity at.
 * @param steps - SPArrayList to add steps to.
 * @param position - Starting position of the steps
 * @param color - Piece's color
 * @return SP_CHESS_GAME_INVALID_ARGUMENT if game or steps are NULL, position is out of bounds or an error occurred while
 *                                        adding step to steps.
 *         SP_CHESS_GAME_SUCCESS otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameAddPawnStepsToList(SPChessGame* game, SPArrayList* steps, char position, int color) {
    if (!game || !steps || !spChessGameVerifyLocation(position)) { // What is this you sent me? I don't want this!
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int currentRow = spChessGameGetRowFromPosition(position);
    int currentColumn = spChessGameGetColumnFromPosition(position);
    int direction = color ? UP : DOWN;  // if pawn is white, can move up, if black otherwise
    int move;
    int destinationRow;
    int destinationColumn;

	int i;
	for (i = 1; i < 3; i++) { // Can move one or two steps in direction
        destinationRow = currentRow + i * direction;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, currentColumn);

        if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // Can he?
            if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // But why?
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
    }

    destinationRow = currentRow + direction;

	for (i = -1; i < 2; i += 2) { // Check possible capturing moves
        destinationColumn = currentColumn + i;
        move = setMoveCoordinatesToInt(currentRow, currentColumn, destinationRow, destinationColumn);

        if (spChessGameIsValidMove(game, move) == SP_CHESS_GAME_SUCCESS) { // YAY!
            if (spChessGameAddSingleStepToList(game, steps, move, color) == SP_CHESS_GAME_INVALID_ARGUMENT) { // NAY...
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameResetBoard(SPChessGame* game) {
    if (!game) { // game is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char* starting_row = STARTING_ROW;

	int i,j;
	for (i = 0; i < N_COLUMNS; i++) {
        game->gameBoard[1][i] = PAWN(BLACK); // Put black pawns on the board
        game->gameBoard[6][i] = PAWN(WHITE); // Put white pawns on the board
        game->gameBoard[0][i] = starting_row[i]; // Put other pieces on the board
        game->gameBoard[7][i] = CAPITAL_TO_LOW(starting_row[i]); // For whitey too

        for (j = 0; j < N_ROWS - 4; j++) {
            game->gameBoard[j + 2][i] = '\0'; // Everything else is empty
        }

        game->locations[i] = spChessGameSetLocation(FIRST_ROW, i); // Black STARTING_ROW
        game->locations[i + N_COLUMNS] = spChessGameSetLocation(FIRST_ROW + 1, i); // Black pawns
        game->locations[i + 3 * N_COLUMNS] = spChessGameSetLocation(LAST_ROW, i); // White STARTING_ROW
        game->locations[i + 2 * N_COLUMNS] = spChessGameSetLocation(LAST_ROW - 1, i); // White pawns
    }
	
	game->gameState = NORMAL; // Game starts as not check/checkmate/draw

	game->currentPlayer = WHITE;

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
    game->gameState = NORMAL;

    spChessGameResetBoard(game);

    game->history = spArrayListCreate(2 * HISTORY_SIZE);
    if (!game->history) { // Failed creating spArrayList for history
        free(game);
        return 0;
    }

    return game;
}

SPChessGame* spChessGameCopy(SPChessGame* game) {
    if (!game) { // game is NULL
        return 0;
    }

    SPChessGame* ret = (SPChessGame*) malloc(sizeof(SPChessGame));
    if (!ret) { // malloc failed
        return 0;
    }

    ret->currentPlayer = game->currentPlayer; // Copy what needs to be copied
    ret->difficulty = game->difficulty;
    ret->userColor = game->userColor;
    ret->gameMode = game->gameMode;
    ret->gameState = game->gameMode;

	int i,j;
	for (i = 0; i < N_ROWS; i++) {
        for ( j = 0; j < N_COLUMNS; j++) {
            if (j < 4) {
                ret->locations[i + j * N_COLUMNS] = game->locations[i + j * N_COLUMNS]; // Copy locations
            }

            ret->gameBoard[i][j] = game->gameBoard[i][j]; // And game board
        }
    }

    ret->history = spArrayListCopy(game->history); // Copy history
    if (!ret->history) {
        free(ret);
        return 0;
    }

    return ret;
}

void spChessGameDestroy(SPChessGame* game) {
    if (game) {
        spArrayListDestroy(game->history);
        free(game);
    }
}

SP_CHESS_GAME_MESSAGE spChessCheckGameState(SPChessGame* game , int color) {
	if (!game) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

    int kingThreatened = spChessGameIsPieceThreatened(game, game->locations[KING_LOC(color)]); // Color's king is currently threatened
    int gameOver = 1;
    int index;

	int i;
	for (i = 0; i < 2 * N_COLUMNS; i++) {
        index = i + color * 2 * N_COLUMNS;

        if (game->locations[index]) {
            SPArrayList* possibleMoves = spChessGameGetMoves(game, game->locations[index]);
            if (!possibleMoves) {
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }

            if (!spArrayListIsEmpty(possibleMoves)) { // There's a piece that can move
                gameOver = 0; // Game isn't over
                spArrayListDestroy(possibleMoves);
                break;
            }

            spArrayListDestroy(possibleMoves);
        }
    }

    if (gameOver) { // No moves to make
        if (kingThreatened == 1) { // And king threatened!
        	game->gameState = CHECKMATE;
            return SP_CHESS_GAME_CHECKMATE;
        }
        else { // King isn't threatened
        	game->gameState = DRAW;
            return SP_CHESS_GAME_DRAW;
        }
    }
    if (kingThreatened) { // Or is he?
    	game->gameState = CHECK;
        return SP_CHESS_GAME_CHECK;
    }

    game->gameState = NORMAL; // Never mind, he isn't...
    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameIsValidMove(SPChessGame* game, int move) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char currentPosition = spChessGameGetCurrentPositionFromMove(move << 8);
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move << 8);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);

    if (!spChessGameVerifyLocation(currentPosition) ||
    	!spChessGameVerifyLocation(destinationPosition)) { // Positions not on board
        return SP_CHESS_GAME_INVALID_POSITION;
    }

    if (currentColumn == destinationColumn && currentRow == destinationRow) { // Can't stay in place
        return SP_CHESS_GAME_ILLEGAL_MOVE;
    }

    char piece = game->gameBoard[currentRow][currentColumn];
    int color = game->currentPlayer;
    if (!piece || color != CHECK_COLOR(piece)) { // What are you trying ot move?
        return SP_CHESS_GAME_NO_PIECE_IN_POSITION;
    }

    char captured = game->gameBoard[destinationRow][destinationColumn];
    if (captured && CHECK_COLOR(captured) == color) { // Can't capture own piece
        return SP_CHESS_GAME_ILLEGAL_MOVE;
    }

    char kingLocation = game->locations[KING_LOC(color)];
    int kingThreatened = spChessGameIsPieceThreatened(game, kingLocation); // Is the king threatened?
    int legalMove = 0;

    if (piece == KING(color)) {
        kingLocation = destinationPosition; // Update his position for checkPotentialThreat
        if (abs(destinationRow - currentRow) <= 1 && abs(destinationColumn - currentColumn) <= 1) { // Move one step?
            legalMove = 1;
        }
    }

    else if (piece == PAWN(color)) { // Pawn here
        int direction = color ? UP : DOWN;  // if pawn is white, can move up, if black otherwise
        int twoStepsRow = color ? LAST_ROW - 1 : FIRST_ROW + 1;

        if (!captured) { // we can only move vertically
            if (destinationColumn == currentColumn) {
                if (destinationRow == currentRow + direction) { // move one step
                    legalMove = 1;
                }
                else if (currentRow == twoStepsRow && destinationRow == currentRow + direction * 2 &&
                		 !game->gameBoard[currentRow + direction][destinationColumn]) { // check conditions for 2 steps
                    legalMove = 1;
                }
            }
        }
        else if (abs(destinationColumn - currentColumn) == 1 && destinationRow == currentRow + direction) { // got here if the move captures, check other conditions
            legalMove = 1;
        }
    }

    else if (abs(destinationRow - currentRow) == abs(destinationColumn - currentColumn)) { // diagonal move, not by pawn
        if ((piece == QUEEN(color) || piece == BISHOP(color)) && // piece can perform diagonal move
            spChessGameCheckDiagonalMove(game, destinationPosition, currentPosition) == 1) { // And that move is legal
            legalMove = 1;
        }
    }

    else if (destinationRow == currentRow || destinationColumn == currentColumn) { // straight line move
        if ((piece == QUEEN(color) || piece == ROOK(color)) && // piece can perform straight line move
            spChessGameCheckStraightLineMove(game, destinationPosition, currentPosition) == 1) { // And move is legal
            legalMove = 1;
        }
    }

    else if (piece == KNIGHT(color)) { // Knight here
        if (spChessGameCheckKnightMove(game, destinationPosition, currentPosition) == 1) { // Legal knight move
            legalMove = 1;
        }
    }

    if (legalMove) { // If the piece at the starting position can theoretically make the move
    	int kingWillBeThreatened = spChessGameCheckPotentialThreat(game, move, kingLocation);
        if (kingWillBeThreatened == 1) { // Will the king be threatened after the move?
            if (kingThreatened) { // Then it isn't legal
                return SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED;
            }
            else { // For some reason
                return SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED;
            }
        }
        else { // King won't be threatened after the move
            return SP_CHESS_GAME_SUCCESS;
        }
    }

    return SP_CHESS_GAME_ILLEGAL_MOVE; // No no no no no
}

SPArrayList* spChessGameGetMoves(SPChessGame* game, char position) {
    if (!game || !spChessGameVerifyLocation(position)) {
        return NULL;
    }

    SPArrayList* steps = spArrayListCreate(30); // TODO - find a better way to limit number of moves?
    if (!steps) {
        return NULL;
    }

    int changedCurrentPlayer = 0; // Want the ability to getMoves for opponent pieces as well
    int currentColumn = spChessGameGetColumnFromPosition(position);
    int currentRow = spChessGameGetRowFromPosition(position);
    char piece = game->gameBoard[currentRow][currentColumn];
    int color = CHECK_COLOR(piece);

    if (color != game->currentPlayer) {
        game->currentPlayer = game->currentPlayer ? BLACK : WHITE; // So isValidMove will work
        changedCurrentPlayer = 1;
    }

    if (piece == QUEEN(color) || piece == BISHOP(color)) { // Diagonal moves, in all directions
        if (spChessGameAddStepsToList(game, steps, position, DOWN, LEFT, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, UP, LEFT, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, DOWN, RIGHT, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, UP, RIGHT, color) != SP_CHESS_GAME_SUCCESS) {
            spArrayListDestroy(steps);
            return NULL;
        }
    }

    if (piece == QUEEN(color) || piece == ROOK(color)) { // Straight line moves, in all directions
        if (spChessGameAddStepsToList(game, steps, position, STAY, LEFT, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, DOWN, STAY, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, UP, STAY, color) != SP_CHESS_GAME_SUCCESS ||
            spChessGameAddStepsToList(game, steps, position, STAY, RIGHT, color) != SP_CHESS_GAME_SUCCESS) {
            spArrayListDestroy(steps);
            return NULL;
        }
    }

    else if (piece == PAWN(color) && // Pawn moves
             spChessGameAddPawnStepsToList(game, steps, position, color) != SP_CHESS_GAME_SUCCESS) {
        spArrayListDestroy(steps);
        return NULL;
    }

    else if (piece == KNIGHT(color) && // Knight moves
             spChessGameAddKnightStepsToList(game, steps, position, color) != SP_CHESS_GAME_SUCCESS) {
        spArrayListDestroy(steps);
        return NULL;
    }

    else if (piece == KING(color) && // And king moves
             spChessGameAddKingStepsToList(game, steps, position, color) != SP_CHESS_GAME_SUCCESS) {
        spArrayListDestroy(steps);
        return NULL;
    }

    if (changedCurrentPlayer) { // Restore currentPlayer, if we changed it
        game->currentPlayer = game->currentPlayer ? BLACK : WHITE;
    }

    return steps;
}

SP_CHESS_GAME_MESSAGE spChessGameSetMove(SPChessGame* game, int move) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    move <<= 8; // Prepare move for history
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move);
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    char currentPosition = spChessGameGetCurrentPositionFromMove(move);
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);
    char captured = game->gameBoard[destinationRow][destinationColumn]; // Get (potentially) captured piece
    move |= captured; // Add captured piece to move for history

    SP_ARRAY_LIST_MESSAGE message;
    if (spArrayListIsFull(game->history)) {
        message = spArrayListRemoveLast(game->history); // Make room for new move in history, if needed
        if (message != SP_ARRAY_LIST_SUCCESS) { // Shouldn't happen
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

    message = spArrayListAddFirst(game->history, move); // Add move to history
    if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // Shouldn't happen
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }
    int color = CHECK_COLOR(game->gameBoard[currentRow][currentColumn]);
    int opponentColor = color ? BLACK : WHITE;

	int i;
	for (i = 0; i < N_COLUMNS * 2; i++) {
        if (captured && game->locations[i + (opponentColor) * 2 * N_COLUMNS] == destinationPosition) { // Change captured piece's location
            game->locations[i + (opponentColor) * 2 * N_COLUMNS] = 0;
        }

        if (game->locations[i + color * 2 * N_COLUMNS] == currentPosition) { // Change moved piece's location
            game->locations[i + color * 2 * N_COLUMNS] = destinationPosition;
        }
    }

    game->gameBoard[destinationRow][destinationColumn] = game->gameBoard[currentRow][currentColumn]; // Set board accordingly
    game->gameBoard[currentRow][currentColumn] = '\0';

    game->currentPlayer = game->currentPlayer ? BLACK : WHITE; // Change current player

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameUndoMove(SPChessGame* game) {
    if (!game) { // game is NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    if (spArrayListIsEmpty(game->history)) { // no move to undo
        return SP_CHESS_GAME_NO_HISTORY;
    }

    int move = spArrayListGetFirst(game->history); // get last move made
    SP_ARRAY_LIST_MESSAGE message = spArrayListRemoveFirst(game->history); // remove move from history
    if (message == SP_ARRAY_LIST_INVALID_ARGUMENT) { // shouldn't happen
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char captured = CLEAN_EXCESS_BYTES((unsigned int) move); // Get captured piece
    char destinationPosition = spChessGameGetDestinationPositionFromMove(move); // Location of captured piece
    int destinationColumn = spChessGameGetColumnFromPosition(destinationPosition);
    int destinationRow = spChessGameGetRowFromPosition(destinationPosition);
    char currentPosition = spChessGameGetCurrentPositionFromMove(move); // Location of moved piece
    int currentColumn = spChessGameGetColumnFromPosition(currentPosition);
    int currentRow = spChessGameGetRowFromPosition(currentPosition);

    game->gameBoard[currentRow][currentColumn] = game->gameBoard[destinationRow][destinationColumn]; // Set board
    game->gameBoard[destinationRow][destinationColumn] = captured;
    game->currentPlayer = game->currentPlayer ? BLACK : WHITE; // And current player
    int currentPlayer = !(game->currentPlayer);

	int i;
	for (i = 0; i < N_COLUMNS * 4; i++) { // Update location for moved piece
        if (game->locations[i] == destinationPosition) {
            game->locations[i] = currentPosition;
        }
    }

    int index = -1;
    if (captured == PAWN(currentPlayer)) { // Update location if pawn was captured
        int startIndex = currentPlayer ? 2 * N_COLUMNS : N_COLUMNS; // Pawn locations by color
        for ( i = 0; i < N_COLUMNS; i++) {
            if (!game->locations[i + startIndex]) {
                game->locations[i + startIndex] = destinationPosition;
                break;
            }
        }
    }

    else if (captured == KNIGHT(currentPlayer)) { // Update location if knight was captured
        if (!game->locations[RIGHT_KNIGHT_LOC(currentPlayer)]) {
            index = RIGHT_KNIGHT_LOC(currentPlayer);
        }
        else if (!game->locations[LEFT_KNIGHT_LOC(currentPlayer)]) {
            index = LEFT_KNIGHT_LOC(currentPlayer);
        }
    }

    else if (captured == ROOK(currentPlayer)) { // Rook taken
        if (!game->locations[RIGHT_ROOK_LOC(currentPlayer)]) {
            index = RIGHT_ROOK_LOC(currentPlayer);
        }
        else if (!game->locations[LEFT_ROOK_LOC(currentPlayer)]) {
            index = LEFT_ROOK_LOC(currentPlayer);
        }
    }

    else if (captured == BISHOP(currentPlayer)) { // Bishop
        if (!game->locations[RIGHT_BISHOP_LOC(currentPlayer)]) {
            index = RIGHT_BISHOP_LOC(currentPlayer);
        }
        else if (!game->locations[LEFT_BISHOP_LOC(currentPlayer)]) {
            index = LEFT_BISHOP_LOC(currentPlayer);
        }
    }

    else if (captured == QUEEN(currentPlayer)) { // QUEEN???
        index = QUEEN_LOC(currentPlayer);
    }

    else if (captured == KING(currentPlayer)) { // For get moves, and check potential threat functions
        index = KING_LOC(currentPlayer);
    }

    if (index != -1) {
        game->locations[index] = destinationPosition;
    }
    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameFprintBoard(SPChessGame* game, FILE* file) {
    if (!game || !file) { // game or file are NULL
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

	int i;
	for (i = 0; i < N_ROWS; i++) {
        fprintf(file, "%d|", N_ROWS - i); // Print row number
        for (int j = 0; j < N_COLUMNS; j++) {
            char currChar = game->gameBoard[i][j];
            fprintf(file, " %c", currChar ? currChar : BLANK); // Print piece if it's there
        }
        fprintf(file, " |\n"); // End row
    }

    fprintf(file, "  "); // Some spaces

    for ( i = 0; i < 2 * N_COLUMNS + 1; i++) {
        fprintf(file, "%c", SEPARATOR); // Close the board at the bottom
    }

    fprintf(file, "\n  "); // WHITE SPACE

    for ( i = 0; i < N_COLUMNS; i++) {
        fprintf(file, " %c", FIRST_COLUMN + i); // Column letters
    }

    fprintf(file, "\n"); // Voila

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGamePrintBoard(SPChessGame* game) {
    return spChessGameFprintBoard(game, stdout);
}

unsigned int spChessGameGetLastMovePlayed(SPChessGame* game) {
    if (!game || spArrayListIsEmpty(game->history)) {
        return 0;
    }

    return ((unsigned int) spArrayListGetFirst(game->history)) >> 8; // Users of this function don't know move saves captured piece
}

SP_CHESS_GAME_MESSAGE spChessGameResetGame(SPChessGame* game) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    spChessGameResetBoard(game); // Reset board

    while (!spArrayListIsEmpty(game->history)) {
        if (spArrayListRemoveFirst(game->history) != SP_ARRAY_LIST_SUCCESS) { // Delete history
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

    return SP_CHESS_GAME_SUCCESS;
}

int spUndoAndRestoreHistory(SPChessGame* game, int lastMove) {
    int full = spArrayListIsFull(game->history);
    if (spChessGameUndoMove(game) == SP_CHESS_GAME_INVALID_ARGUMENT) {
        return 0;
    }

    if (full && spArrayListAddLast(game->history, lastMove) != SP_ARRAY_LIST_SUCCESS) {
        return 0;
    }

    return 1;
}
