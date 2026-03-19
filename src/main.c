#include <stm32f031x6.h>








#include "display.h"

// Function declarations
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void playBeep(uint32_t frequency, uint32_t duration);
void initPWM();
void initSerial(void);
void serialWriteChar(char c);
void serialWriteString(const char *s);
uint8_t serialAvailable(void);
char serialReadChar(void);
uint16_t random16(void);
void setLedState(uint8_t on);

void updateGame();
void drawGame();
void drawMenu();
void drawGameOver();
void resetGame();

// Game states
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

// Game constants
#define DINO_X 20
#define DINO_Y_GROUND 130
#define DINO_WIDTH 12
#define DINO_HEIGHT 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
#define OBSTACLE_WIDTH 8
#define OBSTACLE_HEIGHT 16
#define MAX_OBSTACLES 3
#define GRAVITY 2
#define JUMP_STRENGTH -30
#define SERIAL_BAUD 115200
#define LED_PIN 3

// Global variables
volatile uint32_t milliseconds;
uint8_t gameState = STATE_MENU;
uint16_t score = 0;
uint16_t gameSpeed = 5;
uint32_t frameCounter = 0;

// Dinosaur jump variables
int16_t dinoY = DINO_Y_GROUND;
int16_t dinoVelocity = 0;
uint8_t isJumping = 0;

// Obstacle structure
typedef struct {
    int16_t x;
    uint8_t active;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
uint8_t nextObstacleIndex = 0;
uint16_t spawnWait = 20;
uint32_t randomSeed = 0x12345678;

// Simple dinosaur sprite (8x16 pixels, green)
const uint16_t dinoBitmap[] = {
    0x0000, 0x0000, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000,
    0x0000, 0x2104, 0x2104, 0x0001, 0x0001, 0x2104, 0x2104, 0x0000,
    0x0000, 0x2104, 0x2104, 0x0001, 0x0001, 0x2104, 0x2104, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x2104, 0x2104, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2104, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// Obstacle sprite (red rectangle, 8x16)
const uint16_t obstacleBitmap[] = {
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,
};

int main()
{
	initClock();
	initSysTick();
	setupIO();
	initPWM();
	randomSeed = 0xABCD1234 + milliseconds;
	serialWriteString("DINO GAME START\r\n");
	serialWriteString("Press UP button or send 's' via UART to start.\r\n");
	
	// Initialize obstacles
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		obstacles[i].active = 0;
		obstacles[i].x = SCREEN_WIDTH + i * 40;
	}
	
	uint32_t lastFrameTime = 0;
	
	while(1)
	{
		if (serialAvailable()) {
			char c = serialReadChar();
			if (c == 's' || c == 'S') {
				if (gameState == STATE_MENU) {
					resetGame();
					gameState = STATE_PLAYING;
					serialWriteString("Game started from UART!\r\n");
				}
			} else if (c == 'r' || c == 'R') {
				if (gameState == STATE_GAME_OVER) {
					gameState = STATE_MENU;
					serialWriteString("Restarting to menu from UART...\r\n");
				}
			}
		}
	
		// Frame rate limiting (update at ~30Hz)
		if (milliseconds - lastFrameTime >= 30) {
			lastFrameTime = milliseconds;
			frameCounter++;
			
			if (gameState == STATE_MENU) {
				// Check for start button (UP button)
				if ((GPIOA->IDR & (1 << 8)) == 0) {
					delay(20); // debounce
					if ((GPIOA->IDR & (1 << 8)) == 0) {
						resetGame();
						gameState = STATE_PLAYING;
						playBeep(800, 100);
						serialWriteString("Game started from button\r\n");
					}
					while((GPIOA->IDR & (1 << 8)) == 0);
					delay(20);
				}
				drawMenu();
			}
			else if (gameState == STATE_PLAYING) {
				// Jump input (UP button)
				if ((GPIOA->IDR & (1 << 8)) == 0 && !isJumping) {
					isJumping = 1;
					dinoVelocity = JUMP_STRENGTH;
					playBeep(600, 80);
				}
				// Speed control via buttons
				if ((GPIOB->IDR & (1 << 4)) == 0) {
					if (gameSpeed < 20) gameSpeed++;
					delay(10);
				}
				if ((GPIOB->IDR & (1 << 5)) == 0) {
					if (gameSpeed > 2) gameSpeed--;
					delay(10);
				}
				updateGame();
				drawGame();
				setLedState(0);
			}
			else if (gameState == STATE_GAME_OVER) {
				setLedState(1);
				// Check for restart (UP button)
				if ((GPIOA->IDR & (1 << 8)) == 0) {
					delay(20);
					if ((GPIOA->IDR & (1 << 8)) == 0) {
						gameState = STATE_MENU;
						playBeep(800, 100);
					}
					while((GPIOA->IDR & (1 << 8)) == 0);
					delay(20);
				}
				drawGameOver();
			}
		}
	}
	return 0;
}
void initPWM()
{
    // PB1 будет использоваться для звука (PWM на TIM2_CH4)
    RCC->APB1ENR |= (1 << 0);  // Включаем TIM2
    RCC->AHBENR |= (1 << 18);  // Включаем порт B
    
    // Настраиваем PB1 как альтернативную функцию (AF5) для TIM2_CH4
    GPIOB->MODER &= ~(3 << (1*2)); // Очистить режим
    GPIOB->MODER |= (2 << (1*2));  // AF mode
    GPIOB->AFR[0] &= ~(0xF << (1*4));
    GPIOB->AFR[0] |= (5 << (1*4)); // AF5 для TIM2_CH4

    // Настройка TIM2
    TIM2->PSC = 3;            // Прескейлер (делает таймер медленнее)
    TIM2->ARR = 1000;         // Частота PWM (будет меняться в playBeep)
    TIM2->CCMR2 |= (6 << 12); // PWM mode 1 для CH4
    TIM2->CCMR2 |= (1 << 11); // Preload enable
    TIM2->CCER |= (1 << 12);  // Включаем вывод CH4
    TIM2->CR1 |= (1 << 0);    // Старт таймера
}

void playBeep(uint32_t frequency, uint32_t duration)
{
    if(frequency == 0) {
        TIM2->CCR4 = 0; // выключаем звук
        return;
    }

    uint32_t arr = 4000000 / frequency; // если таймер работает от 4MHz
    TIM2->ARR = arr;
    TIM2->CCR4 = arr / 2;  // 50% duty cycle

    // Ждём duration миллисекунд
    uint32_t endTime = milliseconds + duration;
    while(milliseconds < endTime) {
        __asm(" wfi "); // спим до следующего прерывания
    }

    TIM2->CCR4 = 0; // выключаем звук
}

void resetGame()
{
	score = 0;
	gameSpeed = 5;
	dinoY = DINO_Y_GROUND;
	dinoVelocity = 0;
	isJumping = 0;
	frameCounter = 0;
	nextObstacleIndex = 0;
	
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		obstacles[i].active = 0;
		obstacles[i].x = SCREEN_WIDTH + i * 50;
	}
}

void updateGame()
{
	// Apply gravity
	if (isJumping) {
		dinoVelocity += GRAVITY;
		dinoY += dinoVelocity;
		
		// Check if landed
		if (dinoY >= DINO_Y_GROUND) {
			dinoY = DINO_Y_GROUND;
			dinoVelocity = 0;
			isJumping = 0;
		}
	}
	
// Spawn obstacles with random interval
	if (spawnWait == 0) {
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (!obstacles[i].active) {
				obstacles[i].active = 1;
				obstacles[i].x = SCREEN_WIDTH;
				break;
			}
		}
		spawnWait = 18 + (random16() % 18);
	} else {
		spawnWait--;
	}

	// Update obstacles
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (obstacles[i].active) {
			obstacles[i].x -= gameSpeed;

			// Check collision with dinosaur bounding box
			int obstacleY = DINO_Y_GROUND;
			if (!(obstacles[i].x + OBSTACLE_WIDTH < DINO_X ||
				obstacles[i].x > DINO_X + DINO_WIDTH ||
				dinoY + DINO_HEIGHT < obstacleY ||
				dinoY > obstacleY + OBSTACLE_HEIGHT)) {
				gameState = STATE_GAME_OVER;
				playBeep(200, 200);
				serialWriteString("Collision! Game over\r\n");
				return;
			}

			// Remove off-screen obstacles
			if (obstacles[i].x < -OBSTACLE_WIDTH) {
				obstacles[i].active = 0;
				score++;
				serialWriteString("Score: ");
				char buff[6];
				buff[0] = (score / 10000) % 10 + '0';
				buff[1] = (score / 1000) % 10 + '0';
				buff[2] = (score / 100) % 10 + '0';
				buff[3] = (score / 10) % 10 + '0';
				buff[4] = score % 10 + '0';
				buff[5] = '\0';
				serialWriteString(buff);
				serialWriteString("\r\n");

				// Increase difficulty gradually
				if (score % 5 == 0 && gameSpeed < 20) {
					gameSpeed++;
				}
			}
		}
	}
}

void drawGame()
{
	// Clear screen
	fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);
	
	// Draw ground line
	drawLine(0, DINO_Y_GROUND + DINO_HEIGHT, SCREEN_WIDTH, DINO_Y_GROUND + DINO_HEIGHT, RGBToWord(0xFF, 0xFF, 0xFF));
	
	// Draw dinosaur
	putImage(DINO_X, dinoY, 8, 16, dinoBitmap, 0, 0);
	
	// Draw obstacles
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (obstacles[i].active) {
			putImage(obstacles[i].x, DINO_Y_GROUND, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, obstacleBitmap, 0, 0);
		}
	}
	
	// Draw score
	printNumber(score, 5, 10, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
	
	// Draw speed indicator
	printText("SPD:", 80, 10, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
	printNumber(gameSpeed, 110, 10, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
}

void drawMenu()
{
	// Clear screen
	fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);
	
	// Draw title
	printTextX2("DINO", 35, 20, RGBToWord(0x00, 0xFF, 0x00), 0x0000);
	
	// Draw dinosaur
	putImage(50, 60, 8, 16, dinoBitmap, 0, 0);
	
	// Draw instruction
	printText("PRESS UP TO START", 10, 100, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
	printText("JUMP TO AVOID", 10, 120, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
}

void drawGameOver()
{
	// Clear screen
	fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);
	
	// Draw game over text
	printTextX2("GAME OVER", 10, 30, RGBToWord(0xFF, 0x00, 0x00), 0x0000);
	
	// Draw score
	printText("SCORE:", 30, 70, RGBToWord(0xFF, 0xFF, 0xFF), 0x0000);
	printNumberX2(score, 30, 85, RGBToWord(0x00, 0xFF, 0x00), 0x0000);
	
	// Draw restart instruction
	printText("PRESS UP TO", 20, 130, RGBToWord(0xFF, 0xFF, 0x00), 0x0000);
	printText("RETURN TO MENU", 10, 145, RGBToWord(0xFF, 0xFF, 0x00), 0x0000);
}
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}

void SysTick_Handler(void)
{
	milliseconds++;
}

void initClock(void)
{
	// Set the PLL up
	// First ensure PLL is disabled
	RCC->CR &= ~(1u<<24);
	while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
	
	// Set flash wait states
	FLASH->ACR |= (1 << 0);
	FLASH->ACR &=~((1u << 2) | (1u<<1));
	// Turn on FLASH prefetch buffer
	FLASH->ACR |= (1 << 4);
	// set PLL multiplier to 12 (yielding 48MHz)
	RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
	RCC->CFGR |= ((1<<21) | (1<<19) ); 

	// Need to limit ADC clock to below 14MHz
	RCC->CFGR |= (1<<14);

	// Turn PLL back on
	RCC->CR |= (1<<24);        
	// Set PLL as system clock source 
	RCC->CFGR |= (1<<1);
}

void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

void initSerial(void)
{
	RCC->AHBENR |= (1 << 17); // enable GPIOA
	RCC->APB2ENR |= (1 << 14); // enable USART1

	// PA9: TX, PA10: RX in AF1
	GPIOA->AFR[1] &= ~((0xF << ((9-8)*4)) | (0xF << ((10-8)*4)));
	GPIOA->AFR[1] |= (1 << ((9-8)*4)) | (1 << ((10-8)*4));
	GPIOA->MODER &= ~((3 << (9*2)) | (3 << (10*2)));
	GPIOA->MODER |= (2 << (9*2)) | (2 << (10*2));

	USART1->BRR = 48000000 / SERIAL_BAUD;
	USART1->CR1 = (1 << 2) | (1 << 3) | (1 << 13); // RE, TE, UE
}

void serialWriteChar(char c)
{
	while (!(USART1->ISR & (1 << 7)));
	USART1->TDR = (uint8_t)c;
}

void serialWriteString(const char *s)
{
	while (*s) {
		serialWriteChar(*s++);
	}
}

uint8_t serialAvailable(void)
{
	return (USART1->ISR & (1 << 5)) ? 1 : 0;
}

char serialReadChar(void)
{
	return (char)(USART1->RDR & 0xFF);
}

uint16_t random16(void)
{
	randomSeed ^= (randomSeed << 13);
	randomSeed ^= (randomSeed >> 9);
	randomSeed ^= (randomSeed << 7);
	return (uint16_t)(randomSeed & 0xFFFF);
}

void setLedState(uint8_t on)
{
	if (on)
		GPIOB->ODR |= (1 << LED_PIN);
	else
		GPIOB->ODR &= ~(1 << LED_PIN);
}

int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks if point px,py is within the rectangle defined by x1,y1,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	initSerial();
	setLedState(0);

	// Configure input buttons
	pinMode(GPIOB, 4, 0);  // Right
	pinMode(GPIOB, 5, 0);  // Left
	pinMode(GPIOA, 8, 0);  // Up (Jump)
	pinMode(GPIOA, 11, 0); // Down

	// Configure simple status LED output on PB3
	pinMode(GPIOB, LED_PIN, 1); // output
	GPIOB->ODR &= ~(1 << LED_PIN);

	// Enable pull-ups
	enablePullUp(GPIOB, 4);
	enablePullUp(GPIOB, 5);
	enablePullUp(GPIOA, 11);
	enablePullUp(GPIOA, 8);
}