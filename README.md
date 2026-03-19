# Dino Game (STM32F031) - University Project

## 1. What this project is

This is a simple Dino Runner arcade game running on the STM32F031 microcontroller (Nucleo F031K6). The player presses the UP button to make the dinosaur jump and must avoid obstacles. The game includes a menu, score tracking, sound, UART output, and an LED indicator.

---

## 2. How to build and run

1. Open the project in PlatformIO (VS Code).
2. Make sure the Nucleo F031K6 board is powered and connected via USB.
3. Click Build (or run `platformio run`).
4. Click Upload (or run `platformio run --target upload`).
5. Open the Serial Monitor (`platformio device monitor`) to view text output and send commands.

> **Important**: When the board is running, the menu is displayed. Press the `UP` button or send `s` via UART to start.

---

## 3. File structure and responsibilities

* `src/main.c` — main game logic:

  * Initialization of clock, interrupts, GPIO, and UART.
  * Main loop, frame updates, game state logic.
  * Handling button input and simple UART commands.
  * Rendering (via display library functions).
* `src/display.c` — TFT display driver:

  * SPI and display initialization.
  * Drawing pixel, line, rectangle, circle.
  * Functions: `printText`, `putImage`, `fillRectangle`.
* `src/display.h` — display driver header.
* `src/font5x7.h` — 5x7 font for text.

---

## 4. How the code works (detailed explanation)

### 4.1. Initialization

* `main()` calls:

  * `initClock()` (PLL at 48 MHz),
  * `initSysTick()`,
  * `setupIO()`,
  * `initPWM()`.

* Inside `setupIO()`:

  * Enable GPIO ports A and B.
  * Initialize display (`display_begin()`).
  * Initialize UART: PA9 (TX), PA10 (RX).
  * Configure buttons as input with pull-up.
  * Configure LED on PB3 as output.

---

### 4.2. Serial communication (UART)

* `initSerial()` initializes USART1.
* `serialWriteChar()` and `serialWriteString()` send data.
* In the `main()` loop:

  * Read UART input using `serialAvailable()` and `serialReadChar()`.
  * Commands:

    * `s` / `S` — start game,
    * `r` / `R` — restart after Game Over.

---

### 4.3. Variables and states

* `gameState` values:

  * `STATE_MENU` — menu,
  * `STATE_PLAYING` — gameplay,
  * `STATE_GAME_OVER` — game over.

* `frameCounter` increments every 30 ms.

* `spawnWait` controls random spawn intervals.

* `random16()` generates pseudo-random values using bit shifts.

---

### 4.4. Game loop

Inside `while(1)`:

* Read UART commands.
* If more than 30 ms passed → update frame.
* In menu state → call `drawMenu()`.
* In playing state:

  * UP → jump.
  * RIGHT/LEFT → adjust speed.
  * Call `updateGame()` and `drawGame()`.
* In Game Over:

  * Draw end screen.
  * Turn on LED.

---

### 4.5. `updateGame()` function

Core game mechanics:

* Apply gravity to `dinoVelocity`.
* Spawn obstacles using random `spawnWait`.
* Move obstacles left with `gameSpeed`.
* Check collision using rectangle intersection.
* If obstacle exits screen:

  * Increase `score`,
  * Print to UART,
  * Increase game speed.

---

### 4.6. Graphics and HUD

`drawGame()`:

* Clears screen.
* Draws:

  * Ground,
  * Dinosaur (`putImage()`),
  * Obstacles,
  * HUD (score and speed).

---

### 4.7. Controls

Buttons:

* UP (PA8) — jump / start / restart.
* RIGHT (PB4) — increase speed (during gameplay).
* LEFT (PB5) — decrease speed.

UART:

* `s` — start,
* `r` — restart,
* Outputs statistics (score, events).

---

### 4.8. Sound

* Uses TIM2 CH4 / PB1 (`initPWM()`).
* `playBeep(frequency, duration)` sets `ARR` and `CCR4`.
* Sound plays on jump, start, and Game Over.

---

## 5. Evaluation criteria (what is already implemented)

| Criterion             | Implemented                   | Where                                   |
| --------------------- | ----------------------------- | --------------------------------------- |
| Code quality/comments | Yes, structured and commented | `src/main.c`                            |
| Gameplay              | Yes, menu/game/score          | `main()` + `updateGame()`               |
| Additional input      | Yes, buttons + UART           | `setupIO()` + `main()`                  |
| Additional output     | Yes, LED + UART               | `setLedState()` + `serialWriteString()` |
| Sprite/animation      | Yes                           | `drawGame()`                            |
| HUD / Score           | Yes                           | `drawGame()`                            |
| Main menu             | Yes                           | `drawMenu()`                            |
| Restart/replay        | Yes                           | `main()`                                |
| Random numbers        | Yes                           | `random16()`                            |
| Serial I/O            | Yes                           | `initSerial()`                          |
| Sound                 | Yes                           | `playBeep()`                            |
| Demo                  | Ready                         | display + UART                          |
| Code explanation      | Yes                           | this README                             |

---

## 6. Possible improvements before defense

1. Add screen fade transition on Game Over.
2. Add a high-score table (EEPROM or RAM).
3. Add multiple obstacle types and/or better dinosaur sprite.
4. Increase difficulty progressively (speed scaling).

---

## 7. Quick start guide

* `UP` — start / jump.
* `RIGHT` / `LEFT` — adjust speed.
* `r` via UART — return to menu.
* Screen displays score and current speed.

Good luck with your defense! If you want, I can also prepare a short presentation script (what to show and what to say).
