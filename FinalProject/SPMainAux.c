//
// Created by Roi Koren on 28/12/2017.
#include <string.h>
#include "SPChessGame.h"
#include "SPChessParser.h"
#include "SPMinimax.h"
#include "SPMainAux.h"

<<<<<<< HEAD
=======
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
>>>>>>> 49d9a918b6cbdaca26b214af83140b7319aa6696

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

<<<<<<< HEAD

SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move,char location) {
	SPChessGame* copy = spChessGameCopy(src);
	if (!copy) {
		return SP_CHESS_GAME_ALOCATION_ERROR;
	}
	spChessGameSetMove(copy,move);
	if (spChessGameIsPieceThreatened(copy,location)) {
		return SP_CHESS_GAME_MOVE_WILL_THREATEN;
	}
	spChessGameDestroy(copy);
	return SP_CHESS_GAME_SUCCESS;
}

int spRunGame(SPChessGame* src) {
=======
char spChessGameGetDestinationPositionFromMove(int move) {
    return (char) CLEAN_EXCESS_BYTES((move >> 8)); // Get 2nd byte from the right
}

char spChessGameGetCurrentPositionFromMove(int move) {
    return (char) CLEAN_EXCESS_BYTES((move >> 16)); // Get 3rd Byte from the left
}

int spChessGameGetColumnFromPosition(char position) {
    return (int) (position << 4) >> 4; // Get 4 rightmost bits
}

int spChessGameGetRowFromPosition(char position) {
    return (int) position >> 4; // Get 4 leftmost bits
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

SP_CHESS_GAME_MESSAGE spChessSaveGame(SPChessGame* game, char* file) {
    if (!game || !file) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    FILE* filePointer = fopen(file, "w");
    if (!filePointer) {
        return SP_CHESS_GAME_INVALID_ARGUMENT;
    }

    fprintf(filePointer, "%s\n", game->currentPlayer ? "white" : "black");
    spFprintSettings(game, filePointer);
    spChessGameFprintBoard(game, filePointer);

    fclose(filePointer);

    return SP_CHESS_GAME_SUCCESS;
}

int spGetColor(const char* argument) {
    if (!argument) {
        return -1;
    }
>>>>>>> 49d9a918b6cbdaca26b214af83140b7319aa6696

}
<<<<<<< HEAD
=======

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
>>>>>>> 49d9a918b6cbdaca26b214af83140b7319aa6696
