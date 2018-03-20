CC = gcc
OBJS = main.o SPArrayList.o SPChessGame.o SPChessParser.o SPMainAux.o SPMinimax.o
EXEC = chessprog
CC_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
SPMinimax.o: SPMinimax.h SPMinimax.c SPChessGame.h SPMainAux.h
	$(CC) $(CC_COMP_FLAG) -c $*.c
SPMainAux.o: SPMainAux.h SPMainAux.c SPChessGame.h SPChessParser.h
	$(CC) $(CC_COMP_FLAG) -c $*.c
SPChessParser.o: SPChessParser.h SPChessParser.c
	$(CC) $(CC_COMP_FLAG) -c $*.c
SPChessGame.o: SPChessGame.h SPChessGame.c SPArrayList.h SPMainAux.h
	$(CC) $(CC_COMP_FLAG) -c $*.c
SPArrayList.o: SPArrayList.h SPArrayList.c
	$(CC) $(CC_COMP_FLAG) -c $*.c
main.o: main.c SPChessParser.h SPChessGame.h SPArrayList.h SPMainAux.h SPMinimax.h
	$(CC) $(CC_COMP_FLAG) -c $*.c
clean:
	rm -f *.o $(EXEC)