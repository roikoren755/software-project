//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include <stdlib.h>
#include "SPFIARParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"
#include "SPBufferset.h"

#define UNDO_MOVES_POSSIBLE 10
#define MAXIMUM_COMMAND_LENGTH 1024

int main() {
	SP_BUFF_SET();
    int maxDepth = spGetDifficulty();
    if (!maxDepth) {
    	printf("Error: spGetDifficulty has failed");
    	return -1;
    }
    if (maxDepth == -1) {
    	printf("Exiting...\n");
    	return 0;
    }
    SPFiarGame* game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE);
    if (!game) {
        printf("Error: spFiarGameCreate has failed");
        return -1;
    }
    char winner;
    char* input = malloc((MAXIMUM_COMMAND_LENGTH + 1) * sizeof(char));
    if (!input) {
        printf("Error: main has failed");
        spFiarGameDestroy(game);
        return -1;
    }
    spFiarGamePrintBoard(game);
    int ended = 0;
    while (1) {
        winner  = spFiarCheckWinner(game);
        if (!winner) {
            printf("Please make the next move:\n");
        }
        else {
            ended = 1;
            if (winner == SP_FIAR_GAME_TIE_SYMBOL) {
                printf("Game over: it's a tie\nPlease enter 'quit' to exit or 'restart' to start a new game!\n");
            }
            else {
                printf("Game over: %s\nPlease enter 'quit' to exit or 'restart' to start a new game!\n", winner == SP_FIAR_GAME_PLAYER_1_SYMBOL ? "you win" : "computer wins");
            }
        };
        fgets(input, MAXIMUM_COMMAND_LENGTH, stdin);
        SPCommand command = spParserPraseLine(input);
        if (command.cmd == SP_INVALID_LINE) {
            printf("Error: invalid command\n");
        }
        if (command.cmd == SP_SUGGEST_MOVE && !ended) {
            int suggestedMove = spMinimaxSuggestMove(game, maxDepth);
            if (suggestedMove == -1) {
                printf("Error: spMinimaxSuggestMove has failed");
                free(input);
                spFiarGameDestroy(game);
                return -1;
            }
            printf("Suggested move: drop a disc to column %d\n", suggestedMove + 1);
        }
        if (command.cmd == SP_UNDO_MOVE) {
            if (spArrayListIsEmpty(game->history)) {
                printf("Error: cannot undo previous move!\n");
            }
            else {
                if (!ended || winner != SP_FIAR_GAME_PLAYER_1_SYMBOL) {
                    printf("Remove disc: remove computer's disc at column %d\n", spFiarGameGetLastMovePlayed(game) + 1);
                    spFiarGameUndoPrevMove(game);
                }
                printf("Remove disc: remove user's disc at column %d\n", spFiarGameGetLastMovePlayed(game) + 1);
                spFiarGameUndoPrevMove(game);
            }
            spFiarGamePrintBoard(game);
        }
        if (command.cmd == SP_ADD_DISC && !ended) {
            if (!command.validArg) {
                printf("Error: invalid command\n");
            }
            if (command.arg < 1 || command.arg > SP_FIAR_GAME_N_COLUMNS) {
            	printf("Error: column number must be in range 1-7\n");
            }
            else {
                int col = command.arg - 1;
                if (!spFiarGameIsValidMove(game, col)) {
                    printf("Error: column %d is full\n", command.arg);
                }
                else {
                    spFiarGameSetMove(game, col);
                    int computerCol = spMinimaxSuggestMove(game, maxDepth);
                    if (computerCol == -1) {
                    	printf("Error: spMinimaxSuggestMove has failed");
                    	free(input);
                    	spFiarGameDestroy(game);
                    	return -1;
                    }
                    printf("Computer move: add disc to column %d\n", computerCol + 1);
                    spFiarGameSetMove(game, computerCol);
                }
                spFiarGamePrintBoard(game);
            }
        }
        if (command.cmd == SP_QUIT) {
        	printf("Exiting...\n");
        	free(input);
        	spFiarGameDestroy(game);
        	return 0;
        }
        if (command.cmd == SP_RESTART) {
        	spFiarGameDestroy(game);
        	game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE);
        	ended = 0;
        }
        if (command.cmd == SP_INVALID_LINE) {
        	printf("Error: invalid command\n");
        }
        if (ended && (command.cmd == SP_ADD_DISC || command.cmd == SP_SUGGEST_MOVE)) {
        	printf("Error: the game is over\n");
        }
    }
    free(input);
    spFiarGameDestroy(game);
    return 0;
}
