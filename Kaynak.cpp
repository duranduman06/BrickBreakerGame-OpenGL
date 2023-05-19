#include <GL/freeglut_std.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_W 800
#define WINDOW_H 500
#define BRICK_ROWS 6
#define BRICK_COLS 14
#define BRICK_W 5
#define BRICK_H 2
#define BRICK_GAP 2

static GLfloat screenLeft = -49.5;
static const int FPS = 60;
static GLfloat stick1Position = 0.0;
static GLfloat stickThick = 1.0;
static GLfloat stickSize = 15;
static GLfloat moveXFactor = 0.3;
static GLfloat moveYFactor = 0.2;
int lives = 3;
int score = 0;
bool isGameOver = false;
bool gameStarted = false;
int visibleBricks = 0;
bool gameRunning = true;
bool isWin = false;


struct _ball
{
	GLfloat radius = 1;
	GLfloat X = 0.0;
	GLfloat Y = 0.0;
	int directionX = -1;
	int directionY = 1;
}ball;


struct _brick
{
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	bool visible = true;
} bricks[BRICK_ROWS][BRICK_COLS];




void init_bricks()
{
	for (int i = 0; i < BRICK_ROWS; i++)
	{
		for (int j = 0; j < BRICK_COLS; j++)
		{
			
				bricks[i][j].x = screenLeft + 2 + j * (BRICK_W + BRICK_GAP);
				bricks[i][j].y = 27 - i * (BRICK_H + BRICK_GAP);
				visibleBricks++;
			
		}
	}
}





void draw_brick(GLfloat x, GLfloat y)
{
	if (!bricks[(int)((27 - y) / (BRICK_H + BRICK_GAP))][(int)((x - screenLeft) / (BRICK_W + BRICK_GAP))].visible) return;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glColor3f(0.0f, 0.5f, 1.0f);//baby Blue
	glBegin(GL_QUADS);
	glVertex2f(0.0, 0.0);
	glVertex2f(BRICK_W, 0.0);
	glVertex2f(BRICK_W, BRICK_H);
	glVertex2f(0.0, BRICK_H);
	glEnd();
	glPopMatrix();
}





void draw_bricks()
{
	for (int i = 0; i < BRICK_ROWS; i++)
	{
		for (int j = 0; j < BRICK_COLS; j++)
		{
			if (bricks[i][j].visible)
			{
				draw_brick(bricks[i][j].x, bricks[i][j].y);
				
			}
		}
	}
}





void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0 / aspect, 50.0 / aspect, -1.0, 1.0);
}





void draw_circle(float x, float y, float radius) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	static const int circle_points = 100;
	static const float angle = 2.0f * 3.1416f / circle_points;

	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	double angle1 = 0.0;
	glVertex2d(radius * cos(0.0), radius * sin(0.0));
	int i;
	for (i = 0; i < circle_points; i++)
	{
		glVertex2d(radius * cos(angle1), radius * sin(angle1));
		angle1 += angle;
	}
	glEnd();
	glPopMatrix();
}


void printGameOverTexts() {
	const char* gameOverText = "Game Over";
	unsigned char* gameOverTextPtr = (unsigned char*)gameOverText;

	const char* restartText = "Press Enter to Restart the Game";
	unsigned char* restartTextPtr = (unsigned char*)restartText;

	const char* exitText = "Press ESC to Exit Game";
	unsigned char* exitTextPtr = (unsigned char*)exitText;


	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-8, 10); //stringin konumunu ayarlar.
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f);//stringin rengini ayarlar.
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, gameOverTextPtr);

	glRasterPos2f(-18, 0); //stringin konumunu ayarlar.
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f); //stringin rengini ayarlar.
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, restartTextPtr);

	glRasterPos2f(-14, -10); //stringin konumunu ayarlar.
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f); //stringin rengini ayarlar.
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, exitTextPtr);
	// LIVES'I ÇÝZER
	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	char livesStr[16];
	sprintf_s(livesStr, "Lives: %d", lives);
	glRasterPos2f(35, -25);
	for (int i = 0; i < strlen(livesStr); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, livesStr[i]);
	}

	// SCORE'U ÇÝZER
	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	char scoreStr[16];
	sprintf_s(scoreStr, "Score: %d", score);
	glRasterPos2f(-45, -25);
	for (int i = 0; i < strlen(scoreStr); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
	}
}


void drawPaddle() {
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	glRectf(stick1Position, -30, stick1Position + stickSize, -30 + stickThick);
}


void printWinTexts(){

	const char* gameWinText = "You Win!";
	unsigned char* gameWinTextPtr = (unsigned char*)gameWinText;

	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-8, 10); //stringin konumunu ayarlar.
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f);//stringin rengini ayarlar.
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, gameWinTextPtr);

	const char* restartText = "Press Enter to Restart the Game";
	unsigned char* restartTextPtr = (unsigned char*)restartText;

	glRasterPos2f(-18, 0); //stringin konumunu ayarlar.
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f); //stringin rengini ayarlar.
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, restartTextPtr);

}


void drawStartingScreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	glRasterPos2f(-15, 10);
	char game[32] = "BRICK BREAKER GAME";
	for (int i = 0; i < strlen(game); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game[i]);
	}

	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	glRasterPos2f(-10, 0);
	char startGame[32] = "Press 'R' to Start.";
	for (int i = 0; i < strlen(startGame); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, startGame[i]);
	}

	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	glRasterPos2f(-10, -10);
	char exitGame[32] = "Press ESC to quit.";
	for (int i = 0; i < strlen(exitGame); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, exitGame[i]);
	}
	glutSwapBuffers();
}


void display()
{
		if (!gameStarted && !isGameOver && !isWin) {
			glClear(GL_COLOR_BUFFER_BIT);
			drawStartingScreen();
			glutSwapBuffers();
		}

		if (gameStarted) {
			glClear(GL_COLOR_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			drawPaddle();
			draw_circle(ball.X, ball.Y, ball.radius);
			draw_bricks();
			glFlush();

			// LIVES'I ÇÝZER
			glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
			char livesStr[16];
			sprintf_s(livesStr, "Lives: %d", lives);
			glRasterPos2f(35, -25);
			for (int i = 0; i < strlen(livesStr); i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, livesStr[i]);
			}

			// SCORE'U ÇÝZER
			glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
			char scoreStr[16];
			sprintf_s(scoreStr, "Score: %d", score);
			glRasterPos2f(-45, -25);
			for (int i = 0; i < strlen(scoreStr); i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
			}
		
		}
		//OYUN BITERSE
		if (isGameOver) {
			printGameOverTexts();
			gameStarted = false;
		}
		if (visibleBricks == 0) {
			printWinTexts();
			isWin = true;
			isGameOver = true;
			gameStarted = false;
			
		}
		glutSwapBuffers();
	
}




void reset_game() {
	// oyunu yeniden baþlat.
	screenLeft = -49.5;
	stick1Position = 0.0;
	stickThick = 1.0;
	stickSize = 15;
	moveXFactor = 0.3;
	moveYFactor = 0.2;
	lives = 3;
	score = 0;
	isGameOver = false;
	isWin = false;
	ball.radius = 1;
	ball.X = 0.0;
	ball.Y = 0.0;
	ball.directionX = -1;
	ball.directionY = 1;
	visibleBricks = 0;
	for (int i = 0; i < BRICK_ROWS; i++)
	{
		for (int j = 0; j < BRICK_COLS; j++)
		{
			bricks[i][j].visible = true;
			visibleBricks++;
		}
	}
}






void timer(int v)
{
	glutPostRedisplay();
	if (gameStarted) {
		if (ball.X + ball.radius > 50 || ball.X - ball.radius < -50) { // sað/sol duvarlarla çarpýþmayý kontrol eder.
			ball.directionX *= -1; // topun yönünü deðiþtir.
		}
		if (ball.Y + ball.radius > 30) { // üst duvarla çarpýþmayý kontrol eder
			ball.directionY *= -1; // topun yönünü deðiþtir.
		}

		if (ball.Y - ball.radius < -30) // GAME OVER
		{
			if (lives != 1) {
				printf("Game Over\n");
				while (1) {
					for (int i = 0;i < 500000000;i++) {
					}
					break;
				}
				ball.X = 0;
				ball.Y = -24;
				moveXFactor = 0.3;
				moveYFactor = 0.2;
				ball.directionX = -1;
				ball.directionY = 1;
				lives = lives - 1;
			}
			else {
				lives--;
				isGameOver = 1;
			}
		}

		// top ve raketin etkileþimini kontrol eder.
		if (ball.Y - ball.radius < -29) { // top rakete vurursa
			if (ball.X > stick1Position && ball.X < stick1Position + stickSize) {
				ball.directionY = 1; // topun yönünü deðiþtir.
				moveXFactor *= 1.1; // topun hýzýný artýr.
				moveYFactor *= 1.1; // topun hýzýný artýr.
			}
		}
	}
	if (!isGameOver) {
		for (int i = 0; i < BRICK_ROWS; i++)
		{
			for (int j = 0; j < BRICK_COLS; j++)
			{
				if (bricks[i][j].visible) // sadece visible tuðlalar için.
				{
					GLfloat brickLeft = bricks[i][j].x;
					GLfloat brickRight = bricks[i][j].x + BRICK_W;
					GLfloat brickTop = bricks[i][j].y + BRICK_H;
					GLfloat brickBottom = bricks[i][j].y;

					// topun tuðla sýnýrlarý içinde olup olmadýðýný kontrol eder.
					if (ball.X + ball.radius > brickLeft && ball.X - ball.radius < brickRight &&
						ball.Y + ball.radius > brickBottom && ball.Y - ball.radius < brickTop)
					{
						//top tuðla ile çarpýþtýysa
						GLfloat distToLeft = ball.X - brickLeft;
						GLfloat distToRight = brickRight - ball.X;
						GLfloat distToTop = brickTop - ball.Y;
						GLfloat distToBottom = ball.Y - brickBottom;

						if (distToLeft <= ball.radius || distToRight <= ball.radius) {
							// top yandan tuðla ile çarpýþtý
							ball.directionX *= -1; // topun X yönünü deðiþtir.
						}
						else if (distToTop <= ball.radius) {
							// // top üstten tuðla ile çarpýþtý
							ball.directionY *= -1; // topun Y yönünü deðiþtir.
						}
						else if (distToBottom <= ball.radius) {
							// // top alttan tuðla ile çarpýþtý
							ball.directionY *= -1; // topun Y yönünü deðiþtir.
						}
						bricks[i][j].visible = false; // brick'i unvisible olarak ayarla.
						score += 10;
						visibleBricks--;
					}
				}
			}
		}
	}

	if (gameStarted) {
		ball.X = ball.X + (moveXFactor * ball.directionX);
		ball.Y = ball.Y + (moveYFactor * ball.directionY);
		//printf("%f\n", ball.Y);
	}
	glutTimerFunc(1000 / FPS, timer, v);
}




void kbSpecial(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if(stick1Position + stickSize < 31)
		{
			stick1Position = stick1Position + 0.5;
			printf("%f\n", stick1Position);
		}
		break;
	case GLUT_KEY_DOWN:
		if (stick1Position > -31)
		{
			stick1Position = stick1Position - 0.5;
			printf("%f\n", stick1Position);
		}
		break;
	case GLUT_KEY_LEFT:
		if (stick1Position - moveXFactor >= screenLeft)
		{
			stick1Position -= moveXFactor * 10;
		}
		else
		{
			stick1Position = screenLeft;
		}
		break;
		
	case GLUT_KEY_RIGHT:
		if (stick1Position + moveXFactor + stickSize <= 50)
		{
			stick1Position += moveXFactor * 10;
		}
		else
		{
			stick1Position = 50 - stickSize;
		}
		break;
		
	}
}



void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{

	case 'A':
	case 'a':
		// raket saða doðru hareket eder.
		if (stick1Position - moveXFactor >= screenLeft)
		{
			stick1Position -= moveXFactor * 10;
		}
		else
		{
			stick1Position = screenLeft;
		}
		break;
	case 'D':
	case 'd':
		// raket sola doðru hareket eder.
		if (stick1Position + moveXFactor + stickSize <= 50)
		{
			stick1Position += moveXFactor * 10;
		}
		else
		{
			stick1Position = 50 - stickSize;
		}
		break;
	case 13:
		if (isGameOver) {
		reset_game();
		} break;
	case 27: exit(0); break;
	case 'r':
	case 'R':
		gameStarted = true;
		isWin = false;
	}
}




int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(260, 140);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("Brick Breaker Game");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0); 
	glutSpecialFunc(kbSpecial);
	glutKeyboardFunc(keyboardFunc);
	init_bricks();
	glutMainLoop();
}
