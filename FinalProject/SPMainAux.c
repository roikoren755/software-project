//
// Created by Roi Koren on 28/12/2017.
#include "SPMainAux.h"
#include <string.h>
#include "Button.h"
#include "SPChessGameSdl.h"
#include "SPChessMainSdl.h"
#include "SPLoadSaveGameSdl.h"
#include "SPMinimax.h"

#define MAXIMUM_COMMAND_LENGTH 1024
#define MAX_FILE_LINE_LENGTH 100
#define DELIMITERS " \t\r\n"
#define GET_COLOR(color) color ? "white" : "black"

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

int runSdl(SPChessGame* game) {
	// initialize SDL2 for video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	int done = 0;
	int feedback;
	int i;
	int j;

	// create screens
	Screen* screens[NUM_SCREENS] = { NULL };
	int success = spGameCreateScreens(screens);
	if (!success) {
		done = 1;
	}

	SDL_Event event;
	while (!done) {
		SDL_WaitEvent(&event);
		feedback = NONE;
			// loop over the shown screen's widgets
			for(i = 0; i < NUM_SCREENS; i++) {
				if (screens[i]->shown) {
					for(j = 0; j < screens[i]->widgetsSize; j++) {
						if (screens[i]->widgets[j]) {
							feedback = screens[i]->widgets[j]->handleEvent(screens[i]->widgets[j], &event, screens, game,
																		   i, j);
						}

						if (feedback == PRESSED) {
							break;
						}

						if (feedback == QUIT) {
							done = 1;
							break;
						}
					}
					break;
				}
			}

		for(i = 0; i < NUM_SCREENS; i++) {
			if (screens[i]->shown) {
				screens[i]->draw(screens[i], i);
				break;
			}
		}
	}

	spChessGameDestroy(game);
	spDestroyScreensArr(screens, NUM_SCREENS);
	SDL_Quit();

	return 0;
}

int runConsole(SPChessGame* game) {
	spChessPrintGameTitle();

	char* difficulties[] = {"", "amateur", "easy", "moderate", "hard", "expert"};
	int quit = 0;
	int settings = 1;
	int printedWelcome = 0;
	int gameStarted = 0;
	int moveMade = 0;
	SP_CHESS_GAME_MESSAGE message;
	SPCommand command;
	while (!quit) {
		if (settings) {
			if (!printedWelcome) {
				printf("Specify game settings or type 'start' to begin a game with the current settings:\n");
				printedWelcome = 1;
				gameStarted = 0;
			}
			command = spGetCommand();

			if (command.cmd == SP_GAME_MODE) {
				spChessGameSetGameMode(game, &command);
			}

			else if (command.cmd == SP_DIFFICULTY && game->gameMode == 1) {
				spChessGameSetDifficulty(game, &command, difficulties);
			}

			else if (command.cmd == SP_USER_COLOR && game->gameMode == 1) {
				spChessGameSetUserColor(game, &command);
			}

			else if (command.cmd == SP_LOAD) {
				message = spChessGameLoadGame(game, &command);
				switch (message) {
					case SP_CHESS_GAME_CHECK:
						printf("Check: %s king is threatened\n", GET_COLOR(game->currentPlayer));
					case SP_CHESS_GAME_SUCCESS:
						gameStarted = 1;
						break;
					case SP_CHESS_GAME_CHECKMATE:
						printf("Checkmate! %s player wins the game\n", GET_COLOR(!game->currentPlayer));
						quit = 1;
						break;
					case SP_CHESS_GAME_DRAW:
						printf("The game ends in a draw\n");
						quit = 1;
						break;
					default:
						break;
				}
			}

			else if (command.cmd == SP_DEFAULT) {
				spChessGameResetSettings(game);
			}

			else if (command.cmd == SP_PRINT_SETTINGS) {
				spFprintSettings(game, stdout);
			}

			else if (command.cmd == SP_QUIT) {
				printf("Exiting...\n");
				quit = 1;
			}

			else if (command.cmd == SP_START) {
				printf("Starting game...\n");
				settings = 0;
			}

			else {
				printf("ERROR: invalid command\n");
			}
		}

		else {
			if (game->gameMode == 1 && !gameStarted && game->currentPlayer != game->userColor &&
				(game->gameState == CHECK || game->gameState == NORMAL)) {
				gameStarted = 1;
				int move = spMinimaxSuggestMove(game);
				if (move < 1) {
					printf("ERROR: Couldn't figure out computer move.\n");
				}
				else {
					char piece = spChessGameGetPieceAtPosition(game, spChessGameGetCurrentPositionFromMove(move << 8));
					spPrintComputerMove(piece, move);
					spChessGameSetMove(game, move);
					moveMade = 1;
				}
			}
			if (game->gameState == CHECKMATE) {
				printf("Checkmate! %s player wins the game\n", GET_COLOR(!game->currentPlayer));
				quit = 1;
			}
			else if (game->gameState == DRAW) {
				printf("The game ends in a draw\n");
				quit = 1;
			}
			if (!gameStarted) {
				gameStarted = 1;
				spChessGamePrintBoard(game);
			}

			if (moveMade) {
				spChessGamePrintBoard(game);
				moveMade = 0;
			}
			printf("Enter your move (%s player):\n", GET_COLOR(game->currentPlayer));
			if (!quit) {
				command = spGetCommand();
			}

			if (command.cmd == SP_MOVE) {
				message = spChessGameMove(game, &command);
				switch (message) {
					case SP_CHESS_GAME_CHECKMATE:
					case SP_CHESS_GAME_DRAW:
						quit = 1;
					case SP_CHESS_GAME_SUCCESS:
					case SP_CHESS_GAME_CHECK:
						moveMade = 1;
					default:
						break;
				}
			}

			else if (command.cmd == SP_GET_MOVES) {
				spChessGetMoves(game, &command);
			}

			else if (command.cmd == SP_SAVE) {
				message = spChessSaveGame(game, command.arguments);
				if (message == SP_CHESS_GAME_SUCCESS) {
					printf("Game saved to: %s\n", command.arguments);
				}
				else {
					printf("File cannot be created or modified\n");
				}
			}

			else if (command.cmd == SP_UNDO) {
				message = spChessGameUndo(game);
				switch (message) {
					case SP_CHESS_GAME_INVALID_ARGUMENT:
						quit = 1;
						break;
					case SP_CHESS_GAME_NO_HISTORY:
						if (game->gameMode == 1 && game->currentPlayer != game->userColor) {
							gameStarted = 0;
						}
					default:
						break;
				}
			}

			else if (command.cmd == SP_RESET) {
				settings = 1;
				printedWelcome = 0;
				message = spChessGameResetGame(game);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					printf("ERROR: Something went awry while resetting game!\n");
				}
				else {
					printf("Restarting...\n");
				}
			}

			else if (command.cmd == SP_QUIT) {
				printf("Exiting...\n");
				quit = 1;
			}

			else {
				printf("ERROR: invalid command\n");
			}
		}
	}

	spChessGameDestroy(game);

	return 0;
}

int min(int a, int b) {
	return a < b ? a : b;
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

SP_CHESS_GAME_MESSAGE spChessLoadGame(SPChessGame* game, char* file) {
    if (!game || !file) { // Oops
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    FILE* filePointer = fopen(file, "r");
    if (!filePointer) { // Oops
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    for (int i = 0; i < N_COLUMNS; i++) { // Clear board and locations
        for (int j = 0; j < N_ROWS; j++) {
            if (j < 4) {
                game->locations[i + j * N_COLUMNS] = 0;
            }
            game->gameBoard[i][j] = 0;
        }
    }

    while (!spArrayListIsEmpty(game->history)) { // Clear history
        if (spArrayListRemoveFirst(game->history) != SP_ARRAY_LIST_SUCCESS) {
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
    }

	game->userColor = WHITE; // Reset settings to default, to start with.
	game->gameMode = 1;
	game->difficulty = 2;

    char buffer[MAX_FILE_LINE_LENGTH + 1]; // Preparations
    char* argument;
    int row = 0;
    char location;
    char piece;
    int finishedLoading = 0;
    while (!finishedLoading) {
        fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
        if (!row) { // First section
            argument = strtok(buffer, DELIMITERS);
            game->currentPlayer = spGetColor(argument); // Set current player
            clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
            fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
        }

        else if (row == 1) { // Settings section
            argument = strtok(buffer, DELIMITERS);
            argument = strtok(NULL, DELIMITERS);
            game->gameMode = spGetGameMode(argument); // Game mode
            if (game->gameMode == 1) { // Single player?
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                argument = strtok(NULL, DELIMITERS);
                game->difficulty = spGetDifficulty(argument); // Set difficulty
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                argument = strtok(NULL, DELIMITERS);
                game->userColor = spGetColor(argument); // User color
            }
        }

        else if (row == 2) { // Board section
            for (int i = 0; i < N_ROWS; i++) {
                argument = strtok(buffer, DELIMITERS);
                for (int j = 0; j < N_COLUMNS; j++) {
                	location = 1 << 3;
                	location |= i;
                	location <<= 3;
                	location |= j; // Prepare location for game->locations
                    argument = strtok(NULL, DELIMITERS);
                    piece = argument[0];
                    game->gameBoard[i][j] = piece == BLANK ? '\0' : piece; // Set piece on board
                    if (piece == PAWN(BLACK)) { // Find correct piece and put location in game->locations
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

            finishedLoading = 1; // Finished
        }

        clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
        row++;
    }

    fclose(filePointer);
    return spChessCheckGameState(game, game->currentPlayer); // Update game state
}

SP_CHESS_GAME_MESSAGE spChessVerifyPositionAndPiece(SPChessGame* game, char position) {
    if (!game) { // Oops
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int row = spChessGameGetRowFromPosition(position);
    int column = spChessGameGetColumnFromPosition(position);
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLUMNS) { // Out of bounds?
        return SP_CHESS_GAME_INVALID_POSITION;
    }

    char piece = game->gameBoard[row][column];
    return piece ? SP_CHESS_GAME_SUCCESS : SP_CHESS_GAME_NO_PIECE_IN_POSITION; // Is there a piece there?
}

void spPrintUndoneMove(int move, int color) {
    move <<= 8;
    char startingPosition = spChessGameGetCurrentPositionFromMove(move); // User-friendly display of locations
    int startingRow = 8 - spChessGameGetRowFromPosition(startingPosition);
    char startingColumn = 'A' + spChessGameGetColumnFromPosition(startingPosition);

    char finishPosition = spChessGameGetDestinationPositionFromMove(move);
    int finishRow = 8 - spChessGameGetRowFromPosition(finishPosition);
    char finishColumn = 'A' + spChessGameGetColumnFromPosition(finishPosition);
    printf("Undo move for %s player: <%d,%c> -> <%d,%c>\n", color ? "white" : "black", finishRow, finishColumn,
		   startingRow, startingColumn);
}

void spPrintComputerMove(char piece, int move) {
    move <<= 8; // Pretty print computer's move
    char startingPosition = spChessGameGetCurrentPositionFromMove(move);
    int startingRow = 8 - spChessGameGetRowFromPosition(startingPosition);
    char startingColumn = 'A' + spChessGameGetColumnFromPosition(startingPosition);
    char finishPosition = spChessGameGetDestinationPositionFromMove(move);
    int finishRow = 8 - spChessGameGetRowFromPosition(finishPosition);
    char finishColumn = 'A' + spChessGameGetColumnFromPosition(finishPosition);

    printf("Computer: move ");
    switch (piece) {
		case PAWN(BLACK):
		case PAWN(WHITE):
			printf("pawn");
			break;
		case BISHOP(BLACK):
		case BISHOP(WHITE):
			printf("bishop");
			break;
		case KNIGHT(BLACK):
		case KNIGHT(WHITE):
			printf("knight");
			break;
		case ROOK(BLACK):
		case ROOK(WHITE):
			printf("rook");
			break;
		case QUEEN(BLACK):
		case QUEEN(WHITE):
			printf("queen");
			break;
		case KING(BLACK):
		case KING(WHITE):
			printf("king");
			break;
		default:
			printf("___");
    }

    printf(" at <%d,%c> to <%d,%c>\n", startingRow, startingColumn, finishRow, finishColumn);
}

char spChessGameGetPieceAtPosition(SPChessGame* game, char position) {
    if (!game) { // Oops
        return '\0';
    }

    int row = spChessGameGetRowFromPosition(position);
    int column = spChessGameGetColumnFromPosition(position);
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLUMNS) {
        return '\0'; // Oops
    }

    return game->gameBoard[row][column]; // Oops?
}

SP_CHESS_GAME_MESSAGE spChessPrintMoves(SPArrayList* list) {
    if (!list) { // Nope
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
        printf("<%d,%c>", 8 - row, 'A' + column); // Print each move in the correct format.

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

unsigned int setMoveCoordinatesToInt(unsigned int currentRow,unsigned int currentColumn,unsigned int destinationRow,
									 unsigned int destinationColumn) {
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

void spChessPrintGameTitle() {
	printf(" Chess\n");
	for (int i = 0; i < 7; i++) {
		printf("-");
	}
	printf("\n");
}

void spChessGameSetGameMode(SPChessGame* game, SPCommand* command) {
	if (!game || !command || command->cmd != SP_GAME_MODE) {
		printf("ERROR: Could not set game mode.\n");
		return;
	}

	int gameMode = spParserGetNonNegativeInt(command);
	switch (gameMode) {
		case 1:
		case 2:
			game->gameMode = gameMode;
			printf("Game mode is set to %d-player\n", gameMode);
			break;
		default:
			printf("Wrong game mode\n");
	}
}

void spChessGameSetDifficulty(SPChessGame* game, SPCommand* command, char** difficulties) {
	if (!game || !command || command->cmd != SP_DIFFICULTY || !difficulties) {
		printf("ERROR: Could not set difficulty.\n");
		return;
	}

	int difficulty = spParserGetNonNegativeInt(command);
	if (difficulty >= 1 && difficulty <= 5) {
		game->difficulty = difficulty;
		printf("Difficulty level is set to %s\n", difficulties[difficulty]);
	}
	else {
		printf("Wrong difficulty level. The value should be between 1 to 5\n");
	}
}

void spChessGameSetUserColor(SPChessGame* game, SPCommand* command) {
	if (!game || !command || command->cmd != SP_USER_COLOR) {
		printf("ERROR: Couldn't set user color.\n");
		return;
	}

	int userColor = spParserGetNonNegativeInt(command);
	switch (userColor) {
		case 0:
		case 1:
			game->userColor = userColor;
			printf("User color is set to %s\n", GET_COLOR(userColor));
			break;
		default:
			printf("Wrong user color. The value should be 0 or 1\n");
	}
}

SP_CHESS_GAME_MESSAGE spChessGameLoadGame(SPChessGame* game, SPCommand* command) {
	if (!game || !command || command->cmd != SP_LOAD) {
		printf("Error: Could not find game to load to, or the command to do so! Quitting...\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	SP_CHESS_GAME_MESSAGE message = spChessLoadGame(game, command->arguments);
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("Error: File doesn't exist or cannot be opened\n");
	}

	return message;
}

void spChessGameResetSettings(SPChessGame* game) {
	if (!game) {
		printf("ERROR: Could not reset game settings.\n");
		return;
	}

	game->userColor = WHITE;
	game->gameMode = 1;
	game->difficulty = 2;
	printf("All settings reset to default\n");
}

SP_CHESS_GAME_MESSAGE spChessGameMove(SPChessGame* game, SPCommand* command) {
	if (!game || !command || command->cmd != SP_MOVE) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int move = spParserGetMove(command); // Get move
	SP_CHESS_GAME_MESSAGE message = spChessGameIsValidMove(game, move); // Check validity
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("ERROR: Something went wrong!\n");
	}
	else if (message == SP_CHESS_GAME_INVALID_POSITION) {
		printf("Invalid position on the board\n");
	}
	else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
		printf("The specified position does not contain your piece\n");
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE) {
		printf("Illegal move\n");
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED) {
		printf("Illegal move: king is still threatened\n");
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_KING_BECOMES_THREATENED) {
		printf("Illegal move: king will be threatened\n");
	}
	else { // Legal move
		message = spChessGameSetMove(game, move); // Make it
		if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
			printf("ERROR: OOPS... something went wrong while setting up a move!\n");
		}
		else { // Great success
			message = spChessCheckGameState(game, game->currentPlayer); // Check if game is over
			switch (message) {
				case SP_CHESS_GAME_CHECK: // Check
					printf("Check: %s king is threatened\n", GET_COLOR(game->currentPlayer));
				case SP_CHESS_GAME_SUCCESS: // Or normal - get and make computer move
					if (game->gameMode == 1) {
						move = spMinimaxSuggestMove(game);
						if (move < 1) {
							printf("ERROR: Couldn't figure out computer move.\n");
							return SP_CHESS_GAME_INVALID_ARGUMENT;
						}
						else {
							char piece = spChessGameGetPieceAtPosition(game, spChessGameGetCurrentPositionFromMove(move << 8));
							spPrintComputerMove(piece, move);
							spChessGameSetMove(game, move);
							message = spChessCheckGameState(game, game->currentPlayer);
							switch (message) {
								case SP_CHESS_GAME_CHECKMATE:
									printf("Checkmate! %s player wins the game\n", GET_COLOR(!game->currentPlayer));
									break;
								case SP_CHESS_GAME_DRAW:
									printf("The game ends in a draw\n");
									break;
								case SP_CHESS_GAME_CHECK:
									printf("Check: %s king is threatened\n", GET_COLOR(game->currentPlayer));
									break;
								case SP_CHESS_GAME_INVALID_ARGUMENT:
									printf("ERROR: Something went wrong while checking game state.\n");
								default:
									break;
							}
						}
					}
					break;
				case SP_CHESS_GAME_CHECKMATE: // Checkmate! Take THAT!
					printf("Checkmate! %s player wins the game\n", GET_COLOR(!game->currentPlayer));
					break;
				case SP_CHESS_GAME_DRAW: // The best man won. Wait, what?
					printf("The game ends in a draw\n");
					break;
				default:
					printf("ERROR: Something went wrong while checking game state.\n");
					break;
			}
		}
	}

	return message;
}

void spChessGetMoves(SPChessGame* game, SPCommand* command) {
	if (!game || !command || command->cmd != SP_GET_MOVES) {
		printf("ERROR: Could not find game (or command) to get moves!\n");
		return;
	}
	char location = spParserGetLocation(command);
	SP_CHESS_GAME_MESSAGE message = spChessVerifyPositionAndPiece(game, location); // Check position validity
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("ERROR: Couldn't verify position and piece.\n");
	}
	else if (message == SP_CHESS_GAME_INVALID_POSITION) {
		printf("Invalid position on the board\n");
	}
	else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
		printf("The specified position does not contain a player piece\n");
	}
	else {
		SPArrayList* possibleMoves = spChessGameGetMoves(game, location); // Get moves
		message = spChessPrintMoves(possibleMoves); // Print 'em all
		spArrayListDestroy(possibleMoves); // Bye bye
	}
}

SP_CHESS_GAME_MESSAGE spChessGameUndo(SPChessGame* game) {
	if (!game) {
		printf("ERROR: No game passed to undo move in!\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	int move = spChessGameGetLastMovePlayed(game); // Get last move
	SP_CHESS_GAME_MESSAGE message = spChessGameUndoMove(game); // Undo it
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) { // Oops
		printf("ERROR: Something went wrong while trying to undo move.\n");
	}
	else if (message == SP_CHESS_GAME_NO_HISTORY) { // What move?
		printf("Empty history, no move to undo\n");
	}
	else {
		spPrintUndoneMove(move, game->currentPlayer); // Print move
		move = spChessGameGetLastMovePlayed(game); // 2 undo's!
		message = spChessGameUndoMove(game);
		if (message == SP_CHESS_GAME_INVALID_ARGUMENT) { // Oops
			printf("ERROR: Couldn't undo second move!\n");
		}
		else if (message == SP_CHESS_GAME_SUCCESS) { // Was there a move?
			spPrintUndoneMove(move, game->currentPlayer);
		}
		spChessGamePrintBoard(game);
	}

	return message;
}
