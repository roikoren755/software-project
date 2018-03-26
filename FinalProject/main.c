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
#include "SPBufferset.h"
#include <stdio.h>
#include <string.h>

#define CONSOLE 0
#define GUI 1
#define CONSOLE_ARGUMENT "-c"
#define GUI_ARGUMENT "-g"
#define USAGE "Usage: ./chessprog [-c | -g]\n"
#define GET_COLOR(color) color ? "white" : "black"

void spChessPrintGameTitle(int mode) {
	if (!mode) {
		printf(" Chess\n");
		for (int i = 0; i < 7; i++) {
			printf("-");
		}
		printf("\n");
	}
}

SP_CHESS_GAME_MESSAGE spChessGameSetGameMode(SPChessGame* game, SPCommand* command, int mode) {
	if (!game || !command) {
		printf("ERROR: Could not set game mode, quitting...\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int gameMode = spParserGetNonNegativeInt(command);
	switch (gameMode) {
	case 1:
	case 2:
		game->gameMode = gameMode;
		if (!mode) {
			printf("Game mode is set to %d-player\n", gameMode);
		}
		break;
	default:
		if (!mode) {
			printf("Wrong game mode\n");
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameSetDifficulty(SPChessGame* game, SPCommand* command, char** difficulties, int mode) {
	if (!game || !command) {
		printf("ERROR: Could not set difficulty. Quitting...\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int difficulty = spParserGetNonNegativeInt(command);
	if (difficulty >= 1 && difficulty <= 5) {
		game->difficulty = difficulty;
		if (!mode) {
			printf("Difficulty level is set to %s\n", difficulties[difficulty]);
		}
	}
	else {
		if (!mode) {
			printf("Wrong difficulty level. The value should be between 1 to 5\n");
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameSetUserColor(SPChessGame* game, SPCommand* command, int mode) {
	if (!game || !command) {
		printf("ERROR: Couldn't set user color. Quitting...\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int userColor = spParserGetNonNegativeInt(command);
	switch (userColor) {
	case 0:
	case 1:
		game->userColor = userColor;
		if (!mode) {
			printf("User color is set to %s\n", GET_COLOR(userColor));
		}
		break;
	default:
		if (!mode) {
			printf("Wrong user color. The value should be 0 or 1\n");
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameLoadGame(SPChessGame* game, SPCommand* command, int mode) {
	if (!game || !command) {
		printf("Error: Could not find game to load to, or the command to do so! Quitting...\n");
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	SP_CHESS_GAME_MESSAGE message = spChessLoadGame(game, command->arguments);
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		if (!mode) {
			printf("Error: File doesn’t exist or cannot be opened\n");
			return SP_CHESS_GAME_INVALID_ARGUMENT;
		}
	}

	return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameResetSettings(SPChessGame* game, int mode) {
	if (!game) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	game->userColor = WHITE;
	game->gameMode = 1;
	game->difficulty = 2;
    if (!mode) {
    	printf("All settings reset to default\n");
    }

    return SP_CHESS_GAME_SUCCESS;
}

SP_CHESS_GAME_MESSAGE spChessGameMove(SPChessGame* game, SPCommand* command, int mode) {
	if (!game || !command) {
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}

	int move = spParserGetMove(command);
	SP_CHESS_GAME_MESSAGE message = spChessGameIsValidMove(game, move);
	if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
		printf("ERROR: Something went wrong! Quitting...\n");
		return message;
	}
	else if (message == SP_CHESS_GAME_INVALID_POSITION) {
		if (!mode) {
			printf("Invalid position on the board\n");
		}
	}
	else if (message == SP_CHESS_GAME_NO_PIECE_IN_POSITION) {
		if (!mode) {
			printf("The specified position does not contain your piece\n");
		}
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE) {
		if (!mode) {
			printf("Illegal move\n");
		}
	}
	else if (message == SP_CHESS_GAME_ILLEGAL_MOVE_REMAINS_THREATENED) {
		if (!mode) {
			printf("Illegal move: king is still threatened\n");
		}
	}
	else if (message == SP_CHESS_GAME_KING_BECOMES_THREATENED) {
		if (!mode) {
			printf("Illegal move: king will be threatened\n");
		}
	}
	else {
		message = spChessGameSetMove(game, move);
		if (message != SP_CHESS_GAME_SUCCESS) {
			printf("ERROR: OOPS... something went wrong while setting up a move! Quitting...\n");
		}
		else {
			message = spChessCheckGameState(game, game->currentPlayer);
			if (message != SP_CHESS_GAME_SUCCESS) {
				if (message == SP_CHESS_GAME_CHECK) {
					if (!mode) {
						printf("Check: %s king is threatened\n", game->currentPlayer ? "white" : "black");
					}
				}
				else {
					if (message == SP_CHESS_GAME_CHECKMATE) {
						if (!mode) {
							printf("Checkmate! %s player wins the game\n", game->currentPlayer ? "black" : "white");
						}
					}
					else if (message == SP_CHESS_GAME_DRAW) {
						if (!mode) {
							printf("The game ends in a draw\n");
						}
					}
					else {
						printf("ERROR: Something went wrong while checking game state. Quitting...\n");
						return message;
					}
				}
			}
			else if (game->gameMode == 1) {
				move = spMinimaxSuggestMove(game);
				if (move == -1) {
					printf("ERROR: Couldn't figure out computer move. Quitting...\n");
					return SP_CHESS_GAME_INVALID_ARGUMENT;
				}
				else {
					char piece = spChessGameGetPieceAtPosition(game, spChessGameGetCurrentPositionFromMove(move << 8));
					spPrintComputerMove(piece, move);
					spChessGameSetMove(game, move);
				}
			}
		}
	}

	return message;
}

int main(int argc, char* argv[]) {
	SP_BUFF_SET();
    int mode = CONSOLE;
    if (argc == 1) {
		mode = CONSOLE;
	}
	else if (argc == 2) {
        if (!strcmp(argv[1], GUI_ARGUMENT)) {
            mode = GUI;
        }
        else if (strcmp(argv[1], CONSOLE_ARGUMENT)) {
            printf(USAGE);
            return 0;
        }
    }
	else {
		printf(USAGE);
		return 0;
	}

	SPChessGame* game = spChessGameCreate();
	if (!game) {
		printf("ERROR: Could not create game! Exiting...\n");
		return 1;
	}

	spChessPrintGameTitle(mode);

	char* difficulties[] = {"", "amateur", "easy", "moderate", "hard", "expert"};
	int quit = 0;
	int settings = 1;
	int printedWelcome = 0;
	SP_CHESS_GAME_MESSAGE message;
	SPCommand command;
	while (!quit) {
		if (settings) {
			if (!printedWelcome) {
				printf("Specify game settings or type 'start' to begin a game with the current settings:\n");
				printedWelcome = 1;
			}
			printf("GET COMMAND MESSAGE\n"); // TODO - fix me!
			command = spGetCommand(mode);

			if (command.cmd == SP_GAME_MODE) {
				message = spChessGameSetGameMode(game, &command, mode);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					quit = 1;
				}
			}

			else if (command.cmd == SP_DIFFICULTY && game->gameMode == 1) {
				message = spChessGameSetDifficulty(game, &command, difficulties, mode);
			}

			else if (command.cmd == SP_USER_COLOR && game->gameMode == 1) {
				message = spChessGameSetUserColor(game, &command, mode);
			}

			else if (command.cmd == SP_LOAD) {
				message = spChessGameLoadGame(game, &command, mode);
			}

			else if (command.cmd == SP_DEFAULT) {
				message = spChessGameResetSettings(game, mode);
			}

			else if (command.cmd == SP_PRINT_SETTINGS) {
				spFprintSettings(game, stdout);
			}

			else if (command.cmd == SP_QUIT) {
				quit = 1;
			}

			else if (command.cmd == SP_START) {
				settings = 0;
				if (!mode) {
					printf("Starting game...\n");
				}
			}

			else {
				printf("ERROR: Invalid command\n");
			}
		}

        else {
            spChessGamePrintBoard(game);
            printf("Enter your move (%s player):\n", game->currentPlayer ? "white" : "black");
            command = spGetCommand(mode);
            if (command.cmd == SP_MOVE) {
            	message = spChessGameMove(game, &command, mode);
//            	switch (message) {
//            	case SP_CHESS_GAME_INVALID_ARGUMENT:
//            		quit = 1;
//            		break;
//            	case SP_CHESS_GAME_SUCCESS:
//            		break;
//            	default:
//            		break;
//            	}
                int move = spParserGetMove(&command);
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
								char piece = spChessGameGetPieceAtPosition(game, spChessGameGetCurrentPositionFromMove(move << 8));
								spPrintComputerMove(piece, move);
								spChessGameSetMove(game, move);
							}
						}
					}
				}
            }

			else if (command.cmd == SP_GET_MOVES) {
				char location = spParserGetLocation(&command);
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
				int move = spChessGameGetLastMovePlayed(game);
				message = spChessGameUndoMove(game);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					printf("ERROR: Something went wrong while trying to undo move. Quitting...\n");
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
						printf("ERROR: Couldn't undo second move! Quitting... \n");
						quit = 1;
					}
					else if (message == SP_CHESS_GAME_SUCCESS) {
						spPrintUndoneMove(move, game->currentPlayer);
					}
				}
			}

			else if (command.cmd == SP_RESET) {
				settings = 1;
				message = spChessGameResetGame(game);
				if (message == SP_CHESS_GAME_INVALID_ARGUMENT) {
					printf("ERROR: Something went awry while resetting game! Quitting... \n");
					quit = 1;
				}
			}

			else if (command.cmd == SP_QUIT) {
				quit = 1;
			}

			else {
				printf("ERROR: Invalid command\n");
			}
        }
	}

	spChessGameDestroy(game);
	printf("Exiting...\n");

	return 0;
}
