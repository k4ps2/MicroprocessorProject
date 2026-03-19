Intro (30 сек, любой)
“Hi, we are [A, B, C], and this is our Dino Runner project on STM32.”
“We have menu, gameplay, score, restart, sound, LED, and serial output.”
“Now each of us explains our code part and then we show quick demo.”

Person 1 (1:15)
“I’m [Name1]. I prepared main control flow in main.c.”
“We use initClock(), initSysTick(), setupIO(), display start and UART init.”
“Game states are STATE_MENU, STATE_PLAYING, STATE_GAME_OVER.”
“Menu starts by UP or UART s; game loop updates every 30ms; Game Over can restart with UP or UART r.”
“This part keeps code simple and easy to follow.”

Person 2 (1:15)
“I’m [Name2]. I implemented gameplay physics and obstacles.”
“Dino jumps with JUMP_STRENGTH, gravity updates velocity, land at ground.”
“Obstacles spawn with random interval (spawnWait + random16()), move left by gameSpeed.”
“Collision uses rectangle overlap; on collision we go to game-over and beep.”
“Score increases when obstacle leaves screen, and difficulty grows gradually.”

Person 3 (1:15)
“I’m [Name3]. I did display, sound, and UART messaging.”
“drawGame() draws dino, obstacles, ground, HUD score and speed.”
“drawMenu() and drawGameOver() show instructions and final score.”
“Sound uses PF0 buzzer — playBeep() toggles PF0 to generate tone.”
“Serial prints start, score, collision, restart, so we can debug in monitor.”
“Also LED on PB3 shows game-over state.”

Outro/demo (45 сек)
“Now quick demo: upload, press UP to start, jump, avoid obstacles.”
“Right/Left adjust speed; in game over press UP to go back to menu.”
“Wiring: buzzer + to PF0 (J53), - to GND.”
“That’s our complete project in 5 minutes. Thanks!”
