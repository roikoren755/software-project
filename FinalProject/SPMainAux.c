//
// Created by Roi Koren on 28/12/2017.
#include <string.h>
#include "SPMainAux.h"

#define MAX_FILE_LINE_LENGTH 100
#define DELIMITERS " \t\r\n"
#define PAWN 'M'
#define KING 'K'
#define QUEEN 'Q'
#define BISHOP 'B'
#define ROOK 'R'
#define KNIGHT 'N'
#define BLANK '_'
#define CAPITAL_TO_LOW(c) c + 'a' - 'A'
#define KING_LOC(color) (4+color*3*N_COLUMNS)
#define QUEEN_LOC(color) (3+color*3*N_COLUMNS)
#define LEFT_ROOK_LOC(color) (0+color*3*N_COLUMNS)
#define RIGHT_ROOK_LOC(color) (7+color*3*N_COLUMNS)
#define LEFT_KNIGHT_LOC(color) (1+color*3*N_COLUMNS)
#define RIGHT_KNIGHT_LOC(color) (6+color*3*N_COLUMNS)
#define LEFT_BISHOP_LOC(color) (2+color*3*N_COLUMNS)
#define RIGHT_BISHOP_LOC(color) (5+color*3*N_COLUMNS)
#define CLEAN_EXCESS_BYTES(i) (i << 24) >> 24
#define CONSOLE 0
#define GUI 1
#define CHECK_COLOR(color,piece) ((1-2*color)*(piece) < (1-2*color)*('Z'))
#define PIECE_NAME_LENGTH 6

int min(int a,int b){
	if(a<b){return a;}
	else{return b;}
}

int max(int a,int b){
	if(a>b){return a;}
		else{return b;}
	}

//int abs(int a){
//	if(a>0){return a;}
//	else{return -a;}
//}

char spChessGameGetDestinationPositionFromMove(unsigned int move) {
    move >>= 8;
    move <<= 24;
    move >>= 24;
    return (char) move; // Get 2nd byte from the right
}

char spChessGameGetCurrentPositionFromMove(unsigned int move) {
    move >>= 16;
    move <<= 24;
    move >>= 24;
    return (char) move; // Get 3rd Byte from the left
}

unsigned int spChessGameGetColumnFromPosition(unsigned char position) {
    position <<= 4;
    position >>= 4;
    return (unsigned int) position; // Get 4 rightmost bits
}

unsigned int spChessGameGetRowFromPosition(unsigned char position) {
    position >>= 4;
    return (unsigned int) position; // Get 4 leftmost bits
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

    FILE* filePointer = fopen(file, "w");
    if (!filePointer) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    fprintf(filePointer, "%s\n", game->currentPlayer ? "white" : "black");
    SP_CHESS_GAME_MESSAGE message = spFprintSettings(game, filePointer);
    if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
        fclose(filePointer);
        return message;
    }
    spChessGameFprintBoard(game, filePointer);

    fclose(filePointer);

    return SP_CHESS_GAME_SUCCESS;
}

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

    char buffer[MAX_FILE_LINE_LENGTH + 1];
    char* argument;
    int row = 0;
    int tooManyPieces = 0;
    char location;
    while (fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer)) {
        if (!row) {
            argument = strtok(buffer, DELIMITERS);
            game->currentPlayer = spGetColor(argument);
            if (game->currentPlayer == -1 || strtok(buffer, DELIMITERS)) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
            clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
            fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
            argument = strtok(buffer, DELIMITERS);
            if (!argument || strcmp(argument, "SETTINGS:") || strtok(buffer, DELIMITERS)) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
        else if (row == 1) {
            argument = strtok(buffer, DELIMITERS);
            if (!argument || strcmp(argument, "GAME_MODE:")) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
            argument = strtok(NULL, DELIMITERS);
            game->gameMode = spGetGameMode(argument);
            if (!game->gameMode || strtok(NULL, DELIMITERS)) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
            if (game->gameMode == 1) {
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                if (!argument || strcmp(argument, "DIFFICULTY:")) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
                argument = strtok(NULL, DELIMITERS);
                game->difficulty = spGetDifficulty(argument);
                if (!game->difficulty) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
                argument = strtok(buffer, DELIMITERS);
                if (!argument || strcmp(argument, "USER_COLOR:")) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
                argument = strtok(NULL, DELIMITERS);
                game->userColor = spGetColor(argument);
                if (game->userColor == -1) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
            }
        }
        else if (row == 2) {
            char piece;
            for (int i = 0; i < N_ROWS; i++) {
                char line[MAX_FILE_LINE_LENGTH];
                sprintf(line, "%d|", N_ROWS - i);
                argument = strtok(buffer, DELIMITERS);
                if (!argument || strcmp(argument, line)) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
                for (int j = 0; j < N_COLUMNS; j++) {
                	location = 0;
                	location |= i;
                	location <<= 4;
                	location |= j;
                    argument = strtok(NULL, DELIMITERS);
                    if (!argument) {
                        fclose(filePointer);
                        return SP_CHESS_GAME_INVALID_ARGUMENT;
                    }
                    piece = argument[0];
                    game->gameBoard[i][j] = piece;
                    if (piece == PAWN) {
                    	tooManyPieces = 1;
                    	for (int i = 0; i < N_COLUMNS; i++) {
                    		if (!game->locations[i + N_COLUMNS]) {
                    			game->locations[i + N_COLUMNS] = location;
                    			tooManyPieces = 0;
                    			break;
                    		}
                    	}
                    	if (tooManyPieces) {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == CAPITAL_TO_LOW(PAWN)) {
                    	tooManyPieces = 1;
                    	for (int i = 0; i < N_COLUMNS; i++) {
                    		if (!game->locations[i + N_COLUMNS]) {
                    			game->locations[i + N_COLUMNS] = location;
                    			tooManyPieces = 0;
                    			break;
                    		}
                    	}
                    	if (tooManyPieces) {
                    		fclose(filePointer);
                    	    return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == KING) {
                        if (game->locations[KING_LOC(BLACK)]) {
                        	fclose(filePointer);
                            return SP_CHESS_GAME_INVALID_ARGUMENT;
                        }
                        game->locations[KING_LOC(BLACK)] = location;
                    }
                    else if (piece == CAPITAL_TO_LOW(KING)) {
                        if (game->locations[KING_LOC(WHITE)]) {
                        	fclose(filePointer);
                            return SP_CHESS_GAME_INVALID_ARGUMENT;
                        }
                        game->locations[KING_LOC(WHITE)] = location;
                    }
                    else if (piece == QUEEN) {
                    	if (game->locations[QUEEN_LOC(BLACK)]) {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == CAPITAL_TO_LOW(QUEEN)) {
                    	if (game->locations[QUEEN_LOC(WHITE)]) {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == ROOK) {
                    	if (!game->locations[LEFT_ROOK_LOC(BLACK)]) {
                    		game->locations[LEFT_ROOK_LOC(BLACK)] = location;
                    	}
                    	else if (!game->locations[RIGHT_ROOK_LOC(BLACK)]) {
                    		game->locations[LEFT_ROOK_LOC(BLACK)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == CAPITAL_TO_LOW(ROOK)) {
                    	if (!game->locations[LEFT_ROOK_LOC(WHITE)]) {
                    		game->locations[LEFT_ROOK_LOC(WHITE)] = location;
                    	}
                    	else if (!game->locations[RIGHT_ROOK_LOC(WHITE)]) {
                    		game->locations[LEFT_ROOK_LOC(WHITE)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == BISHOP) {
                    	if (!game->locations[LEFT_BISHOP_LOC(BLACK)]) {
                    		game->locations[LEFT_BISHOP_LOC(BLACK)] = location;
                    	}
                    	else if (!game->locations[RIGHT_BISHOP_LOC(BLACK)]) {
                    		game->locations[LEFT_BISHOP_LOC(BLACK)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == CAPITAL_TO_LOW(BISHOP)) {
                    	if (!game->locations[LEFT_BISHOP_LOC(WHITE)]) {
                    		game->locations[LEFT_BISHOP_LOC(WHITE)] = location;
                    	}
                    	else if (!game->locations[RIGHT_BISHOP_LOC(WHITE)]) {
                    		game->locations[LEFT_BISHOP_LOC(WHITE)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == KNIGHT) {
                    	if (!game->locations[LEFT_KNIGHT_LOC(BLACK)]) {
                    		game->locations[LEFT_KNIGHT_LOC(BLACK)] = location;
                    	}
                    	else if (!game->locations[RIGHT_KNIGHT_LOC(BLACK)]) {
                    		game->locations[LEFT_KNIGHT_LOC(BLACK)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece == CAPITAL_TO_LOW(KNIGHT)) {
                    	if (!game->locations[LEFT_KNIGHT_LOC(WHITE)]) {
                    		game->locations[LEFT_KNIGHT_LOC(WHITE)] = location;
                    	}
                    	else if (!game->locations[RIGHT_KNIGHT_LOC(WHITE)]) {
                    		game->locations[LEFT_KNIGHT_LOC(WHITE)] = location;
                    	}
                    	else {
                    		fclose(filePointer);
                    		return SP_CHESS_GAME_INVALID_ARGUMENT;
                    	}
                    }
                    else if (piece != BLANK) {
                    	fclose(filePointer);
                    	return SP_CHESS_GAME_INVALID_ARGUMENT;
                    }
                }
                argument = strtok(NULL, DELIMITERS);
                if (!argument || strcmp(argument, "|") || strtok(NULL, DELIMITERS)) {
                    fclose(filePointer);
                    return SP_CHESS_GAME_INVALID_ARGUMENT;
                }
                clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
                fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
            }

            if (strcmp(buffer, "  -----------------\n")) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }

            clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
            fgets(buffer, MAX_FILE_LINE_LENGTH, filePointer);
            if (strcmp(buffer, "   A B C D E F G H\n")) {
                fclose(filePointer);
                return SP_CHESS_GAME_INVALID_ARGUMENT;
            }
        }
        else {
            fclose(filePointer);
            return SP_CHESS_GAME_INVALID_ARGUMENT;
        }
        clearBuffer(buffer, MAX_FILE_LINE_LENGTH + 1);
        row++;
    }

    fclose(filePointer);
    return SP_CHESS_GAME_SUCCESS;
}

SPCommand spGetCommand(int mode) {
    char input[MAXIMUM_COMMAND_LENGTH + 1];
    if (mode == CONSOLE) {
        fgets(input, MAXIMUM_COMMAND_LENGTH, stdin);
    }
    return spParserParseLine(input);
}

SP_CHESS_GAME_MESSAGE spChessVerifyPositionAndPiece(SPChessGame* game, char position) {
    if (!game) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    int row = spChessGameGetRowFromPosition(position);
    int column = spChessGameGetColumnFromPosition(position);
    if (row >= 0 && row < N_ROWS && column >= 0 && column < N_COLUMNS) {
        return SP_CHESS_GAME_INVALID_POSITION;
    }

    char piece = game->gameBoard[row][column];
    return CHECK_COLOR(game->currentPlayer, piece) ? SP_CHESS_GAME_SUCCESS : SP_CHESS_GAME_NO_PIECE_IN_POSITION;
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

    if (piece == PAWN || piece == CAPITAL_TO_LOW(PAWN)) {
        printf("pawn");
    }
    else if (piece == BISHOP || piece == CAPITAL_TO_LOW(BISHOP)) {
        printf("bishop");
    }
    else if (piece == KNIGHT || piece == CAPITAL_TO_LOW(KNIGHT)) {
        printf("knight");
    }
    else if (piece == ROOK || piece == CAPITAL_TO_LOW(ROOK)) {
        printf("rook");
    }
    else if (piece == QUEEN || piece == CAPITAL_TO_LOW(QUEEN)) {
        printf("queen");
    }
    else if (piece == KING || piece == CAPITAL_TO_LOW(KING)) {
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
