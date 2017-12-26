#ifndef SPMINIMAXNODE_H_
#define SPMINIMAXNODE_H_

typedef enum {
	MM_SUCCESS,
	MM_ERROR,
} SP_MM_MASSAGE;

typedef struct command_t {
	SP_MM_MASSAGE mas;
	int score;
} MMreturn;

//Put all decleartions and constants here
int scoreBoard(SPFiarGame* src);
MMreturn spEvalGame(SPFiarGame* game, unsigned int height, bool turn);
MMreturn spMaxIndex(SPFiarGame* game);
MMreturn spMaxScore(SPFiarGame* game, unsigned int height);
MMreturn spMinScore(SPFiarGame* game, unsigned int height);

#endif
