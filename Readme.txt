--------------------------------------------------------------------Space Invaders--------------------------------------------------------------------------------------

(from: http://www.classicgaming.cc/classics/space-invaders/play-guide)

The object of the game is to shoot the invaders with your missile base while avoiding their shots and preventing an invasion.
Amassing a high score is a further objective and one that must be prioritised against your continued survival.
Each game screen starts with five rows of 11 invaders.
The bottom 2 rows score 10 points per large invader, the middle 2 rows 20 points for each medium invader and the top row scores 30 points for
each small invader. This makes each screen worth a total of 990 points.
The score for each screen can be dramatically increased by the destruction of the 'mystery' value spaceships that frequently fly from
one side of the screen to the other. The mystery ufo should have a points value of 50 to 200 points.
Temporary sanctuary can be sought beneath the 4 base shields. Shields are destructable by both the player and the aliens. Only the part of the
shield hit by either a player missile or an alien bomb should be destroyed not the entire shield.

The invaders travel sideways and each time they touch the side they drop down 1 line.
On level 1 they need to drop 11 lines to reach the bottom and 'invade'.
From level 2 through to screen 9 they start progressively lower down the screen.
At level 10 the game reverts to the level 1 start position and the cycle begins again.

There is no time limit, except for the fact that if you do not shoot them all before they reach the bottom of the screen the game ends.

The more invaders the player shoots, the faster the remaining invaders move.
When the invaders are low enough to touch the base shelters they erase them as they pass. The last invader moves very quickly.

Any bombs dropped by an invader on the row above 'invasion' row will not harm the player's missile.

The aliens can have a maximum of 3 bombs on screen at any one time. The player can have only 1 missile shot on screen at any one time.

If the player is hit, the game should pause, and the player should be showing the exploding animation.

If an alien is hit, they should explode for a short period of time then disappear.

There should be an intro screen, game over screen (where the player can enter their name), and high score screen

There should be a high score table that can be seen from the initial welcome screen and after the game is over and the player enters his/her 3 letter name


All regular aliens are 2 by 4 in terms of size (2 rows of 4 columns)
Player is 2 by 5
Alien UFO is 2 by 6

Alien sprites:
30 point aliens:  /oo\  /oo\
				  <  >  /""\

20 point aliens:  ><  |><|
				 |\/| /  \

10 point aliens: /--\  /--\
				 /  \  <  >

Alien Explosion: \||/
				 /||\

alien bombs: \ | / -

Alien UFO: _/oo\_
		   =q==p=

Player sprite:  =A=
			   =====
					0    1
Player explosion: ,~^.' '+-`.
				  ===== =====

Shield: /IIIII\
		IIIIIII
		I/   \I


General Data:
-------------
Player
Aliens
Shields
Alien bombs
Player missiles
Game states
Scores
High Score table


Player
------
Position
Missile - position
Animation State
Size of the sprite
Lives (3)
Score

Shields
-------
Sprite - the sprite can change
Position

Alien Swarm (all the 10, 20, and 30 point aliens)
-----------
Position
direction (left or right)
animation state
size of each alien
State of each alien - alive, dead, or exploding
Alien bombs

Alien UFO
---------
size
position
points

Position
--------
x, y coordinate

Alien Bomb
---------
position
animation state - \ | / -

Missile (sprite - |)
-------
Position

Game
-----
Game State
Level
size of the window

Score
-----
score value
name

High Score table
----------------
vector of scores