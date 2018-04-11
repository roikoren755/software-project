#include "unit_test_util.h"
#include "SPChessParser.h"
#include <string.h>

static int spParserCheckGetNonNegativeInt() {
	SPCommand command;
	strcpy(command.arguments, "1");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == 1);
	strcpy(command.arguments, "11");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == 11);
	strcpy(command.arguments, "111");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == 111);
	strcpy(command.arguments, "-1");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == -1);
	strcpy(command.arguments, "09990");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == 9990);
	strcpy(command.arguments, "1234");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == 1234);
	strcpy(command.arguments, "A");
	ASSERT_TRUE(spParserGetNonNegativeInt(&command) == -1);
	return 1;
}

static int spParserCheckParseLine() {
	SPCommand cmd;
	cmd = spParserParseLine("game_mode 1");
	ASSERT_TRUE(cmd.cmd == SP_GAME_MODE);
	cmd = spParserParseLine("difficulty <blabla>");
	ASSERT_TRUE(cmd.cmd == SP_DIFFICULTY);
	cmd = spParserParseLine("	user_color 5 justkidding not");
	ASSERT_TRUE(cmd.cmd == SP_USER_COLOR);
	cmd = spParserParseLine("load ../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_LOAD);
	cmd = spParserParseLine("default for the win");
	ASSERT_TRUE(cmd.cmd == SP_DEFAULT);
	cmd = spParserParseLine("print_settings");
	ASSERT_TRUE(cmd.cmd == SP_PRINT_SETTINGS);
	cmd = spParserParseLine("quit for the love of all mighty C quit");
	ASSERT_TRUE(cmd.cmd == SP_QUIT);
	cmd = spParserParseLine("start now please");
	ASSERT_TRUE(cmd.cmd == SP_START);
	cmd = spParserParseLine("move <4,A> to <5,D> please");
	ASSERT_TRUE(cmd.cmd == SP_MOVE);
	cmd = spParserParseLine("get_moves OOPS");
	ASSERT_TRUE(cmd.cmd == SP_GET_MOVES);
	cmd = spParserParseLine("save ../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_SAVE);
	cmd = spParserParseLine("undo\n../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_UNDO);
	cmd = spParserParseLine("undo ../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_UNDO);
	cmd = spParserParseLine("reset ../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_RESET);
	cmd = spParserParseLine("daskldjajlsload ../wait/what");
	ASSERT_TRUE(cmd.cmd == SP_INVALID_LINE);
	return 1;
}

char spChessGameGetDestinationPositionFromMove2(unsigned int move) {
	move >>= 8;
	move <<= 24;
	move >>= 24;
	return (char) move; // Get 2nd byte from the right
}

char spChessGameGetCurrentPositionFromMove2(unsigned int move) {
	move >>= 16;
	move <<= 24;
	move >>= 24;
	return (char) move; // Get 3rd Byte from the left
}

unsigned int spChessGameGetColumnFromPosition2(unsigned char position) {
	position <<= 5;
	position >>= 5;
	return (unsigned int) position; // Get 3 rightmost bits
}

unsigned int spChessGameGetRowFromPosition2(unsigned char position) {
	position <<= 2;
	position >>= 5;
	return (unsigned int) position; // Get next 3 bits
}

static int spParserCheckGetLocation() {
	SPCommand command;
	strcpy(command.arguments, "<1,A>");
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spParserGetLocation(&command)) == 7);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spParserGetLocation(&command)) == 0);
	strcpy(command.arguments, "<8,H>");
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spParserGetLocation(&command)) == 0);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spParserGetLocation(&command)) == 7);
	strcpy(command.arguments, "<0,0>");
	ASSERT_TRUE(spParserGetLocation(&command) == 0);
	strcpy(command.arguments, "<>");
	ASSERT_TRUE(spParserGetLocation(&command) == 0);
	strcpy(command.arguments, "<1,1>");
	ASSERT_TRUE(spParserGetLocation(&command) == 0);
	strcpy(command.arguments, "<2,E>");
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spParserGetLocation(&command)) == 6);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spParserGetLocation(&command)) == 4);
	return 1;
}

static int spParserCheckGetMove() {
	SPCommand command;
	strcpy(command.arguments, "<1,A><1,A>");
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command))) == 7);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command))) == 0);
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8)) == 7);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8)) == 0);
	strcpy(command.arguments, "<1,A><>");
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command)) == 0);
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8) == 0);
	strcpy(command.arguments, "<1,A><8,H>");
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command))) == 7);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command))) == 0);
	ASSERT_TRUE(spChessGameGetRowFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8)) == 0);
	ASSERT_TRUE(spChessGameGetColumnFromPosition2(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8)) == 7);
	strcpy(command.arguments, "<><1,A>");
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command)) == 0);
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8) == 0);
	strcpy(command.arguments, "<1,A><1,1>");
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command)) == 0);
	ASSERT_TRUE(spChessGameGetDestinationPositionFromMove2(spParserGetMove(&command) << 8) == 0);
	return 1;
}

int main() {
	RUN_TEST(spParserCheckGetNonNegativeInt);
	RUN_TEST(spParserCheckParseLine);
	RUN_TEST(spParserCheckGetLocation);
	RUN_TEST(spParserCheckGetMove);
	return 0;
}
