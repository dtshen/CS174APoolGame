![ScreenShot](https://raw.github.com/dtshen/CS174APoolGame/screenshots/1.jpg)                                              
                                                                     
                                             
Paul Rodriguez
Dennis Shen
Wei Zou

CS174A project
This requires an installation of openGL to run.
our advance topic consists of collisions of balls with walls and other balls.

our project is a billiards game, where the objective for each player is to get the black ball number 8 into one of the pockets
to win the game.

the player can use the mouse to rotate around the pool table and use the left and right arrow keys to control the direction
of where to hit the ball.

by pressing and holding the SPACEBAR, the user can charge up the power for hitting the ball and releasing it will cause the ball
to move at that specific speed.

the balls can bounce off the sides of the table and also off other balls, and if they get into one of be pockets/baskets then the 
balls disappear since they wont be needed until another game.

the positions of the balls are update per frame where a timer is used for efficient velocity update since performance seems to decrease when more objects are drawn and it increases when less objects are draw.

collision detection between balls are iterated multiple times to prevent balls from jumping very far.

every time a player puts a ball in a pocket, then they are assigned either striped or colored, the two types of balls avaiable and if 
they put another ball of the same type into the pocket then they can shoot again, or if they miss then it skips to the other player's turn.

CONTROL KEYS:
Press:
  q to quit.
  r to restart game.
  w,s to move camera up and down.
  a,d to move camera left and right.
  left and right directional arrows to control angle of ball to hit.
  up and down directional arrows to move camera forward and back press.
Hold: 
  spacebar to power up hitting of the ball.

ADVANCED TOPIC:
our advance topic consists of collision with walls and also with other balls.
the collisions are implemented in the files "Sphere.h" and "Sphere.cpp" and they are checked in the idle function of the 
program every time a ball is moving.
collision between balls are done before any movement of balls and every pair is checked about 50 times.
ball-to-ball collision is done by basically checking that the distance between two spheres are less than the sum of their radius.
if they do collide then we must move them back so that they are just touching since they might overlap, then we compute the 
new velocities using conservation of momentum and conservation of energy by finding the tangential and normal components of old velocities so that we can use them to find the new velocities by solving the equations.

FEATURES:
the features include everything covered in the first half of the quarter: shading, lightning, texture mapping, rotation, animation.
quaternions are used to rotate the balls with appropriate velocities and angular speed.
the pool table consists of many complex shapes

THINGS TO IMPROVE:
sometimes collision does not work very well when three or more balls intersect, maybe because we need to calculate which collision is the earliest.
gravity when balls fall into the pockets could have implemented better

![ScreenShot](./screenshots/2.jpg)
![ScreenShot](./screenshots/3.jpg)
![ScreenShot](./screenshots/4.jpg)
![ScreenShot](./screenshots/5.jpg) 
