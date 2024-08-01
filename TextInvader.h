#pragma once

#include <string>
#include <vector>

const char* PLAYER_SPRITE[] = { " =A= ", "=====" };

const char* PLAYER_EXPLOSION_SPRITE[] = { ",~^.'","=====","'+-'.","=====" };

const char PLAYER_MISSILE_SPRITE = '|';

const char* SHIELD_SPRITE[] = { "/IIIII\\","IIIIIII","I/   \\I" };

const char* ALIEN30_SPRITE[] = { "/oo\\","<  >","/oo\\", "/\''\\" };

const char* ALIEN20_SPRITE[] = { " >< ","|\\/|","|><|", "/  \\" };

const char* ALIEN10_SPRITE[] = { "/--\\","/  \\","/--\\", "<  >" };

const char* ALIEN_EXPLOSION[] = { "\\||/", "/||\\" };

const char* ALIEN_BOMB_SPRITE = "\\|/-";

const char* ALIEN_UFO_SPRITE[] = {"_/oo\\_","=q==p="};

const char* FILE_NAME = "TextInvadersHighScoresTable.txt";

const char* BOOM = "BooM!";

enum {
	SHIELD_SPRITE_HEIGHT = 3,
	SHIELD_SPRITE_WIDTH = 7,
	NUM_ALIEN_ROWS = 5,
	NUM_ALIEN_COLUMNS = 11,
	MAX_NUM_OF_ALIEN_BOMBS = 3,
	MAX_NUM_OF_LIVES = 3,
	PLAYER_SPRITE_WIDTH = 5,
	PLAYER_SPRITE_HEIGHT = 2,
	NOT_IN_PLAY = -1,
	PLAYER_MOVEMENT_AMOUNT = 2,
	PLAYER_MISSILE_SPEED = 1,
	FPS = 30,
	NUM_SHIELDS = 4,
	ALIEN_SPRITE_WIDTH = 4,
	ALIEN_SPRITE_HEIGHT = 2,
	ALIENS_X_PADDING = 1,
	ALIENS_Y_PADDING = 1,
	ALIEN_EXPLOSION_TIME = 4,
	ALIEN_BOMB_SPEED = 1,
	MAX_LEVELS = 10,
	WAIT_TIME_NEXT_LVL = 100,
	WAIT_TIME = 10,
	ALIEN_UFO_SPRITE_WIDTH = 6,
	ALIEN_UFO_SPRITE_HEIGHT = 2,
	MAX_NUMBER_OF_CHARACTERS_IN_NAME = 3,
	MAX_ALPHABET_CHARACTERS = 26,
	MAX_HIGH_SCORES = 10,
};

enum GameState {
	GS_INTRO=0,
	GS_HIGHSCORES,
	GS_PLAY,
	GS_PLAYER_DEAD,
	GS_WAIT,
	GS_GAME_OVER
};

enum AlienState {
	AS_ALIVE = 0,
	AS_DEAD,
	AS_EXPLODING
};

struct Position {
	int x;
	int y;
};

struct Size {
	int width;
	int height;
};

struct Player {
	Position position;
	Position missile;
	Size spriteSize;
	int animation;
	int lives; //Max 3 
	int score;
};

struct Shield {
	Position position;
	char* sprite[SHIELD_SPRITE_HEIGHT];
};

struct AlienBomb {
	Position position;
	int animation;

};

struct AlienSwarm {
	Position position;
	AlienState aliens[NUM_ALIEN_ROWS][NUM_ALIEN_COLUMNS];
	AlienBomb bombs[MAX_NUM_OF_ALIEN_BOMBS];
	Size spriteSize;
	int animation;
	int direction; // > 0 - (1) for going right, < 0 - (-1)for going left
	int numberOfBombsInPlay;
	int movementTimer; //This is going to capture how fast the aliens should be going
	int explosionTimer; //This is going to capture how long to explode for
	int numAliensLeft; // This is to capture when to go to next level
	int line; //This is to capture when the aliens win - starts at the current level and decreases to 0 - once it's 0 then the aliens win
};

struct AlienUFO
{
	Position position;
	Size size;
	int Points; // From 50-200
};

struct Score {
	int score;
	std::string name; //std:: because we're not saying using namespace std; - in this file
};
	
struct HighScoreTable {
	std::vector<Score> scores;
};

struct Game {
	Size windowSize;
	GameState currentState;
	int level;
	int waitTimer;
	clock_t gameTimer;

	int gameOverHPositionCursor; //where the horizontal cursor is
	char playerName[MAX_NUMBER_OF_CHARACTERS_IN_NAME + 1];
	int gameOverVPositionCursor[MAX_NUMBER_OF_CHARACTERS_IN_NAME];
};
