//
// Created by Roi Koren on 28/12/2017.
#include "SPMainAux.h"
#include <string.h>

#define MAXIMUM_COMMAND_LENGTH 1024
#define MAX_FILE_LINE_LENGTH 100
#define DELIMITERS " \t\r\n"

int x=0;

int min(int a, int b) {
    return a < b ? a : b;
}

int runSdl(SPChessGame* game ) {
	//SP_BUFF_SET();

	// initialize SDL2 for video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	int done = 0,feedback,i,j;

	Screen* screens[NUM_SCREENS];
	int success = SPGameCreateScreens(screens);
	if(!success){
		done = 1;
	}
	SPUpdateLoadSaveSlots(screens);
	SDL_Event e;
	printf("1\n");

	while(!done){
		SDL_WaitEvent(&e);
		feedback = NONE; //
			for(i=0; i<NUM_SCREENS; i++){
				if(screens[i]->shown){
					for(j=0; j<screens[i]->widgetsSize; j++){
						if(screens[i]->widgets[j]&&screens[i]->widgets[j]->shown){
							feedback = screens[i]->widgets[j]->handleEvent(screens[i]->widgets[j],
									&e,screens,game,i,j);
						}
						if(feedback == PRESSED){
							break;
						}
						if(feedback == QUIT){
							done = 1;
							break;
						}
					}
					break;
				}
			}

		for(i=0; i<NUM_SCREENS; i++){
			if(screens[i]->shown){
				printf("%d.screen %d shown\n",x++,i);
				screens[i]->draw(screens[i]);
				break;
			}
		}

	}

	spChessGameDestroy(game);

	SPDestroyScreensArr(screens,NUM_SCREENS);
	printf("%d\n",done);




	SDL_Quit();
	return 0;

}

unsigned char spChessGameGetDestinationPositionFromMove(unsigned int move) {
    move >>= 8;
    move <<= 24;
    move >>= 24;
    return (unsigned char) move; // Get 2nd byte from the right
}

unsigned char spChessGameGetCurrentPositionFromMove(unsigned int move) {
    move >>= 16;
    move <<= 24;
    move >>= 24;
    return (unsigned char) move; // Get 3rd Byte from the left
}

unsigned int spChessGameGetColumnFromPosition(unsigned char position) {
    position <<= 5;
    position >>= 5;
    return (unsigned int) position; // Get 3 rightmost bits
}

unsigned int spChessGameGetRowFromPosition(unsigned char position) {
	position <<= 2;
    position >>= 5;
    return (unsigned int) position; // Get next 3 bits
}

SP_CHESS_GAME_MESSAGE spFprintSettings(SPChessGame* game, FILE* file) {
    if (!game || !file) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    char* difficulties[] = {"", "amateur", "easy", "moderate", "hard", "expert"};

    fprintf(file, "SETTINGS:\n");
    fprintf(file, "GAME_MODE: %d-player\n", game->gameMode);

    if (game->gameMode == 1) {
        fprintf(file, "DIFFICULTY: %s\n", difficulties[game->difficulty]);
        fprintf(file, "USER_COLOR: %s\n", game->userColor ? "white" : "black");
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessSaveGame(SPChessGame* game, const char* file) {
    if (!game || !file) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    FILE* filePointer = fopen(file, "w"); // Overwrite any existing file of the same name
    if (!filePointer) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    fprintf(filePointer, "%s\n", game->currentPlayer ? "white" : "black");
    spFprintSettings(game, filePointer);
    spChessGameFprintBoard(game, filePointer);

    fclose(filePointer);

    return SP_CHESS_GAME_SUCCESS;
}

SPCommand spGetCommand() {
    char input[MAXIMUM_COMMAND_LENGTH + 1];
    fgets(input, MAXIMUM_COMMAND_LENGTH, stdin);
    return spParserParseLine(input);
}

/***
 * Converts from string to int representation of colors.
 * @param argument - String to convert to color.
 * @return 1 if argument is "white"
 *         0 if argument is "black"
 *        -1 otherwise
 */
int spGetColor(const char* argument) {
    if (!argument) {
        return -1;
    }

    if (!strcmp(argument, "white")) {
        return 1;
    }
    if (!strcmp(argument, "black")) {
        return 0;
    }
    return -1;
}

/***
 * Same as above, just for game mode
 * @param argument - Same
 * @return 1 if argument is "1-player"
 *         2 if argument is "2-player"
 *         0 otherwise
 */
int spGetGameMode(const char* argument) {
    if (!argument) {
        return 0;
    }

    if (!strcmp(argument, "1-player")) {
        return 1;
    }
    if (!strcmp(argument, "2-player")) {
        return 2;
    }

    return 0;
}

/***
 * Converts difficulty name to int representing same difficulty level.
 * @param argument - Same as above, sort of
 * @return 1 if argument is "amateur"
 *         2 if argument is "easy"
 *         3 if argument is "moderate"
 *         4 if argument is "hard"
 *         5 if argument is "expert"
 *         0 otherwise
 */
int spGetDifficulty(const char* argument) {
    if (!argument) {
        return 0;
    }

    if (!strcmp(argument, "amateur")) {
        return 1;
    }
    if (!strcmp(argument, "easy")) {
        return 2;
    }
    if (!strcmp(argument, "moderate")) {
        return 3;
    }
    if (!strcmp(argument, "hard")) {
        return 4;
    }
    if (!strcmp(argument, "expert")) {
        return 5;
    }

    return 0;
}

/***
 * Helper function, turns the first bufferLength bytes of buffer to 0.
 * @param buffer - Buffer to clear
 * @param bufferLength - How much of it to clear
 */
void clearBuffer(char* buffer, int bufferLength) {
    if (!buffer) {
        return;
    }

    for (int i = 0; i < bufferLength; i++) {
        buffer[i] = '\0';
    }
}

SP_CHESS_GAME_MESSAGE spChessLoadGame(SPChessGame* game, char* file) {
    if (!game || !file) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    FILE* filePointer = fopen(file, "r");
    if (!filePointer) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    for (int i = 0; i < N_COLUMNS; i++) {
        for (int j = 0; j < N_ROWS; j++) {
            if (j < 4) {
                game->locations[i + j * N_COLUMNS] = 0;
            }
            game->gameBoard[i][j] = 0;
        }
    }

    while (!spArrayListIsEmpty(game->history)) {
        if (spArrayListRemoveFirst(game->history) != SP_ARRAY_LIST_SUCCESS) {
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

    char buffer[MAX_FILE_LINE_LENGTH + 1];
    char* argument;
    int row = 0;
    char location;
    char piece;
    int finishedLoading = 0;
    while (!finishedLoading) {
        fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
        if (!row) {
            argument = strtok(buffer, DELIMITERS);
            game->currentPlayer = spGetColor(argument);
            clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
            fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
        }

        else if (row == 1) {
            argument = strtok(buffer, DELIMITERS);
            argument = strtok(NULL, DELIMITERS);
            game->gameMode = spGetGameMode(argument);
            if (game->gameMode == 1) {
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                argument = strtok(NULL, DELIMITERS);
                game->difficulty = spGetDifficulty(argument);
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                argument = strtok(NULL, DELIMITERS);
                game->userColor = spGetColor(argument);
            }
        }

        else if (row == 2) {
            for (int i = 0; i < N_ROWS; i++) {
                argument = strtok(buffer, DELIMITERS);
                for (int j = 0; j < N_COLUMNS; j++) {
                	location = 1 << 3;
                	location |= i;
                	location <<= 3;
                	location |= j;
                    argument = strtok(NULL, DELIMITERS);
                    piece = argument[0];
                    game->gameBoard[i][j] = piece == BLANK ? '\0' : piece;
                    if (piece == PAWN(BLACK)) {
                    	for (int i = 0; i < N_COLUMNS; i++) {
                    		if (!game->locations[i + N_COLUMNS]) {
                    			game->locations[i + N_COLUMNS] = location;
                    			break;
                    		}
                    	}
                    }
                    else if (piece == PAWN(WHITE)) {
                    	for (int i = 0; i < N_COLUMNS; i++) {
                    		if (!game->locations[i + 2 * N_COLUMNS]) {
                    			game->locations[i + 2 * N_COLUMNS] = location;
                    			break;
                    		}
                    	}
                    }

                    else if (piece == KING(BLACK)) {
                        game->locations[KING_LOC(BLACK)] = location;
                    }
                    else if (piece == KING(WHITE)) {
                        game->locations[KING_LOC(WHITE)] = location;
                    }

                    else if (piece == QUEEN(BLACK)) {
                    	game->locations[QUEEN_LOC(BLACK)] = location;
                    }
                    else if (piece == QUEEN(WHITE)) {
                    	game->locations[QUEEN_LOC(WHITE)] = location;
                    }

                    else if (piece == ROOK(BLACK)) {
                    	if (!game->locations[LEFT_ROOK_LOC(BLACK)]) {
                    		game->locations[LEFT_ROOK_LOC(BLACK)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_ROOK_LOC(BLACK)] = location;
                    	}
                    }
                    else if (piece == ROOK(WHITE)) {
                    	if (!game->locations[LEFT_ROOK_LOC(WHITE)]) {
                    		game->locations[LEFT_ROOK_LOC(WHITE)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_ROOK_LOC(WHITE)] = location;
                    	}
                    }

                    else if (piece == BISHOP(BLACK)) {
                    	if (!game->locations[LEFT_BISHOP_LOC(BLACK)]) {
                    		game->locations[LEFT_BISHOP_LOC(BLACK)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_BISHOP_LOC(BLACK)] = location;
                    	}
                    }
                    else if (piece == BISHOP(WHITE)) {
                    	if (!game->locations[LEFT_BISHOP_LOC(WHITE)]) {
                    		game->locations[LEFT_BISHOP_LOC(WHITE)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_BISHOP_LOC(WHITE)] = location;
                    	}
                    }

                    else if (piece == KNIGHT(BLACK)) {
                    	if (!game->locations[LEFT_KNIGHT_LOC(BLACK)]) {
                    		game->locations[LEFT_KNIGHT_LOC(BLACK)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_KNIGHT_LOC(BLACK)] = location;
                    	}
                    }
                    else if (piece == KNIGHT(WHITE)) {
                    	if (!game->locations[LEFT_KNIGHT_LOC(WHITE)]) {
                    		game->locations[LEFT_KNIGHT_LOC(WHITE)] = location;
                    	}
                    	else {
                    		game->locations[RIGHT_KNIGHT_LOC(WHITE)] = location;
                    	}
                    }
                }

                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
            }

            finishedLoading = 1;
        }

        clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
        row++;
    }

    fclose(filePointer);
    return spChessCheckGameState(game, game->currentPlayer);
}

SP_CHESS_GAME_MESSAGE spChessVerifyPositionAndPiece(SPChessGame* game, char position) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int row = spChessGameGetRowFromPosition(position);
    int column = spChessGameGetColumnFromPosition(position);
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLUMNS) {
        return SP_CHESS_GAME_INVALID_POSITION;
    }

    char piece = game->gameBoard[row][column];
    return piece ? SP_CHESS_GAME_SUCCESS : SP_CHESS_GAME_NO_PIECE_IN_POSITION;
}

void spPrintUndoneMove(int move, int color) {
    move <<= 8;
    char startingPosition = spChessGameGetCurrentPositionFromMove(move);
    int startingRow = 8 - spChessGameGetRowFromPosition(startingPosition);
    char startingColumn = 'A' + spChessGameGetColumnFromPosition(startingPosition);

    char finishPosition = spChessGameGetDestinationPositionFromMove(move);
    int finishRow = 8 - spChessGameGetRowFromPosition(finishPosition);
    char finishColumn = 'A' + spChessGameGetColumnFromPosition(finishPosition);
    printf("Undo move for %s player: <%d,%c> -> <%d,%c>\n", color ? "white" : "black", finishRow, finishColumn, startingRow, startingColumn);
}

void spPrintComputerMove(char piece, int move) {
    move <<= 8;
    char startingPosition = spChessGameGetCurrentPositionFromMove(move);
    int startingRow = 8 - spChessGameGetRowFromPosition(startingPosition);
    char startingColumn = 'A' + spChessGameGetColumnFromPosition(startingPosition);
    char finishPosition = spChessGameGetDestinationPositionFromMove(move);
    int finishRow = 8 - spChessGameGetRowFromPosition(finishPosition);
    char finishColumn = 'A' + spChessGameGetColumnFromPosition(finishPosition);

    printf("Computer: move ");

    if (piece == PAWN(BLACK) || piece == PAWN(WHITE)) {
        printf("pawn");
    }
    else if (piece == BISHOP(BLACK) || piece == BISHOP(WHITE)) {
        printf("bishop");
    }
    else if (piece == KNIGHT(BLACK) || piece == KNIGHT(WHITE)) {
        printf("knight");
    }
    else if (piece == ROOK(BLACK) || piece == ROOK(WHITE)) {
        printf("rook");
    }
    else if (piece == QUEEN(BLACK) || piece == QUEEN(WHITE)) {
        printf("queen");
    }
    else if (piece == KING(BLACK) || piece == KING(WHITE)) {
        printf("king");
    }
    else {
        printf("___");
    }

    printf(" at <%d,%c> to <%d,%c>\n", startingRow, startingColumn, finishRow, finishColumn);
}

char spChessGameGetPieceAtPosition(SPChessGame* game, char position) {
    if (!game) {
        return '\0';
    }

    int row = spChessGameGetRowFromPosition(position);
    int column = spChessGameGetColumnFromPosition(position);
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLUMNS) {
        return '\0';
    }

    return game->gameBoard[row][column];
}

SP_CHESS_GAME_MESSAGE spChessPrintMoves(SPArrayList* list) {
    if (!list) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int numOfMoves = spArrayListSize(list);
    int step;
    char location;
    int row;
    int column;

    for (int i = 0 ; i < numOfMoves; i++) {
        step = spArrayListGetAt(list, i);
        location = spChessGameGetDestinationPositionFromMove(step);
        row = spChessGameGetRowFromPosition(location);
        column = spChessGameGetColumnFromPosition(location);
        printf("<%d,%c>", 8 - row, 'A' + column);

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

unsigned int setMoveCoordinatesToInt(unsigned int currentRow,unsigned int currentColumn,unsigned int destinationRow, unsigned int destinationColumn) {
    unsigned int move = spChessGameSetLocation(currentRow, currentColumn) << 8; // format is 8 bits current location, 8 bits destination location
    move |= spChessGameSetLocation(destinationRow, destinationColumn);
    return move;
}

int spChessGameStepWillThreaten(unsigned int step) {
    return (step << 24) >> 28; //Get bits [4-7]
}

int spChessGameStepWillCapture(unsigned int step) {
    return (step << 28) >> 28; //Get 4 right most bits
}

unsigned char spChessGameSetLocation(int row, int column) {
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLUMNS) {
        return 0;
    }

    unsigned char location = 1 << 3;
    location = (row | location) << 3;
    location |= column;

    return location;
}
