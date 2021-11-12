# Assignment 2 Justification

Demonstration of (almost) all features implemented in this project can be viewed in these two videos:
https://www.youtube.com/watch?v=xIFnil_tP2s
https://www.youtube.com/watch?v=cTxstJ59Y9I

Basic Minecraft Clone by z5309206
CONTROLS:

Move mouse to move view
WASD to move
Double tap W to run
SPACE to jump
LEFT CLICK to break the highlighted block
RIGHT CLICK to place the block in your current hand
MIDDLE CLICK to quickly put the put that you've highlighted into your hand
MOUSE WHEEL to scroll through hotbar
LEFT SHIFT to sneak (Same functionality as in Minecraft, where you can not fall off blocks in sneak state. Sneaking will also stop highlighting blocks so that you can see the specular reflections better)
TAB to maximise/minimise game window
C to sleep and wake up (Toggles between player and cutscene camera)
F to toggle flight mode (Toggles between player and flying camera)
G to print debug information into terminal, such as framerate player position etc.
O to convert the current world into lines of code so that it can be used as a preset world
E to change from the two hotbars (general blocks -> wool blocks and vice versa)
I to see these instructions ingame
ESC to terminate game

When the player's pitch angle is between a certain threshold, blocks such as the oak log and barrel will rotate based on where the player is standing when they first palced the block.

I am aiming to obtain marks in all sections of this Assignment

## Section 1: Creation and Use of 3D Objects

### 1a: Show that you have created or imported at least one 3D object that is visible in your application. Include references to where this is implemented in your code.

When the tutor selects a preset Minecraft World, the game will load in desired preset by loading in the correct 3D objects. The tutor should be able to see the creation of these 3D objects (the texture co-ordinates, vertices, indices etc.) in shapes.hpp and shapes.cpp. These functions are all called by scene.hpp within the constructor of the world struct, which is between lines 224 -> 480 in scene.hpp. No shapes has been imported from .obj files in this Minecraft implementation. All were created within the program.


### 1b: Describe your scene graph and the hierarchy within the scene. Include references to where this is implemented in your code.

There is a commented part between line 432 -> 449 which describes the scene graphs used. This program has 3 main scene graphs.

Screen, which contains all the 2D shapes and 3D shapes that needs to be rendered on the screen at all time.
Screen scene graph is constructed within scene.hpp from line 319 -> 431
Graph: Screen Node -> Hand
                   -> Flying icon
                   -> Instructions
                   -> Hotbar -> All 9 items in the hotbar

Centre of the World, which contains every object that needs to be moved in relation to where the player is.
Centre of the World scene ghraph is constructed within scene.hpp from line 237 -> 318.
Graph: Centre of World Node -> Skybox
                            -> Sun -> Sun Aura
                            -> Moon -> All hundreds of stars -> Some stars have an aura around it as well

Bed node, which contains all the shapes for the player model and the bed which the player rests on.
Bed scene graph is constructed within scene.cpp within the function "node_t createBedPlayer" at line 191.
Graph: Bed -> Centre of Player Node -> Head
                                    -> Torso
                                    -> Left Arm
                                    -> Right Arm
                                    -> Left leg
                                    -> Right leg

### 1c: Describe how your objects are moving or animating in realtime. Include code references (line numbers and function names) to use of delta time and also how this affects the scene graph.

A lot of the animation is dealt within scene.hpp. The main loop would call on the functions inside the world struct to animate certain objects by passing in delta time as one of the parameters (delta time calculation can be found in utility.cpp "time_delta" function). The functions which animates using delta time are:

void updatePlayerPositions(GLFWwindow *window, float dt, renderer::renderer_t *renderInfo); [Line 1049 -> 1218 scene.hpp]
This deals with changing the perspective/camera angle of the player while also moving them based on input controls. The player movement is based on delta time. The gravity which is enacted on the player when there is no blocks underneath them/they aren't in flying mode is also calculated in delta time, and the camera is moved downwards accordingly [Line 1169 -> 1184]. As the gravity and players vertical velocity is dependant on delta time, the player will not jump higher if they have a faster computer nor will they jump lower if they have a slower machine.
This function also branches out to call other animating functions as well and passes in parameters such as delta time.

void updateCameraAngle(playerPOV &cam, GLFWwindow *win, float dt); [Line 18 -> 42 player.cpp]
This function updates the camera's yaw and pitch depending on how far the cursor has moved. The delta time is used so that the new offset will scale based on how fast the program is responding, making it so that rotating the camera around will take the same amount of time in any machine.

void drawScreen(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo); [Line 1478 -> 1485 scene.hpp]
This function repositions the main node of the Screen Scene Graph to the player camera position, and then rotates it on the camera's pitch and yaw.
As we are animating/transitioning the main parent node of the Screen Scene Graph, all the children will be rotated as well, keeping the hotbar, hand etc. infront of the player's screen as well. The camera yaw and pitch is also updated with delta time, and by extension, the screens will also rotate around on delta time so that it's insync with being infront of the player's view.

void bobHand(float dt); [Line 714 -> 742 scene.hpp]
This function animates the player's hand bobbing up and down based on the player's movement. This also uses a bezier curve with delta time to determine the Y position of the hand. As this hand is the children of the main Screen Scene Graph, other elements such as the hotbar is not affected by this animation.

void swingHand(dt); [Line 752 -> 790 scene.hpp]
This function checks if the player has used either left or right mouse click. If the player has, it will restart the animation of the "Hand" within the Screen Scene Graph. It uses delta time on two separate bezier curves to animate its swinging and rotating animation respectively. As this hand is the children of the main Screen Scene Graph, other elements such as the hotbar is not affected by this animation.

void updateSunPosition(float degree, glm::vec3 skyColor, float dt); [Line 703 -> 709]
This function animates the sun based on the given degree value (rotation of the sunlight to the player). The degree value is already based on delta time (can be seen in main.cpp line 501 -> 505), so this function directly plugs in the new degree value into the rotation of the main Scene Node in the CentreOfWorld Scene Graph. As the parent is being rotated, all the children such as the Moon, Sun, Stars and Sky Box/Sphere is rotated around in relation to the centre of the World. The delta time is used for animating the stars.

void tickStars(float dt);
This function is called for all of the children of the Moon. Using delta time, the stars rotate at a constant rate regardless of framerate and also flicker at random. As the stars are rotating, so will their aura children if they have any.


### 1d: Describe how you have used parametric splines in your application. Include code references to the implementation of control point based curves in your project.

Parametric spline (cubic bezier) can be found in utility.ccp between Line 81 -> 87. This function is used to figure out the speed of the camera movement (Ease in, Ease out), the path of the camera, the path of hand swinging/bobbing. These can be all found in scene.hpp in the bobHand() function found between Line 714 -> 742, swingHand() between Line 752 -> 790, and animateCutscene() between Line 611 -> 654. These functions all contain the control points to be passed into the utility::cubicBezier function.

bobHand uses the Y position of a cubic bezier as the Y position of the hand. [Line 714 -> 742]
swingHand uses the Z position of the cubic bezier for the hand's rotation and the position of another bezier curve for it's new translation location. [Line 752 -> 790]
animateCutscene uses a Bezier curve to ease in and ease out the cutscene camera from start to finish, and also uses a sideways "S" shape for the camera's path. [Line 611 -> 654]

## Section 2: Camera

### 2a: Describe what controls the player should use to move the camera in your scene. Include code references to the implementation of delta time control of speed and use of user input in your project.

The player should use:
W to move forward in the direction they are facing
A to move to their immediate left
D to move to their immediate right
S to move backwards. These controls are bound to the X Z axis.
To move in the Y axis, the player should use SPACE. This is bound by gravity however, and eventually the camera will come back down. All cameras can not clip into objects besides the cutscene camera.

If the player is using the flying camera, the controls remain the same except shift is used to descend and space is used to ascened with no penalty from gravity.

If the player is using the cutscene camera, all movement controls are restricted from the player. If the player wishes to exit the cutscene, they should press C again.

All camera movement related calculations can be found in scene.hpp updatePlayerPositions() function between Line 1049 -> 1218. This includes the collision/gravity calculations. Delta time is used to control the player's camera speed so that the time it takes to move around will be the same on any machine regardless of performance. Delta time can be seen being used at line 1065 and line 24 of player.cpp.

### 2b: Describe how your camera interacts with the scene. Is it able to "walk" on the Z-X plane or follow terrain? Include code references.

updatePlayerPositions() checks for any user input. If an input is pressed, it will advance the player in the respective direction. But if the program detects that the players new position is within a block, then it will readjust the players position accordingly. This does mean that gravity enacts on players even though they are on ground level. If there aren't any blocks beneath the player's hitbox of 0.25 in radius, then the players vertical velocity will start to increase exponentially (capping at 50 units and follows delta time) and the camera will continue to move in relation to the vertical velocity until it reaches out of bounds or it detects a solid block beneath the player's hitbox. As such, this gives an illusion of the player following a terrain that they can shape at their own will.

updatePlayerPositions() receiving player input can be found between Line 1088 -> 1146. This is where the collision detection so that the camera can follow the terrain mostly happens.
The gravity/player vertical velocity calculation can be found between line 1168 -> 1182. The rest of the code is mainly to prevent the player from going out of bounds or to cover little loopholes in this system.

If the player wishes to marvel at their creation by being able to move freely in any axis, the player can press F to toggle to flying camera, where they won't be bound to gravity (However they still won't be able to clip through blocks. This is to replicate the flight camera found in real Minecraft). The "Space to ascend and Shift to descend" control scheme is also borrowed from Minecraft.

### 2c: Describe how the view of your scene can change from player controlled to application controlled (specify user input that makes this change happen). Include code references to use of parametric curves and delta time in the application controlled camera.

The cutscene camera I've created is to show the player sleeping and time advancing ahead faster than usual to simulate sleep. To switch to the cutscene camera, the player should press C. This will copy the player camera's position, ptich and yaw and the pastes it onto the cutscene camera [This is found in line 554 -> 571 of scene.hpp]. Then, it calculates the required yaw and pitch of the final position of the cutscene camera. As the cutscene camera is planned to move directly forward of the player while rotating 180 degrees, the control points for the bezier curve must adapt and shift [Found in line 623 -> 628 & 638 -> 641 of scene.hpp]. Next it calculates the delta time between the time the cutscene first played to the current time and is scaled down by a quater. It uses this delta time to calculate the new yaw and pitch of the current frame's cutscene camera and also the new position. The yaw and pitch are also controlled by an Ease In Ease Out bezier curve so that the cutscene camera moves more smoothly. All these calculations can be found in the funciton animateCutscene() found in scene.hpp between line 611 -> 654.

The control point for the easing in and out of the camera rotation can be found between line 631 -> 636.
The control points for the S shaped path of the cutscene camera can be found between line 623 -> 628. This also takes in the player camera's yaw so that the bezier curve can move directly infront of the player and as such the cutscene camera emerges forward from the player camera's original view.

If the player wishes to end the cutscene/wake up and take control of the player, they should press C again.

## Section 3: Lighting

### 3a: Describe where Diffuse Lighting is clear to see in your application. Include code references to your implemented lighting (including where this is implemented in your shaders).

Diffuse lighting can be immediately seen from the directional lighting from the Sun which is positioned directly above the world when the game first starts. If the sun is above, bright white light is used and all faces with normals facing up will be lit up. When the sun moves around, the diffuse lighting color changes to simulate sunset and night. When the sun is directly below, the objects will have their bottom side lit up and all other sides will have the ambient lighting (which also adapts to where the Sun is). renderer.hpp has a renderer struct which can be initialised to set pointers to the sun location inside the shader default.frag. If the object does not have any normals, it diffuse lihting will not apply to it (This is to trick the player into thinking something is brighter than the others). (This is found on line 83 -> 85 of default.frag.). The normals are also normalised and readjusted to fit the rotation/position of the object in default.vert.

From line 93 -> 94 in default.frag, the shader takes in the sun's color (which adapts to the day cycle) and multiplies it with the object's diffuse. Then it finds the dot product of the normal of the fragment and the sun's dirction. If it is less than 0, then the light should not be hitting this fragment and thus the max() function will return 0 and thus the color of that fragment will just be ambient + 0. If is more than 0, then the diffuse lighting will scale depending on how "parallel" the uSun direction is with the normal and thus gives an illusion of lighting. (tThe dot product finds what amount of one vector contributes another vector. Thus, if two vectors are going the same direction, then 100% of one vector contributes to the other and thus the max value is found from their dot product. As such normals that are parallel with the sun with achieve max dot product and thus have a brighter diffuse lighting applied).

The ambient + diffuse + result of all point lights will be used to calculate the color of the fragment, giving an illusion of lighting.

### 3b: Describe where Specular Lighting is clear to see in your application, including how the camera position affects the lighting. Include code references to your implemented lighting (including where this is implemented in your shaders).

Specular light is most clear on objects with specular maps. If the tutor chooses the preset world "Parkour Course" at the beginning of the program, they should see "Glowstone Blocks" emitting light which can be found reflecting off the white blocks called "Block of Iron". It will be even more obvious if the tutor advances the time by pressing "C" and wait for night time, where there will be less diffuse lighting and more point light which in this program uses specular lighting. Another way to see specular lighting would be to place these glowing blocks near blocks with specular maps and also wait till night time for added impact. The list of glowing blocks can be found in the point light section below. The list of blocks with specular mapping to them are:

- Barrel
- Crafting Table
- Block of Iron
- Block of Copper
- Block of Gold
- Block of Raw Iron
- Block of Raw Copper
- Block of Raw Gold
- Diamond Ore
- Emerald Ore
- Lapis Lazuli Ore
- Iron Ore
- Copper Ore
- Coal Ore
- Gold Ore
- Glass Block
- Block of Marc

The camera position will determine which fragment will be affected by the specular lighting. The lit parts will seem to follow where the camera is to add an illusion of the object's shininess. This is achieved by havig a uniform variable in default.frag point to the camera's position and then finding the normalised vector between the camera position and the position of the fragment. This vector is then dot producted with the reflection vector to determine if the specular map should be lit up or not, similarly to the technique used for the diffuse lighting. (Reflction vector would be the direction from the point light to the block face reflected along the normal of that face). Specular lighting calculations can be found in default.frag line 66 -> 78.

### 3c: Describe which of your light(s) in your scene are point light(s) and how you have placed objects in the scene to clearly show the point lights functioning. Include code references to point light implementation.

The point lights in my scene can either be generated by the player placing down a glowing block or generated from a preset (The only preset to include point lights is the "Parkour Course"). The parkour course has lighting on the base platform to clearly show off the point light shining onto a block with specular maps. The player can also destroy a point light by breaking its respective glowing block. In other words, point lights can be found in the following blocks:

- Glowstone
- Crying Obsidian
- Sea Lantern
- Redstone Ore
- Magma Block

Each block has different colored point lights. The tutor is encouraged to create their own scene with the point lights in the spirit of authentic Minecraft. If they aren't able to, they can use the Parkour Course as a reference for how point lights are placed in a scene (Mainly near other blocks with specular mapping to show off the lighting effect). Point lights have the most impact during ingame night time.

A caveat to this implementation of Minecraft is that there can only be a maximum of 101 lights at any time, as default.frag shader as a #define of MAX_LIGHTS 101. This can be increased of course at the expense of framerate. (If this is changed, the #define in renderer.hpp at line 14 must also be changed).

How the placement of point light occurs is by calling the addLightSource() function in renderer.hpp between line 203 -> 227. This function is called during placing blocks. (Setting up the uniform location between these light sources and the point light location in the default.frag can be found between line 108 -> 118). This function finds a position in the "allLightSources" array, which contains a struct lightSource with the uniform locations of each array cell in the default.frag shader. Then, it copies over the block's position, color and intensity over into the shader and then returns the index of this light inside the renderer array to be stored inside the node_t containing the light source. When breaking a light source using removeLightSource() (which is called whenever a player breaks a block) in renderer.hpp between line 234 -> 241, the node_t containing the block with the light to be removed will give its light index and then is invalidated by moving its position out of bounds so that the default.frag will ignore that point light when drawing.

Line 102 -> 108 shows the shader picking and choosing which point light to render.

If the tutor wants to experiment with point lights, it is best to use a preset world with a lot of white color such as "Iron World" or "Wooly World".

### 3d: Describe all the lights in your scene and what type they are. Include code references to implementation of the capability to work with an arbitrary number of lights.

The lights in my scene come from the sun and any light source blocks placed into the terrain.

I have used directional light for the sun and up to 101 point lights for blocks which emit light in all directions. Directional light is used for the sun as directional light is best used to light up the entire scene, which is perfect for the sun. Point light is used for blocks as they aren't meant to impact all surrounding blocks (only blocks close enough) and also do not have a set direction needed to be lit up. 

Directional light impacts the diffuse lighting of an object by their diffuse/texture map.
Point lights impacts the specular lighting of an object by their specular map.
Ambient lighting is also used so that when the directional lighting is out of sight or there aren't any point lights, the scene is still lit up to a degree.

My project allows up to 101 point lights and only one directional light for the sun. All preset worlds do not feature point lights at the start of its generation besides the Parkour Course map. This is to encourage the player to create their own scene and use an arbitrary number of point lights. The arbitrary number of lights is made possible by the "allLightSources" array seen in renderer.hpp at line 67. The initialise function, specifically line 108 -> 117 in renderer.hpp, points all cells in this array to their respective cell in the frag shader found in default.frag line 56. Then, the frag shader gets to pick and choose which light to render by looping through all 101 point lights and seeing if they are within bounds. If they aren't, then the point light is not rendered. If they are, then they are rendered and will have affect on surrounding blocks and models. This creates an illusion that the player can work with an arbitrary number of lights to create their scene (however they are warned that only up to 101 lights can be placed throughout their scene).

Side note: line 177 -> 193 of renderer.hpp sets the pointers from shader's array to the renderer's array data.

### 3e: Explain what maps you have used and which objects they are on. Your application should clearly show the effect these maps are having on lighting. Include code references to maps on objects and vertices as well as lighting code that uses the maps.

The two maps I have used are diffuse maps and specular maps. I did not use normal maps for this project. The effect of diffuse map can be greatly seen when the player enters cutscene mode to sleep by pressing "C". The world will light up based on the colors found on the diffuse map and the dirction in which the sun is currently at. Additionally, tutor is encouraged to place down glowing blocks to see its lighting effects on the surrounding blocks.

The directional lighting by the sun uses the diffuse map for diffuse lighting. Maps are loaded in using the init function of texture_2d, which returns the handler to that texture, which is stored in the node_t struct. Specular lighting uses specular maps for it's lighting calculations. Passing the texture and specular map into the shader can be seen in renderer.hpp between line 190 -> 192 where it determines the location of each map in the shader. Then, when drawing objects in scene.cpp between line 8 -> 85, the code binds the node's diffuse and specular to the shader. The map co-ordinates and indices can also be seen passed in duing the initialisation of the node's mesh seen in static_mesh.cpp in a vertex buffer object and an element buffer object respectively so that when drawing the node, it will use the data stored in these buffers for the indices and map co-ordinates.

The lighting code can all be found within the default.frag shader file, which takes in the direction of the light source (For point light's case, it will be the vector from light to block position) and dot products it with the block face's normal to determine how much effect the light will be on the fragment. The lighting also factors in the mappings as well.

For point light and specular lighting found between line 66 -> 79 in default.frag, it will take in the specular map passed into uniform sampler2D uSpec at line 93 to find the material's specular and then multiply it with the point light's specular and the dot product between reflection and view at line 73. This results in dark spots in the specular map not being accounted for the specular lighting and thus we can control which parts of a shape will be impacted by specular lighting or not via specular map. Point light also takes into account of the distance between the light and the fragment. If it is too far, the fragment won't be affected by it. This is done by finding the distance to calculate the attenuation which is used for the final point light value (found in line 76 of default.frag).

For diffuse lighting and directional lighting calculations found between line 86 -> 90 in default.frag, it will take in the diffuse map passed in earlier into uniform sampler2D uTex and pass in the color in the diffuse map and store it in vec4 color. This is the base color after all the lighting calculations such as the ambient and diffuse lighting. The ambient + diffuse + all the point lights is then multiplied with the rgb values of the color that was calculated by the passed in diffuse map to then find the final color of the fragment.


All maps can be found in the textures folder inisde res.

## Section 4: Subjective Mark

### How have you fulfilled this requirement?

Following the theme of my previous assignment, I wanted to recreate Minecraft as much as possible within the given time frame and requirements. As such, I had to implement collision detection and controls that are as close as possible with the controls Minecraft had back in its early development stage. (This includes sneaking, the way the flying camera is controlled, the mouse controls, different moon phases, scrolling through hotbar mechanic etc.) As I was recreating Minecraft, I was compelled to add in the ability to shape the scene with the blocks implemented, just like in Minecraft. I also added a few twists not part of modern Minecraft such as a camera that pans out when the player is sleeping and specular lighting on blocks (and also the fabled Block of Marc).

I've also tried to implement the various optimisation techniques found in Minecraft, such as Frustum Culling (My program doesn't use real Frustum Culling, but it has code found in frustum.cpp which replicates Frustum Culling) and also does not render faces that are obscured by a block (This can be seen when placing two transparent blocks like the glass block side by side, which makes the face that is inbetween the two blocks invisible).

### What is your vision for the scene you are showing?

I wanted to show classic Minecraft during its early stages of development to evoke a sense of nostalgia. I also wanted my project to be able to fool a few people into thinking this was actually Minecraft back in early development stages (I was not successful with this). The game also comes with 5 preset worlds, which is to suit to whatever scene the player wants (If they fancy creating builds on a blank canvas, they may choose Wooly World or Iron World. If they want to experience nostalgic Minecraft, they may choose Basic Super Flat World or Classic Sky Block. If they want a challenge using classic Minecraft controls, they may choose the Parkour Course preset). Just like with the original Minecraft, the vision of the scene is the desired vision and creativity of the player.

### Are you referencing anything in this scene? Is this a quote or homage to any other art? Please describe it.

I am referencing Notch Pearson's Minecraft and am using textures created by Jasper "JAPPA" Boerstra's Official Minecraft textures for the block and player model. The Block of Marc uses the Marccoin found in the tutorials and the Chicken emoji found on the course discord. 

### Please give instructions for how a viewer/player should interact with this scene to see all the content you have created.

Instructions can be seen when the player firsts boots up the game. The player needs to input their desired world size, the world preset, and their render distance. How the player interacts with the scene depends on what they create. They may create a race course to race around, a parkour map like the one given in the preset, create pixel art, or rebuild the marccoin. Instructions is also given at the start of this markdown file.

All features (Besides a few preset worlds and a few blocks) are demonstrated in the following demo videos:

https://www.youtube.com/watch?v=xIFnil_tP2s
https://www.youtube.com/watch?v=cTxstJ59Y9I

Side note: The program may take a long time to start if the player chooses a large world size and/or a large render distance.


Thank you for reading through this and I hope you enjoy this recreation of Minecraft (and perhaps feel a sense of nostalgia if you're familiar with Classic Minecraft)

I am wondering about one thing with the style. How does games implement HUDs like the one featured in this project? (i.e. the hand block always remaining on screen, the hotbar/tool bar staying there static etc.). I'm absolutely sure the way I've implemented a HUD in this project is not the norm. I'm assuming normal games does not use a scene graph nor actually rotate those objects around based on the players cursor just so that it's always in view of the camera. I'm assuming we need another separate shader for this?

## Section 5 (optional): Code Style Feedback

### There are no marks assigned to code style in this assignment. However, if you would like to receive some feedback on your coding style, please point out a section of your code (up to 100 lines) and your tutor will give you some written feedback on the structure and readability of that code.

Write your answer here (lines numbers or function names that you'd like reviewed)

## Section 6 (optional): Use of External Libraries

### What does the library do?

Write your answer here...

### Why did you decide to use it?

Write your answer here...

### How does this integrate with the assignment project build system? Please include any instructions and requirements if a marker needs to integrate an external library.

Write your answer here...
