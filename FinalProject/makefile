CC = gcc
OBJS = Button.o main.o SPArrayList.o SPChessGame.o SPChessGameSdl.o SPChessMainSdl.o SPChessParser.o SPLoadSaveGameSdl.o SPMainAux.o SPMinimax.o Widget.o

EXEC = chessprog
CCC_COMP_FLAG = -std=c99 -Wall -Wextra -Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main 

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@
main.o: main.c SPChessGame.h SPArrayList.h SPMainAux.h SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Button.o: Button.c Button.h SPChessGame.h SPArrayList.h Widget.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPArrayList.o: SPArrayList.c SPArrayList.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessGame.o: SPChessGame.c SPChessGame.h SPArrayList.h SPMainAux.h SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessGameSdl.o: SPChessGameSdl.c Button.h SPChessGame.h SPArrayList.h Widget.h SPChessGameSdl.h SPChessMainSdl.h SPMainAux.h SPChessParser.h SPMinimax.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessMainSdl.o: SPChessMainSdl.c SPChessMainSdl.h Widget.h SPChessGame.h SPArrayList.h Button.h SPChessGameSdl.h SPLoadSaveGameSdl.h SPMainAux.h SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPChessParser.o: SPChessParser.c SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPLoadSaveGameSdl.o: SPLoadSaveGameSdl.c SPLoadSaveGameSdl.h SPChessMainSdl.h Widget.h SPChessGame.h SPArrayList.h Button.h SPChessGameSdl.h SPMainAux.h SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPMainAux.o: SPMainAux.c SPMainAux.h SPChessGame.h SPArrayList.h SPChessParser.h Button.h Widget.h SPChessGameSdl.h SPChessMainSdl.h SPLoadSaveGameSdl.h SPMinimax.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
SPMinimax.o: SPMinimax.c SPMinimax.h SPChessGame.h SPArrayList.h SPMainAux.h SPChessParser.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Widget.o: Widget.c Widget.h SPChessGame.h SPArrayList.h
	$(CC) $(CCC_COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
clean:
	rm -f *.o $(EXEC)