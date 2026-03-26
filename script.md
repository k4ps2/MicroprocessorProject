🎤 Speaker 1 (0:00 – 3:00) — Initialization and Program Structure
Составление текстов

[Part: headers, function declarations, constants, start of main()]

Hello! I will explain the structure and foundation of this program.

At the beginning, we include libraries:
stm32f031x6.h, display.h, sound.h, musical_notes.h.
They handle the microcontroller, display, and sound.

Next, we see function declarations.
They tell the compiler about functions before they are used, such as:
initClock(), setupIO(), playBeep(), and others.

Then we define game constants:

screen size (SCREEN_WIDTH, SCREEN_HEIGHT)
dinosaur parameters (DINO_X, DINO_Y_GROUND)
physics (GRAVITY, JUMP_STRENGTH)
game states (STATE_MENU, STATE_PLAYING, STATE_GAME_OVER)

This makes the code easier to read and modify.

After that, we have global variables:

score — the current score
gameState — current game state
dinoY, dinoVelocity — movement of the dinosaur
obstacles array — stores obstacles

Now the main part — the main() function.

First comes system initialization:

initClock() — sets CPU frequency
initSysTick() — millisecond timer
setupIO() — buttons and LED setup
initSound() and initPWM() — sound system

Then messages are sent via UART for debugging.

After that, the program enters an infinite loop while(1), where:

input from UART is processed
buttons are checked
the game is updated
the screen is drawn

There is also FPS limiting (~30 frames per second) using milliseconds.

This loop is where the whole program runs.




🎤 Speaker 2 (3:00 – 6:00) — Game Logic
Составление текстов

[Part: updateGame(), resetGame(), jump logic, collisions]

Now I will explain how the game works.

Let’s start with resetGame():
it resets everything:

score = 0
speed = initial value
dinosaur returns to the ground
obstacles are reset

This is called when a new game starts.

The main logic is in updateGame().

First — gravity and jumping:
If the dinosaur is jumping (isJumping == 1):

velocity increases (dinoVelocity += GRAVITY)
position changes (dinoY += dinoVelocity)

When it reaches the ground, the jump stops.

Next — spawning obstacles:

controlled by spawnWait
when it reaches 0, a new obstacle appears
randomness comes from random16()

Then obstacle movement:

obstacles move left (x -= gameSpeed)

The most important part — collision detection:
It checks rectangle overlap:
if the dinosaur intersects with an obstacle:
→ game ends (STATE_GAME_OVER)
→ a sound is played

Next — removing obstacles:
when they leave the screen:

they are deactivated
score increases

Also, every 4 points, speed increases — making the game harder.

In main() we also handle input:

UP button → jump
other buttons → control speed

Additionally:
when jumping, the LED blinks — just a visual effect.

So overall, the game logic consists of:
movement + physics + collisions + scoring.




🎤 Speaker 3 (6:00 – 9:00) — Rendering, Sound, and Low-Level Control
Составление текстов

[Part: drawGame(), drawMenu(), drawGameOver(), sound, Serial, SysTick]

Now let’s look at rendering and low-level functionality.

The function drawGame() draws the gameplay:

clears the screen (fillRectangle)
draws the ground (drawLine)
draws the dinosaur (putImage)
draws obstacles
displays score and speed

There are also:

drawMenu() — start screen
drawGameOver() — game over screen

All of these use functions from display.h.

Now about sound:

The function playBeep():

plays a note using playNote()
waits for a duration
stops the timer

Sound is used:

when jumping
when starting the game
when losing

Next — UART (Serial communication):

serialWriteString() — sends text
serialReadChar() — reads input

This allows controlling the game from a computer.

Now an important part — SysTick timer:

initSysTick() sets it to 1 millisecond
SysTick_Handler() increments milliseconds

This is the time base for:
FPS, delays, and animations.

Finally — GPIO control:

pinMode() — sets pin modes
enablePullUp() — enables pull-up resistors
setLedState() — controls the LED

In conclusion:
this program combines:

low-level STM32 control
game logic
graphics and sound

All running inside a single infinite loop.

Thank you!
