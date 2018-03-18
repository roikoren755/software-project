/*
 * main.c
 *
 *  Created on: 6 במרס 2018
 *      Author: user
 */

#include "SPChessParser.h"
#include "SPChessGame.h"
#include "SPArrayList.h"
#include "SPMainAux.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
	SPChessGame *game = spChessGameCreate(3);
	spChessGamePrintBoard(game);

	spChessGameDestroy(game);

}
