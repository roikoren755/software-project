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
    	printf("Exiting...\n");
    	return 0;
    }
    SPFiarGame* game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE);
    if (!game) {
        printf("Error: spFiarGameCreate has failed");
        return -1;
    }
    char input[MAXIMUM_COMMAND_LENGTH + 1];
    spFiarGamePrintBoard(game);
    char winner  = spFiarCheckWinner(game);;
    int success;
    printf("Please make the next move:\n");
    while (1) {
        fgets(input, MAXIMUM_COMMAND_LENGTH, stdin);
        SPCommand command = spParserPraseLine(input);
        if (command.cmd == SP_INVALID_LINE) {
            printf("Error: invalid command\n");
        }
        if (command.cmd == SP_SUGGEST_MOVE && !winner) {
        	success = spFiarGameSuggestMove(game, maxDepth);
            if (!success) {
            	return -1;
            }
        }
        if (command.cmd == SP_UNDO_MOVE) {
            spFiarGameUndoMove(game, winner);
        }
        if (command.cmd == SP_ADD_DISC && !winner) {
        	success = spFiarGameAddDisc(game, command, maxDepth);
        	if (!success) {
        		return -1;
        	}
        	winner = spFiarCheckWinner(game);
        	if (success == 1) {
        		if (!winner) {
        			printf("Please make the next move:\n");
        		}
        		else {
        			if (winner == SP_FIAR_GAME_TIE_SYMBOL) {
        				printf("Game over: it's a tie\nPlease enter 'quit' to exit or 'restart' to start a new game!\n");
        			}
        			else {
        				printf("Game over: %s\nPlease enter 'quit' to exit or 'restart' to start a new game!\n", winner == SP_FIAR_GAME_PLAYER_1_SYMBOL ? "you win" : "computer wins");
        			}
        		}
        	}
        }
        if (command.cmd == SP_QUIT) {
        	printf("Exiting...\n");
        	spFiarGameDestroy(game);
        	return 0;
        }
        if (command.cmd == SP_RESTART) {
        	maxDepth = spFiarGameRestart(&game);
        	if (!maxDepth) {
        		return 0;
        	}
        }
        if (winner && (command.cmd == SP_ADD_DISC || command.cmd == SP_SUGGEST_MOVE)) {
        	printf("Error: the game is over\n");
        }
    }
}
