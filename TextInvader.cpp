// TextInvader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CursesUtils.h"
#include "TextInvader.h"
#include <ctime>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <algorithm> //for sort
#include <fstream> //for file i/o

using namespace std;

/// Function Declaration
void InitGame(Game& game);
void InitPlayer(const Game& game, Player& player);
void ResetPlayer(const Game& game, Player& player);
void ResetMissile(Player& player);

int HandleInput(Game& game, Player& player, AlienSwarm& aliens, Shield shields[], int numberOfShields, HighScoreTable& table);
void UpdateGame(clock_t dt, Game& game, Player& player, Shield shield[], int numberOfShields, AlienSwarm& aliens, AlienUFO& ufo);
void NextLevel(Game& game, Player& player, AlienSwarm& aliens, Shield  shields[], int numberOfShields);
void DrawGame(const Game& game, const Player& player, Shield shields[], int numberOfShields, const AlienSwarm& aliens, const AlienUFO& ufo, const HighScoreTable& table);
void MovePlayer(const Game& game, Player& player, int dx);
void PlayerShoot(Player& player);
void DrawPlayer(const Player& player, const char* sprite[], const Game& game);
void UpdateMissile(Player& player);
void InitShields(const Game& game, Shield shields[], int numberOfShields);
void CleanShields(Shield shields[], int numberOfShields);
void DrawShields(const Shield shields[], int numberOfShields);

//returns the shield index of which shield got hit,
//return NOT_IN_PLAY if nothing was hit
//also returns the shield collision point
int IsCollision(const Position& projectile, const Shield shields[], int numberOfShields, Position& shieldCollisionPoint);
void ResolveShieldCollision(Shield shields[], int shieldIndex, const Position& shieldCollisionPoint);
void InitAliens(const Game& game, AlienSwarm& aliens);
void DrawAliens(const AlienSwarm& aliens);
bool IsCollision(const Player& player, const AlienSwarm& aliens, Position& alienCollisionPositionInArray);
int ResolveAlienCollision(AlienSwarm& aliens, const Position hitPositionInAliensArray);
bool UpdateAliens(Game& game, AlienSwarm& aliens, Player& player, Shield shields[], int numberOfShields);
void ResetMovementTime(AlienSwarm& aliens);

void FindEmptyRowAndColumns(const AlienSwarm& aliens, int& emptyColsLeft, int& emptyColsRight, int& emptyRowBottom);
void DestroyShields(const AlienSwarm& aliens, Shield shields[],int numberOfShields );
void CollideShieldsWithAlien(Shield shields[], int numberOfShields, int alienPosX, int alienPosY, const Size& size);

bool ShouldShootBomb(const AlienSwarm& aliens);
void ShootBomb(AlienSwarm& aliens, int columnToShoot);
bool UpdateBombs(const Game& game, AlienSwarm& aliens, Player& player, Shield shield[], int numberOfShields);
bool IsCollision(const Position& projectile,const Position& spritePosition, const Size& spriteSize);

void ResetGame(Game& game, Player& player, AlienSwarm& aliens, Shield shields[], int numberOfShields);
void ResetShields(const Game& game,Shield shields[], int numberOfShields);

void DrawGameOverScreen(const Game& game);
void DrawIntroScreen(const Game& game);

void ResetUFO(const Game& game, AlienUFO& ufo);
void PutUFOInPlay(const Game& game, AlienUFO& ufo);
void UpdateUFO(const Game& game, AlienUFO& ufo);
void DrawUFO(const AlienUFO& ufo);

void ResetGameOverPositionCursors(Game& game);
void AddHighScore(HighScoreTable& table, int score, const string& name);

bool ScoreCompare(const Score& score1, const Score& score2);
void DrawHighScoreTable(const Game& game, const HighScoreTable& table);

void SaveHighScores(const HighScoreTable& table);
void LoadHighScores(HighScoreTable& table);
void DrawCountDownScreen(const Game& game);

int main()
{
	srand(time_t(NULL));

    Game game;
    Player player;
    Shield shields[NUM_SHIELDS];
    AlienSwarm aliens;
    AlienUFO ufo;
    HighScoreTable table;

    InitializeCurses(true);

    InitGame(game);
    game.level = 1;
    InitPlayer(game, player);
    InitShields(game, shields, NUM_SHIELDS);
    InitAliens(game, aliens);
    ResetUFO(game, ufo);
    LoadHighScores(table);

    bool quit = false;
    int input;

    clock_t lastTime = clock();

    while(!quit)
    {
        input = HandleInput(game, player, aliens, shields, NUM_SHIELDS, table);
        
        if (input != 'q') 
        {
            clock_t currentTime = clock();
            clock_t dt = currentTime - lastTime;

            if (dt > CLOCKS_PER_SEC/FPS) {

                lastTime = currentTime;
                UpdateGame(dt,game, player, shields, NUM_SHIELDS, aliens, ufo);
                ClearScreen();
                DrawGame(game, player, shields, NUM_SHIELDS, aliens, ufo, table);
                RefreshScreen();
            }
        }
        else {
            quit = true;
        }
    }

    CleanShields(shields, NUM_SHIELDS);
    ShutdownCurses();

    return 0;
}

void InitGame(Game& game)
{
    game.windowSize.width = ScreenWidth();
    game.windowSize.height = ScreenHeight();
    game.currentState = GS_INTRO;
    game.waitTimer = 0;
    game.gameTimer = 0;

    ResetGameOverPositionCursors(game);
}

void InitPlayer(const Game& game, Player& player)
{
    player.lives = MAX_NUM_OF_LIVES;
    player.spriteSize.width = PLAYER_SPRITE_WIDTH;
    player.spriteSize.height = PLAYER_SPRITE_HEIGHT;
    player.score = 0;
    ResetPlayer(game, player);
}

void ResetPlayer(const Game& game, Player& player)
{
    player.position.x = game.windowSize.width / 2 - player.spriteSize.width / 2; // puts the player in the center of the screen
    player.position.y = game.windowSize.height - player.spriteSize.height - 1 ;
    player.animation = 0;
    ResetMissile(player);
}

void ResetMissile(Player& player)
{
    player.missile.x = NOT_IN_PLAY;
    player.missile.y = NOT_IN_PLAY;
}

int HandleInput(Game& game, Player& player, AlienSwarm& aliens, Shield shields[], int numberOfShields, HighScoreTable& table)
{
    int input = GetChar();
   
    switch (input) {
    case 's':
        if (game.currentState == GS_INTRO)
        {
            game.currentState = GS_HIGHSCORES;
        }
        break;
    case 'q':
        return input;   
    case AK_LEFT:
        if (game.currentState == GS_PLAY) 
        {
            MovePlayer(game, player, -PLAYER_MOVEMENT_AMOUNT);
        }
        else if (game.currentState == GS_GAME_OVER)
        {
            game.gameOverHPositionCursor = game.gameOverHPositionCursor - 1;
            if (game.gameOverHPositionCursor < 0)
            {
                game.gameOverHPositionCursor = MAX_NUMBER_OF_CHARACTERS_IN_NAME - 1;
            }
        }
        break;
    case AK_RIGHT:
        if (game.currentState == GS_PLAY) {
            MovePlayer(game, player, PLAYER_MOVEMENT_AMOUNT);
        }
        else if (game.currentState == GS_GAME_OVER)
        {
            game.gameOverHPositionCursor = (game.gameOverHPositionCursor + 1) % MAX_NUMBER_OF_CHARACTERS_IN_NAME;
        }
        break;
    case AK_UP:
        if (game.currentState == GS_GAME_OVER)
        {
            game.gameOverVPositionCursor[game.gameOverHPositionCursor] = game.gameOverVPositionCursor[game.gameOverHPositionCursor] - 1;

            if (game.gameOverVPositionCursor[game.gameOverHPositionCursor] < 0)
            {
                game.gameOverVPositionCursor[game.gameOverHPositionCursor] = MAX_ALPHABET_CHARACTERS - 1;
            }

            game.playerName[game.gameOverHPositionCursor] = 'A' + game.gameOverVPositionCursor[game.gameOverHPositionCursor];
        }
        break;
    case AK_DOWN:
        if (game.currentState == GS_GAME_OVER)
        {
            game.gameOverVPositionCursor[game.gameOverHPositionCursor] = (game.gameOverVPositionCursor[game.gameOverHPositionCursor] + 1) % MAX_ALPHABET_CHARACTERS;

            game.playerName[game.gameOverHPositionCursor] = 'A' + game.gameOverVPositionCursor[game.gameOverHPositionCursor];
        }
        break;
    case 'd':
        if (game.currentState == GS_PLAY)
        {
            NextLevel(game, player, aliens, shields, numberOfShields);
        }
        break;
    case ' ':
        if (game.currentState == GS_PLAY) {
            PlayerShoot(player);
        }
        else if (game.currentState == GS_PLAYER_DEAD)
        {
            player.lives--;
            player.animation = 0;

            if (player.lives == 0)
            {
                game.currentState = GS_GAME_OVER;
                ResetGameOverPositionCursors(game);
            }
            else
            {
                game.currentState = GS_WAIT;
                game.waitTimer = WAIT_TIME;
            }
        }
        else if (game.currentState == GS_GAME_OVER)
        {
            game.playerName[MAX_NUMBER_OF_CHARACTERS_IN_NAME] = '\0';
            AddHighScore(table, player.score, std::string(game.playerName));
            game.currentState = GS_HIGHSCORES;
        }
        else if (game.currentState == GS_HIGHSCORES)
        {
            game.currentState = GS_INTRO;
            ResetGame(game, player, aliens, shields, numberOfShields);
        }
        else if (game.currentState == GS_INTRO) {
            game.currentState = GS_PLAY;
            InitPlayer(game, player);
        }
    }
    return input;
}

void UpdateGame(clock_t dt, Game& game, Player& player, Shield shields[], int numberOfShields, AlienSwarm& aliens, AlienUFO& ufo)
{
    game.gameTimer += dt;

    if (game.currentState == GS_PLAY)
    {       
        UpdateMissile(player);

        Position shieldCollisionPoint;

        int shieldIndex = IsCollision(player.missile, shields, numberOfShields, shieldCollisionPoint);
        if (shieldIndex != NOT_IN_PLAY)
        {   
            ResetMissile(player);
            ResolveShieldCollision(shields, shieldIndex, shieldCollisionPoint);
        }

        Position playerAlienCollisionPoint;

        if (IsCollision(player, aliens, playerAlienCollisionPoint)) {
            ResetMissile(player);
            player.score += ResolveAlienCollision(aliens, playerAlienCollisionPoint);
        }

        if (UpdateAliens(game, aliens, player, shields, numberOfShields)) 
        {
            game.currentState = GS_PLAYER_DEAD;
        }

        if (aliens.numAliensLeft == 0) 
        {
            //game.level++;
             //CODE UPDATE: the game.level should increment properly. If the game.level goes down to 0, it should actually be at 1 instead so that aliens.line works correctly

            NextLevel(game, player, aliens, shields, numberOfShields);
        }
        if (ufo.position.x == NOT_IN_PLAY) 
        {
            //Put the ufo in play
            if (game.gameTimer % 500 == 13) 
            {
                PutUFOInPlay(game, ufo);
            }
        }
        else 
        {
            // Update the ufo
            if (IsCollision(player.missile, ufo.position, ufo.size))
            {
                player.score += ufo.Points;
                ResetMissile(player);
                ResetUFO(game, ufo);
            }
            else 
            {
                UpdateUFO(game, ufo);
            }
        }
    }
    else if (game.currentState == GS_PLAYER_DEAD)
    {
        player.animation = (player.animation + 1) % 2;
    }
    else if (game.currentState==GS_WAIT)
    {
        game.waitTimer--;
        
       if (game.waitTimer == 0) 
        {
            game.currentState = GS_PLAY;
        }
    }
}

void NextLevel(Game& game, Player& player, AlienSwarm& aliens, Shield  shields[], int numberOfShields)
{
    game.level = ((game.level + 1) % MAX_LEVELS);

    if (game.level == 0)
    {
        game.level = 1;
    }

    game.currentState = GS_WAIT;
    game.waitTimer = WAIT_TIME_NEXT_LVL;
    ResetGame(game, player, aliens, shields, numberOfShields);
}

void DrawGame(const Game& game, const Player& player, Shield shields[], int numberOfShields, const AlienSwarm& aliens, const AlienUFO& ufo,const HighScoreTable& table)
{
    if (game.currentState  == GS_PLAY || game.currentState == GS_PLAYER_DEAD ) 
    {
        if (game.currentState == GS_PLAY)
        {
            DrawPlayer(player, PLAYER_SPRITE, game);
        } 
        else 
        {
            DrawPlayer(player, PLAYER_EXPLOSION_SPRITE, game);
        }

        DrawShields(shields, numberOfShields);

        DrawAliens(aliens);

        DrawUFO(ufo);
    }
    else if(game.currentState == GS_GAME_OVER)
    {
        DrawGameOverScreen(game);
    }
    else if (game.currentState == GS_INTRO) 
    {
        DrawIntroScreen(game);
    }
    else if (game.currentState == GS_HIGHSCORES)
    {
        DrawHighScoreTable(game, table);
    }
    else if (game.currentState == GS_WAIT) {
        DrawCountDownScreen(game);
    }
}

void MovePlayer(const Game& game, Player& player, int dx) {
    if (player.position.x + player.spriteSize.width + dx > game.windowSize.width) //Make sure player doesn't go off the screen to the right
        player.position.x = game.windowSize.width - player.spriteSize.width; // The right most position the player can be
    else if (player.position.x + dx < 0) //The player is off the screen to the left.
        player.position.x = 0;
    else
        player.position.x += dx;
}


void PlayerShoot(Player& player)
{
    if (player.missile.x == NOT_IN_PLAY || player.missile.y == NOT_IN_PLAY) {
        player.missile.y = player.position.y - 1; // One row above the player
        player.missile.x = player.position.x + player.spriteSize.width / 2;
    }
}

void DrawPlayer(const Player& player, const char* sprite[], const Game& game) 
{    
    DrawSprite(player.position.x, player.position.y, sprite, player.spriteSize.height, player.animation * player.spriteSize.height);
    
    if (player.missile.x != NOT_IN_PLAY) 
    {
        DrawCharacter(player.missile.x, player.missile.y, PLAYER_MISSILE_SPRITE);
    }
    mvprintw(0, 0, "Score: %i, Lives: %i, Level: %i", player.score, player.lives, game.level);
}

void UpdateMissile(Player& player) {
    if (player.missile.y != NOT_IN_PLAY) {
        player.missile.y -= PLAYER_MISSILE_SPEED;
        if (player.missile.y < 0) {
            ResetMissile(player);
        }
    }
}

void InitShields(const Game& game, Shield shields[], int numberOfShields) {

    for (int i = 0; i < numberOfShields; i++)
    {
        Shield& shield = shields[i];

        for (int row = 0; row < SHIELD_SPRITE_HEIGHT; row++) {
            shield.sprite[row] = new char[SHIELD_SPRITE_WIDTH + 1];
        }
    }
    ResetShields(game, shields, numberOfShields);
}

void CleanShields(Shield shields[], int numberOfShields) {
    for (int i = 0; i < numberOfShields; i++)
    {
        Shield& shield = shields[i];

        for (int  row = 0; row < SHIELD_SPRITE_HEIGHT; row++)
        {
            delete[] shield.sprite[row];
        }
    }
}

void DrawShields(const Shield shields[], int numberOfShields)
{
    for (int i = 0; i < numberOfShields; i++)
    {
        const Shield& shield = shields[i];
        DrawSprite(shield.position.x, shield.position.y, (const char**)shield.sprite, SHIELD_SPRITE_HEIGHT);
    }
}

int IsCollision(const Position& projectile, const Shield shields[], int numberOfShields, Position& shieldCollisionPoint)
{
    shieldCollisionPoint.x = NOT_IN_PLAY;
    shieldCollisionPoint.y = NOT_IN_PLAY;

    if (projectile.y != NOT_IN_PLAY) 
    {
        for (int i = 0; i < numberOfShields; i++)
        {
            const Shield& shield = shields[i];

            if (projectile.x >= shield.position.x && projectile.x < (shield.position.x + SHIELD_SPRITE_WIDTH) //is it line horizontally?
                && (projectile.y >= shield.position.y && projectile.y<(shield.position.y + SHIELD_SPRITE_HEIGHT)) //is it line Veritcally?
                && shield.sprite[projectile.y - shield.position.y][projectile.x-shield.position.x] != ' ') // Does it collide with part of the shield
            {
                // we collided
                shieldCollisionPoint.x = projectile.x - shield.position.x;
                shieldCollisionPoint.y = projectile.y - shield.position.y;

                return i;
            }
        }
    }

    return NOT_IN_PLAY;
}

void ResolveShieldCollision(Shield shields[], int shieldIndex, const Position& shieldCollisionPoint) {

    shields[shieldIndex].sprite[shieldCollisionPoint.y][shieldCollisionPoint.x] = ' ';
}

void InitAliens(const Game& game, AlienSwarm& aliens) {
    for (int row = 0; row < NUM_ALIEN_ROWS; row++) {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++) {
            aliens.aliens[row][col] = AS_ALIVE; 
        }
    }

    ResetMovementTime(aliens);

    aliens.direction = 1;
    aliens.numAliensLeft = NUM_ALIEN_ROWS * NUM_ALIEN_COLUMNS;
    aliens.animation = 0;
    aliens.spriteSize.width = ALIEN_SPRITE_WIDTH;
    aliens.spriteSize.height = ALIEN_SPRITE_HEIGHT;
    aliens.numberOfBombsInPlay = 0;
    aliens.position.x = (game.windowSize.width - NUM_ALIEN_COLUMNS * (aliens.spriteSize.width + ALIENS_X_PADDING)) / 2;
    aliens.position.y = game.windowSize.height - NUM_ALIEN_COLUMNS - NUM_ALIEN_ROWS * aliens.spriteSize.height - ALIENS_Y_PADDING *( NUM_ALIEN_ROWS - 1) - 3 + game.level;
    aliens.line = NUM_ALIEN_COLUMNS - (game.level - 1);
    aliens.explosionTimer = NOT_IN_PLAY;

    for (int i = 0; i < MAX_NUM_OF_ALIEN_BOMBS; i++)
    {
        aliens.bombs[i].animation = 0;
        aliens.bombs[i].position.x = NOT_IN_PLAY;
        aliens.bombs[i].position.y = NOT_IN_PLAY;
    }

}

void DrawAliens(const AlienSwarm& aliens) {
    const int NUM_30_POINT_ALIEN_ROWS = 1;
    //Draw 1 row of the 30 point aliens
    for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
    {
        int xPos = aliens.position.x + col * (aliens.spriteSize.width + ALIENS_X_PADDING);
        int yPos = aliens.position.y;

        if (aliens.aliens[0][col] == AS_ALIVE) {
            DrawSprite(xPos, yPos, ALIEN30_SPRITE, aliens.spriteSize.height, aliens.animation * aliens.spriteSize.height);
        }
        else if (aliens.aliens[0][col] == AS_EXPLODING) 
        {
            DrawSprite(xPos, yPos, ALIEN_EXPLOSION, aliens.spriteSize.height);
        }
    }
    //Draw 2 row of the 20 point aliens
    const int NUM_20_POINT_ALIEN_ROWS = 2;

    for (int row = 0; row < NUM_20_POINT_ALIEN_ROWS; row++)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
        {
            int xPos = aliens.position.x + col * (aliens.spriteSize.width + ALIENS_X_PADDING );
            int yPos = aliens.position.y + row * (aliens.spriteSize.height + ALIENS_Y_PADDING ) + NUM_30_POINT_ALIEN_ROWS * (aliens.spriteSize.height + ALIENS_Y_PADDING);

            if (aliens.aliens[NUM_30_POINT_ALIEN_ROWS + row][col] == AS_ALIVE) {
                DrawSprite(xPos, yPos, ALIEN20_SPRITE, aliens.spriteSize.height, aliens.animation * aliens.spriteSize.height);
            }
            else if (aliens.aliens[NUM_30_POINT_ALIEN_ROWS + row][col] == AS_EXPLODING)
            {
                DrawSprite(xPos, yPos, ALIEN_EXPLOSION, aliens.spriteSize.height);
            }
        }
    }
    //Draw 2 row of the 10 point aliens

    const int NUM_10_POINT_ALIEN_ROWS = 2;

    for (int row = 0; row < NUM_10_POINT_ALIEN_ROWS; row++)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
        {
            int xPos = aliens.position.x + col * (aliens.spriteSize.width + ALIENS_X_PADDING);
            int yPos = aliens.position.y + row * (aliens.spriteSize.height + ALIENS_Y_PADDING) + NUM_30_POINT_ALIEN_ROWS * (aliens.spriteSize.height + ALIENS_Y_PADDING) + NUM_20_POINT_ALIEN_ROWS * (aliens.spriteSize.height + ALIENS_Y_PADDING);

            if (aliens.aliens[NUM_30_POINT_ALIEN_ROWS + NUM_20_POINT_ALIEN_ROWS + row][col] == AS_ALIVE) {
                DrawSprite(xPos, yPos, ALIEN10_SPRITE, aliens.spriteSize.height, aliens.animation * aliens.spriteSize.height);
            }
            else if (aliens.aliens[NUM_30_POINT_ALIEN_ROWS + NUM_20_POINT_ALIEN_ROWS + row][col] == AS_EXPLODING)
            {
                DrawSprite(xPos, yPos, ALIEN_EXPLOSION, aliens.spriteSize.height);
            }
        }
    }
    if (aliens.numberOfBombsInPlay > 0) 
    {
        for (int i = 0; i < MAX_NUM_OF_ALIEN_BOMBS; i++)
        {
            if (aliens.bombs[i].position.x != NOT_IN_PLAY && aliens.bombs[i].position.y != NOT_IN_PLAY) 
            {
                DrawCharacter(aliens.bombs[i].position.x, aliens.bombs[i].position.y, ALIEN_BOMB_SPRITE[aliens.bombs[i].animation]);
            }
        }
    }
}

bool IsCollision(const Player& player, const AlienSwarm& aliens, Position& alienCollisionPositionInArray)
{
    alienCollisionPositionInArray.x = NOT_IN_PLAY;
    alienCollisionPositionInArray.y = NOT_IN_PLAY;

    for (int row = 0; row < NUM_ALIEN_ROWS; row++)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
        {
            int xPos = aliens.position.x + col * (aliens.spriteSize.width + ALIENS_X_PADDING);
            int yPos = aliens.position.y + row * (aliens.spriteSize.height + ALIENS_Y_PADDING);

            if (aliens.aliens[row][col] == AS_ALIVE &&
                player.missile.x >= xPos && player.missile.x < xPos + aliens.spriteSize.width &&
                player.missile.y >= yPos && player.missile.y < yPos + aliens.spriteSize.height) //Is the missile within the x boundary of the current alien
            {
                alienCollisionPositionInArray.x = col;
                alienCollisionPositionInArray.y = row;
                return true;
            }
        }
    }
    return false;
}

int ResolveAlienCollision(AlienSwarm& aliens, const Position hitPositionInAliensArray)
{
    aliens.aliens[hitPositionInAliensArray.y][hitPositionInAliensArray.x] = AS_EXPLODING;
    aliens.numAliensLeft--;

    if (aliens.explosionTimer == NOT_IN_PLAY) {
        aliens.explosionTimer = ALIEN_EXPLOSION_TIME;
    }

    if (hitPositionInAliensArray.y == 0) {
        return 30;
    }
    else if (hitPositionInAliensArray.y >= 1 && hitPositionInAliensArray.y < 3) {
        return 20;
    }
    else
        return 10;
}

bool UpdateAliens(Game& game, AlienSwarm& aliens, Player& player, Shield shields[], int numberOfShields)
{
    if (UpdateBombs(game, aliens, player, shields, numberOfShields)) 
    {
        return true;    
    }
    if (aliens.explosionTimer >= 0) 
    {
        aliens.explosionTimer--; //If explosionTimer == 0 -> explosionTimer = NOT_IN_PLAY
    }
    for (int row = 0; row < NUM_ALIEN_ROWS; row++)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
        {
            if (aliens.aliens[row][col] == AS_EXPLODING && aliens.explosionTimer == NOT_IN_PLAY) {
                aliens.aliens[row][col] = AS_DEAD;
            }
        }
    }
    aliens.movementTimer--;

    bool moveHorizontal = 0 >= aliens.movementTimer;
    int emptyColsLeft = 0;
    int emptyColsRight = 0;
    int emptyRowBottom = 0;

    FindEmptyRowAndColumns(aliens, emptyColsLeft, emptyColsRight, emptyRowBottom);

    int numberOfColumns = NUM_ALIEN_COLUMNS - emptyColsLeft - emptyColsRight;
    int leftAlienPosition = aliens.position.x + emptyColsLeft * (aliens.spriteSize.width + ALIENS_X_PADDING);
    int rightAlienPosition = leftAlienPosition + numberOfColumns * aliens.spriteSize.width + (numberOfColumns - 1) * ALIENS_X_PADDING;
     
    if (((rightAlienPosition >= game.windowSize.width && aliens.direction > 0) || (leftAlienPosition <= 0 && aliens.direction < 0)) && moveHorizontal && aliens.line > 0) {
        //Move down
        moveHorizontal = false;
        aliens.position.y++;
        aliens.line--;
        aliens.direction = -aliens.direction;
        ResetMovementTime(aliens);

        DestroyShields(aliens, shields, numberOfShields);

        if (aliens.line == 0) 
        {
            game.currentState = GS_GAME_OVER;
            return false;
        }
    }
    if (moveHorizontal) {
        aliens.position.x += aliens.direction;
        ResetMovementTime(aliens);
        aliens.animation = aliens.animation == 0 ? 1 : 0;

        DestroyShields(aliens, shields, numberOfShields);
    }

    if (!moveHorizontal) 
    {
        int activeColumns[NUM_ALIEN_COLUMNS];//columns that still exist - some aliens are still alive in that column

        int numActiveCols = 0;

        for (int c = emptyColsLeft; c < numberOfColumns; ++c)
        {
            for (int r = 0; r < NUM_ALIEN_ROWS; r++)
            {
                if (aliens.aliens[r][c] == AS_ALIVE) 
                {
                    activeColumns[numActiveCols] = c;
                    numActiveCols++;
                    break;
                }
            }
        }  
        if (ShouldShootBomb(aliens)) 
        {   
            if (numActiveCols > 0) 
            {
                int numberOfShots = ((rand() % 3) + 1) - aliens.numberOfBombsInPlay; // make sure that there are only 3 bombs in play

                for (int i = 0; i < numberOfShots; i++)
                {
                    int columnToShoot = rand() % numActiveCols;
                    ShootBomb(aliens, columnToShoot);
                }
            }
        }
    }
    return false;
}

void ResetMovementTime(AlienSwarm& aliens) 
{
    aliens.movementTimer = aliens.line * 2 + (5 * (float(aliens.numAliensLeft) / float(NUM_ALIEN_COLUMNS * NUM_ALIEN_ROWS)));
}

void FindEmptyRowAndColumns(const AlienSwarm& aliens, int& emptyColsLeft, int& emptyColsRight, int& emptyRowBottom)
{
    bool found = false;
    for (int col = 0; col < NUM_ALIEN_COLUMNS && !found; col++)
    {
        for (int  row = 0; row < NUM_ALIEN_ROWS && !found; row++)
        {
            if (aliens.aliens[row][col] == AS_DEAD) {
                if (row == NUM_ALIEN_ROWS - 1) //last row
                    emptyColsLeft++;
            }
            else {
                found = true;
            }
        }
    }
    found = false;

    for (int col = NUM_ALIEN_COLUMNS - 1 ; col >= 0 && !found; col--)
    {
        for (int row = 0; row < NUM_ALIEN_ROWS && !found; row++)
        {
            if (aliens.aliens[row][col] == AS_DEAD) {
                if (row == NUM_ALIEN_ROWS - 1)
                    emptyColsRight++;
            }
            else {
                found = true;
            }
        }
    }
    found = false;

    for (int row = NUM_ALIEN_ROWS - 1; row >= 0 && !found; row--)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS && !found; col++)
        {
            if (aliens.aliens[row][col] == AS_DEAD) {
                if (col == NUM_ALIEN_COLUMNS - 1)
                    emptyRowBottom++;
            }
            else {
                found = true;
            }
        }
    }
}

void DestroyShields(const AlienSwarm& aliens, Shield shields[], int numberOfShields)
{
    for (int row = 0; row < NUM_ALIEN_ROWS; row++)
    {
        for (int col = 0; col < NUM_ALIEN_COLUMNS; col++)
        {
            if (aliens.aliens[row][col] == AS_ALIVE) 
            {
                int xPos = aliens.position.x + col * (aliens.spriteSize.width + ALIENS_X_PADDING);
                int yPos = aliens.position.y + row * (aliens.spriteSize.height + ALIENS_Y_PADDING);

                CollideShieldsWithAlien(shields, numberOfShields, xPos, yPos, aliens.spriteSize);
            }
        }
    }
}

void CollideShieldsWithAlien(Shield shields[], int numberOfShields, int alienPosX, int alienPosY, const Size& size)
{
    for (int s = 0; s < numberOfShields; s++)
    {
        Shield& shield = shields[s];
        if (alienPosX < shield.position.x + SHIELD_SPRITE_WIDTH && alienPosX + size.width >= shield.position.x &&
            alienPosY < shield.position.y + SHIELD_SPRITE_HEIGHT && alienPosY + size.height >= shield.position.y )
        {
            //we're colliding
            int dy = alienPosY - shield.position.y;
            int dx = alienPosX - shield.position.x;

            for (int h = 0; h < size.height; h++)
            {
                int shieldY = dy + h;
                if (shieldY >= 0 && shieldY < SHIELD_SPRITE_HEIGHT) 
                {
                    for (int w = 0; w < size.width; w++) 
                    {
                        int shieldX = dx + w;
                        if (shieldX >= 0 && shieldX < SHIELD_SPRITE_WIDTH) 
                        {
                            shield.sprite[shieldY][shieldX] = ' ';
                        }
                    }
                }
            }
            break;
        }
    }
}

bool ShouldShootBomb(const AlienSwarm& aliens)
{
    //The more aliens we have destroyed the more they are going to drop bombs
    return int(rand() % (70 - int(float(NUM_ALIEN_ROWS * NUM_ALIEN_COLUMNS) / float(aliens.numAliensLeft + 1)))) == 1;
}

void ShootBomb(AlienSwarm& aliens, int columnToShoot)
{
    int bombId = NOT_IN_PLAY;    

    for (int i = 0; i < MAX_NUM_OF_ALIEN_BOMBS; i++)
    {
        if (aliens.bombs[i].position.x == NOT_IN_PLAY || aliens.bombs[i].position.y == NOT_IN_PLAY) 
        {
            bombId = i;
            break;
        }
    }
    for (int r = NUM_ALIEN_ROWS-1; r >= 0; r--)
    {
        if (aliens.aliens[r][columnToShoot] == AS_ALIVE) 
        {
            int xPos = aliens.position.x + columnToShoot * (aliens.spriteSize.width + ALIENS_X_PADDING) + 1; //middle of the aliens
            int yPos = aliens.position.y + r * (aliens.spriteSize.height + ALIENS_Y_PADDING) + aliens.spriteSize.height; //bottom of the alien
        
            aliens.bombs[bombId].animation = 0;
            aliens.bombs[bombId].position.x = xPos;
            aliens.bombs[bombId].position.y = yPos;

            aliens.numberOfBombsInPlay++;
            break;
        }
    }
}

bool UpdateBombs(const Game& game, AlienSwarm& aliens, Player& player, Shield shields[], int numberOfShields)
{
    int numBombSprites = strlen(ALIEN_BOMB_SPRITE);
    for (int i = 0; i < MAX_NUM_OF_ALIEN_BOMBS; i++)
    {
        if (aliens.bombs[i].position.x != NOT_IN_PLAY && aliens.bombs[i].position.y != NOT_IN_PLAY) 
        {
            aliens.bombs[i].position.y += ALIEN_BOMB_SPEED;
            aliens.bombs[i].animation = (aliens.bombs[i].animation + 1) % numBombSprites;

            Position collisionPoint;
            int shieldIndex = IsCollision(aliens.bombs[i].position, shields, numberOfShields, collisionPoint);

            if (shieldIndex != NOT_IN_PLAY) 
            {
                aliens.bombs[i].position.x = NOT_IN_PLAY;
                aliens.bombs[i].position.y = NOT_IN_PLAY;
                aliens.bombs[i].animation = 0;
                aliens.numberOfBombsInPlay--; 
                ResolveShieldCollision(shields, shieldIndex, collisionPoint);
            }   
            else if (IsCollision(aliens.bombs[i].position, player.position, player.spriteSize)) 
            {
                aliens.bombs[i].position.x = NOT_IN_PLAY;
                aliens.bombs[i].position.y = NOT_IN_PLAY;
                aliens.bombs[i].animation = 0;
                aliens.numberOfBombsInPlay--;
                return true;
            }
			else if (aliens.bombs[i].position.y >= game.windowSize.height)
            {
                aliens.bombs[i].position.x = NOT_IN_PLAY;
                aliens.bombs[i].position.y = NOT_IN_PLAY;
                aliens.bombs[i].animation = 0;
                aliens.numberOfBombsInPlay--;
            }
        }
    }
    return false;
}

bool IsCollision(const Position& projectile, const Position& spritePosition, const Size& spriteSize)
{
    bool result = (projectile.x >= spritePosition.x && projectile.x < (spritePosition.x + spriteSize.width) &&
        projectile.y >= spritePosition.y && projectile.y < (spritePosition.y + spriteSize.height));
	return result;
}

void ResetGame(Game& game, Player& player, AlienSwarm& aliens, Shield shields[], int numberOfShields)
{
    //game.waitTimer = WAIT_TIME;
    game.gameTimer = 0;
    ResetPlayer(game, player);
    ResetShields(game, shields, numberOfShields);
    InitAliens(game, aliens);
}

void ResetShields(const Game& game, Shield shields[], int numberOfShields)
{
    int firstPadding = ceil(float(game.windowSize.width - numberOfShields * SHIELD_SPRITE_WIDTH) / float(numberOfShields + 1));
    int xPadding = floor(float(game.windowSize.width - numberOfShields * SHIELD_SPRITE_WIDTH) / float(numberOfShields + 1));

    for (int i = 0; i < numberOfShields; i++)
    {
        Shield& shield = shields[i];
        shield.position.x = firstPadding + i * (SHIELD_SPRITE_WIDTH + xPadding);
        shield.position.y = game.windowSize.height - PLAYER_SPRITE_HEIGHT - 1 - SHIELD_SPRITE_HEIGHT - 2;

        for (int row = 0; row < SHIELD_SPRITE_HEIGHT; row++) {
            strcpy_s(shields[i].sprite[row], strlen(SHIELD_SPRITE[row]) + 1, SHIELD_SPRITE[row]);
        }
    }
}

void DrawGameOverScreen(const Game& game)
{
    string  gameOverStr = "Game Over!";
    string  pressSpaceStr = "Press Space Bar to Continue!!!";

    string namePromptString = "Please Enter your name:";

    const int yPos = game.windowSize.height / 3;
    
    const int  gameOverXPos = game.windowSize.width / 2 - gameOverStr.length() / 2; 
    const int pressSpaceXPos = game.windowSize.width / 2 - pressSpaceStr.length() / 2;
    const int namePromptXPos = game.windowSize.width / 2 - namePromptString.length() / 2;

    DrawString(gameOverXPos, yPos, gameOverStr);
    DrawString(pressSpaceXPos, yPos+1, pressSpaceStr);
    DrawString(namePromptXPos, yPos + 3, namePromptString);

    for (int i = 0; i < MAX_NUMBER_OF_CHARACTERS_IN_NAME; i++)
    {
        if (i == game.gameOverHPositionCursor)
        {
            attron(A_UNDERLINE);
        }

        DrawCharacter(game.windowSize.width / 2 - MAX_NUMBER_OF_CHARACTERS_IN_NAME / 2 + i, yPos + 5, game.playerName[i]);

        if (i == game.gameOverHPositionCursor)
        {
            attroff(A_UNDERLINE);
        }
    }

}

void DrawIntroScreen(const Game& game)
{
    string startStr = "Welcome to Text Invaders!";
    string pressSpaceStr = "Press Space Bar to Start!!!";
    string pressSString = "Press (s) to go to the high scores";

    const int yPos = game.windowSize.height / 2-2;
    const int startStringXPos = game.windowSize.width / 2 - startStr.length() / 2;
    const int pressSpaceXPos = game.windowSize.width / 2 - pressSpaceStr.length() / 2;
    const int pressSXPos = game.windowSize.width / 2 - pressSString.length() / 2;


    DrawString(startStringXPos, yPos, startStr);
    DrawString(pressSpaceXPos, yPos + 1, pressSpaceStr);
    DrawString(pressSXPos, yPos + 2, pressSString);

}

void DrawCountDownScreen(const Game& game)
{
    string startStr = "Count Down Timer!";
    string pressSpaceStr = "Please wait until time is off!!!";

    const int yPos = game.windowSize.height / 2 - 2;
    const int startStringXPos = game.windowSize.width / 2 - startStr.length() / 2;
    const int pressSpaceXPos = game.windowSize.width / 2 - pressSpaceStr.length() / 2;

    DrawString(startStringXPos, yPos, startStr);
    DrawString(pressSpaceXPos, yPos + 1, pressSpaceStr);
    mvprintw(0, yPos + 3, "CountDown: %i", game.waitTimer);
}


void ResetUFO(const Game& game, AlienUFO& ufo)
{
    ufo.size.width = ALIEN_UFO_SPRITE_WIDTH;
    ufo.size.height = ALIEN_UFO_SPRITE_HEIGHT;

    ufo.Points = ((rand() % 4) + 1) * 50;

    ufo.position.x = NOT_IN_PLAY; //no UFO on the screen
    ufo.position.y = ufo.size.height;
}

void PutUFOInPlay(const Game& game, AlienUFO& ufo)
{
    ufo.position.x = 0;
}

void UpdateUFO(const Game& game, AlienUFO& ufo)
{
    ufo.position.x += 1;

    if (ufo.position.x + ufo.size.width >= game.windowSize.width) {
        ResetUFO(game, ufo);
    }
}

void DrawUFO(const AlienUFO& ufo) 
{
    if (ufo.position.x != NOT_IN_PLAY) 
    {
        DrawSprite(ufo.position.x, ufo.position.y, ALIEN_UFO_SPRITE, ALIEN_SPRITE_HEIGHT);
    }
}

void ResetGameOverPositionCursors(Game& game)
{
    game.gameOverHPositionCursor = 0; //first letter
    for (int i = 0; i < MAX_NUMBER_OF_CHARACTERS_IN_NAME; i++)
    {
        game.playerName[i] = 'A';
        game.gameOverVPositionCursor[i] = 0; //be at 'A'
    }
}

void AddHighScore(HighScoreTable& table, int score, const string& name)
{
    Score highScore;
    highScore.score = score;
    highScore.name = name;

    table.scores.push_back(highScore);

    sort(table.scores.begin(), table.scores.end(), ScoreCompare);

    SaveHighScores(table);
}

bool ScoreCompare(const Score& score1, const Score& score2)
{
    return score1.score > score2.score; //descending order
}

void DrawHighScoreTable(const Game& game, const HighScoreTable& table)
{
    string title = "High Scores";
    int titleXPos = game.windowSize.width / 2 - title.length() / 2;
    int yPos = 5;
    int yPadding = 2;

    attron(A_UNDERLINE);
    DrawString(titleXPos, yPos, title);
    attroff(A_UNDERLINE);

    for (int i = 0; i < table.scores.size() && i < MAX_HIGH_SCORES; i++)
    {
        Score score = table.scores[i];
        mvprintw(yPos + (i + 1) * yPadding, titleXPos - MAX_NUMBER_OF_CHARACTERS_IN_NAME, "%s\t\t%i", score.name.c_str(), score.score);
    }
}

void SaveHighScores(const HighScoreTable& table)
{
    ofstream outFile;

    outFile.open(FILE_NAME);

    if (outFile.is_open())
    {
        for (int i = 0; i < table.scores.size() && i < MAX_HIGH_SCORES; i++)
        {
            outFile << table.scores[i].name << " " << table.scores[i].score << endl;
        }

        outFile.close();
    }
}

void LoadHighScores(HighScoreTable& table)
{
    ifstream inFile;

    inFile.open(FILE_NAME);

    string name;
    int scoreVal;

    Score score;

    if (inFile.is_open())
    {
        while (!inFile.eof())
        {
            inFile >> ws;
            if (inFile.eof())
            {
                break;
            }

            inFile >> name >> scoreVal;
            score.name = name;
            score.score = scoreVal;

            table.scores.push_back(score);
        }

        inFile.close();
    }
}