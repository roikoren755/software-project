/*
 * main.c
 *
 *  Created on: 6 March 2018
 *      Author: user
 */

#include "SPChessParser.h"
#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
#include "SPMinimax.h"
#include <stdio.h>
#include <string.h>

#define CONSOLE 0
#define GUI 1
#define CONSOLE_ARGUMENT "-c"
#define GUI_ARGUMENT "-g"

int main(int argc, char* argv[]) {
    int mode;
    if (argc == 1) {
		mode = CONSOLE;
	}
	else if (argc == 2) {
        if (!strcmp(argv[1], GUI_ARGUMENT)) {
            mode = GUI;
        }
        else if (strcmp(argv[1], CONSOLE_ARGUMENT)) {
            printf("Usage: ./chessprog [-c | -g]\n");
            return 0;
        }
    }
	else {
		printf("Usage: ./chessprog [-c | -g]\n");
		return 0;
	}

	SPChessGame* game = spChessGameCreate();
	if (!game) {
		perror("ERROR: Could not create game! Exiting...\n");
		return 1;
	}

	if (!mode) {
		printf(" Chess\n");
		for (int i = 0; i < 7; i++) {
			printf("-");
		}
		printf("\n");
	}

	char* difficulties[] = {"", "amateur", "easy", "moderate", "hard", "expert"};
	int quit = 0;
	int settings = 1;
	int printedWelcome = 0;
	while (!quit) {
		if (settings) {
			if (!printedWelcome) {
				printf("Specify game settings or type 'start' to begin a game with the current settings:\n");
				printedWelcome = 1;
			}
			printf("GET COMMAND MESSAGE\n"); // TODO - fix me!
			SPCommand cmd = spGetCommand(mode);

			if (cmd.cmd == SP_GAME_MODE) {
				int gameMode = spParserGetNonNegativeInt(&cmd);
				if (gameMode == 1 || gameMode == 2) {
					game->gameMode = gameMode;
					printf("Game mode is set to %d-player\n", gameMode);
				}
				else {
					printf("Wrong game mode\n");
				}
			}

			else if (cmd.cmd == SP_DIFFICULTY && game->gameMode == 1) {
				int difficulty = spParserGetNonNegativeInt(&cmd);
				if (difficulty >= 1 && difficulty <= 5) {
					game->difficulty = difficulty;
					printf("Difficulty level is set to %s\n", difficulties[difficulty]);
				}
				else {
					printf("Wrong difficulty level. The value should be between 1 to 5\n");
				}
			}

			else if (cmd.cmd == SP_USER_COLOR && game->gameMode == 1) {
				int userColor = spParserGetNonNegativeInt(&cmd);
				if (userColor == 0 || userColor == 1) {
					game->userColor = userColor;
					printf("User color is set to %s\n", userColor ? "white" : "black");
				}
				else {
					printf("Wrong user color. The value should be 0 or 1\n");
				}
			}

			else if (cmd.cmd == SP_LOAD) {
				spChessLoadGame(game, cmd.arguments);
			}

			else if (cmd.cmd == SP_DEFAULT) {
				game->userColor = WHITE;
				game->gameMode = 1;
				game->difficulty = 2;
                printf("All settings reset to default\n");
			}

			else if (cmd.cmd == SP_PRINT_SETTINGS) {
				spFprintSettings(game, stdout);
			}

			else if (cmd.cmd == SP_QUIT) {
				quit = 1;
			}

			else if (cmd.cmd == SP_START) {
				settings = 0;
				printf("Starting game...\n");
			}

			else {
				perror("ERROR: Invalid command\n");
			}
		}

        else {
            spChessGamePrintBoard(game);
            printf("Enter your move (%s player):\n", game->currentPlayer ? "white" : "black");
            SPCommand cmd = spGetCommand(mode);
			SP_CHESS_GAME_MESSAGE message;
            if (cmd.cmd == SP_MOVE) {
                int move = spParserGetMove(&cmd) << 8;
				message = spChessGameIsValidMove(game, move);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					perror("ERROR: Something went wrong! Quitting...\n");
					quit = 1;
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
				else if (message == SP_CHESS_GAME_KING_BECOMES_THREATENED) {
					printf("Illegal move: king will be threatened\n");
				}
				else {
					message = spChessGameSetMove(game, move);
					if (message != SP_CHESS_GAME_SUCCESS) {
						perror("ERROR: OOPS... something went wrong while setting up a move! Quitting...\n");
						quit = 1;
					}
					else {
						message = spChessCheckGameState(game, game->currentPlayer);
						if (message != SP_CHESS_GAME_SUCCESS) {
							if (message == SP_CHESS_GAME_CHECK) {
								printf("Check: %s king is threatened\n", game->currentPlayer ? "white" : "black");
							}
							else {
								quit = 1;
								if (message == SP_CHESS_GAME_CHECKMATE) {
									printf("Checkmate! %s player wins the game\n", game->currentPlayer ? "black" : "white");
								}
								else if (message == SP_CHESS_GAME_DRAW) {
									printf("The game ends in a draw\n");
								}
								else {
									perror("ERROR: Something went wrong while checking game state. Quitting...\n");
								}
							}
						}

						if (!quit && game->gameMode == 1) {
							move = spMinimaxSuggestMove(game);
							if (move == -1) {
								perror("ERROR: Couldn't figure out computer move. Quitting...\n");
								quit = 1;
							}
							else {
								char piece = spChessGameGetPieceAtPosition(game, spChessGameGetDestinationPositionFromMove(move << 8));
								spPrintComputerMove(piece, move);
								spChessGameSetMove(game, move);
							}
						}
					}
				}
            }

			else if (cmd.cmd == SP_GET_MOVES) {
				char location = spParserGetLocation(&cmd);
				message = spChessVerifyPositionAndPiece(game, location);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					perror("ERROR: Couldn't verify position and piece. Quitting...\n");
					quit = 1;
				}
				else if (message == SP_CHESS_GAME_INVALID_POSITION) {
					printf("Invalid position on the board\n");
				}
				else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
					printf("The specified position does not contain a player piece\n");
				}
				else {
					SPArrayList* possibleMoves = spChessGameGetMoves(game, location);
					message = spChessPrintMoves(possibleMoves);
					spArrayListDestroy(possibleMoves);
				}
			}

			else if (cmd.cmd == SP_SAVE) {
				message = spChessSaveGame(game, cmd.arguments);
				if (message == SP_CHESS_GAME_SUCCESS) {
					printf("Game saved to: %s\n", cmd.arguments);
				}
				else {
					printf("File cannot be created or modified\n");
				}
			}

			else if (cmd.cmd == SP_UNDO) {
				int move = spChessGameGetLastMovePlayed(game);
				message = spChessGameUndoMove(game);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					perror("ERROR: Something went wrong while trying to undo move. Quitting...\n");
					quit = 1;
				}
				else if (message == SP_CHESS_GAME_NO_HISTORY) {
					printf("Empty history, no move to undo\n");
				}
				else {
					spPrintUndoneMove(move, game->currentPlayer);
					move = spChessGameGetLastMovePlayed(game);
					message = spChessGameUndoMove(game);
					if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
						perror("ERROR: Couldn't undo second move! Quitting... \n");
						quit = 1;
					}
					else if (message == SP_CHESS_GAME_SUCCESS) {
						spPrintUndoneMove(move, game->currentPlayer);
					}
				}
			}

			else if (cmd.cmd == SP_RESET) {
				settings = 1;
				message = spChessGameResetGame(game);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					perror("ERROR: Something went awry while resetting game! Quitting... \n");
					quit = 1;
				}
			}

			else if (cmd.cmd == SP_QUIT) {
				quit = 1;
			}
        }
	}

	spChessGameDestroy(game);
	printf("Exiting...\n");

	return 0;
}
