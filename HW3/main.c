//
// Created by Roi Koren on 28/12/2017.
//
#include <stdio.h>
#include "SPFIARGame.h"
#include "SPFIARParser.h"
#include "SPMainAux.h"
#include "SPMinimax.h"

#define UNDO_MOVES_POSSIBLE 10
#define MAXIMUM_COMMAND_LENGTH 1024

int main() {
    int maxDepth = spGetDifficulty();
    SPFiarGame game = spFiarGameCreate(2 * UNDO_MOVES_POSSIBLE);
    if (!game) {
        printf("Error: spFiarGameCreate has failed");
        return;
    }
    char winner;
    char* input = malloc((MAXIMUM_COMMAND_LENGTH + 1) * sizeof(char));
    if (!input) {
        printf("Error: main has failed");
        spFiarGameDestroy(game);
        return;
    }
    int ended = 0;
    while (1) {
        spFiarGamePrintBoard(game);
        winner  = spFiarCheckWinner(game);
        if (!winner) {
            printf("Please make the next move:\n");
        }
        else {
            ended = 1;
            if (winner == SP_FIAR_GAME_TIE_SYMBOL) {
                printf("Game over: it’s a tie\nPlease enter 'quit' to exit or 'restart' to start a new game!\n");
            }
            else {
                printf("Game over: %s\nPlease enter 'quit' to exit or 'restart' to start a new game!\n", winner == SP_FIAR_GAME_PLAYER_1_SYMBOL ? "you win" : "computer wins");
            }
        };
        fgets(input, MAXIMUM_COMMAND_LENGTH + 1, stdin);
        SPCommand command = spParserPraseLine(input);
        if (!command) {
            printf("Error: spParserPraseLine has failed");
            free(input);
            spFiarGameDestroy(game);
            return;
        }
        if (command.cmd == SP_INVALID_LINE) {
            printf("Error: invalid command\n");
        }
        if (command.cmd == SP_SUGGEST_MOVE && !ended) {
            int suggestedMove = spMinimaxSuggestMove(game, maxDepth);
            if (suggestedMove == -1) {
                printf("Error: spMinimaxSuggestMove has failed");
                free(&command);
                free(input);
                spFiarGameDestroy(game);
                return;
            }
            printf("Suggested move: drop a disc to column %d\n", suggestedMove + 1);
        }
        if (command.cmd == SP_UNDO_MOVE) {
            if (spArrayListIsEmpty(game.history)) {
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
        }
        if (command.cmd == SP_ADD_DISC && !ended) {
            if (!command.validArg) {
                printf("Error: column number must be in range 1-7\n");
            }
            else {
                int col = command.arg - 1;
                if (!spFiarGameIsValidMove(game, col)) {
                    printf("Error: column %d is full\n", command.arg);
                }
                else {
                    spFiarGameSetMove(game, col);
                    spFiarGameSetMove(game, spMinimaxSuggestMove(game, maxDepth));
                }
            }
        }
    }
}