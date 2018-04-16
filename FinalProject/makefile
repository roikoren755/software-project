CC = gcc
OBJS = Button.o main.o SPArrayList.o SPChessGame.o SPChessGameSdl.o SPChessMainSdl.o SPChessParser.o SPLoadSaveGameSdl.o SPMainAux.o SPMinimax.o Widget.o

EXEC = chessprog
CCC_COMP_FLAG = -std=c99 -Wall -Wextra -Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main 

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@
main.o: main.c Widget.h Button.h SPChessGame.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessParser.o: SPChessParser.c SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Widget.o: Widget.c Widget.h 
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Button.o: Button.c Button.h Widget.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPArrayList.o: SPArrayList.c SPArrayList.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessGame.o: SPChessGame.c SPChessGame.h SPArrayList.h SPMainAux.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessGameSdl.o: SPChessGameSdl.c SPChessGameSdl.h SPChessGame.h SPChessMainSdl.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPLoadSaveGameSdl.o: SPLoadSaveGameSdl.c SPLoadSaveGameSdl.h SPChessMainSdl.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPMainAux.o: SPMainAux.c SPMainAux.h 
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPMinimax.o: SPMinimax.c SPMinimax.h 
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessMainSdl.o: SPChessMainSdl.c SPChessMainSdl.h SPChessGame.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
clean:
	rm -f *.o $(EXEC)
