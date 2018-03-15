//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <string.h>
#include "SPChessGame.h"
#include "SPChessParser.h"

#define MAX_FILE_LINE_LENGTH 100
#define DELIMITERS " \t\r\n"

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

SP_CHESS_GAME_MESSAGE spChessGameCheckPotentialThreat(SPChessGame* src, int move, char location) {
	if (!src) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int lastMove = 0;
	int full = 0;
	int threatened = 0;
	if (spArrayListIsFull(src->history)) {
		lastMove = spArrayListGetLast(src->history);
		full = 1;
	}

	SP_CHESS_GAME_MESSAGE gameMessage = spChessGameSetMove(src, move);
	if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	if (spChessGameIsPieceThreatened(src, location)) {
		threatened = 1;
	}

	gameMessage = spChessGameUndoMove(src);
	if (gameMessage == SP_CHESS_GAME_INVALID_ARGUMENT) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	if (full) {
		SP_ARRAY_LIST_MESSAGE message = spArrayListAddLast(src->history, lastMove);
		if (message != SP_ARRAY_LIST_SUCCESS) {
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

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

void spFprintSettings(SPChessGame* game, FILE* file) {
    if (!game || !file) {
        return;
    }
    fprintf(file, "SETTINGS:\n");
    fprintf(file, "GAME_MODE: %d-player\n", game->gameMode);
    if (game->gameMode == 1) {
        fprintf(file, "DIFFICULTY: %s\n", difficulties[game->difficulty]);
        fprintf(file, "USER_COLOR: %s\n", game->userColor ? "white" : "black");
    }
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
                    argument = strtok(NULL, DELIMITERS);
                    if (!argument) {
                        fclose(filePointer);
                        return SP_CHESS_GAME_INVALID_ARGUMENT;
                    }
                    piece = argument[0];
                    if (piece == PAWN || piece == CAPITAL_TO_LOW(PAWN) ||
                            piece == KING || piece == CAPITAL_TO_LOW(KING) ||
                            piece == QUEEN || piece == CAPITAL_TO_LOW(QUEEN) ||
                            piece == ROOK || piece == CAPITAL_TO_LOW(ROOK) ||
                            piece == BISHOP || piece == CAPITAL_TO_LOW(BISHOP) ||
                            piece == KNIGHT || piece == CAPITAL_TO_LOW(KNIGHT) ||
                            piece == BLANK) {
                        game->gameBoard[i][j] = piece;
                    }
                    else {
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