Project name: Missile Exterminatus
Collaborators: Paul Coley, Erik Puesey
Additional Setup: Requires the LUA header files and library on the proper file path (LUA_HOME) to compile, as well as lua52.dll in the same folder as the executable to function. These can be found at http://sourceforge.net/projects/luabinaries/files/5.2.3/Windows%20Libraries/Dynamic/ with the download compiled with VS12.

Goal: Defeat as many of the fishy missiles as possible before they overwhelm you

Lose Condition: Get hit by 10 of the fishy missiles

Controls:

	W: Move forward
	A: Move left
	S: Move backward
	D: Move right

	Space: Fire the death laser to destroy fishy missiles

	Mouse Movement: Aim your death laser

Requirement Satisfaction:

 In this prototype we opt to lock the camera onto the player controlled character
to provide a third person shooter type feel.  This type of game is different than the
other assignments this semester due to the overall goal and how the player is directly
in control of the player agent.  The fish swim towards the player character using the
boids model until they reach within a set distance where they ignore their seperation
in an attempt to defeat the player.  Every ten seconds a power up spawns in a random position
on the map and has a chance to increase the player's speed, but can also be a dud with
no change in speed.  Every five seconds a new wave of fish spawn with the number of fish
per spawner increasing by one every ten seconds.  To kill the fish the player fires a ray
projected from the head of the player model that then kills any fish it intersects with.
To show the player how long they survived and the amount of fish they have killed a gui
panel is shown.  Upon the tenth fish hitting the player the timer stops and a game over
button is shown along with the cursor that ends the program when clicked.

Contribution Divisions:
 We initially used the level loading project as the base for this project and modified
it as needed.

 Paul Coley:
   Fish animation
   Source file shell construction
   LUA inclusion
   Level Loader changes
   WASD movement
   Boids model to missiles
   PowerUp internal implementation
   Missile Spawner Implementation
   Projectile Destruction on ray intersection
   Ray tracing implementation
 Erik Puesey:
   New Header file construction
   New class constructor methods
   Initial AddTime and update method frameworks
   Missile movement and tracking
   Bounding Box collision detection
   GUI implementation
   PowerUp visual implementation
   Lose condition implementation
   Time tracking implementation
   Visual firing cue implementation