//
//  main.cpp
//  class1
//
//  Created by Aaron Taylor on 9/11/14.
//  Copyright (c) 2014 Aaron Taylor. All rights reserved.
//

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Needed on MsWindows
#include <windows.h>
#endif // Win32 platform

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
// Download glut from:
// http://www.opengl.org/resources/libraries/glut/
#include <GLUT/glut.h>

#define ARC4RANDOM_MAX 0x100000000
// random double between 0.0 and 1.0
#define LITTLE_RAND ((double)arc4random() / ARC4RANDOM_MAX)

//////////////////////////////
// game tuning parameters
//////////////////////////////

#define NUM_BLOCKS 45
#define STEP_SIZE 0.02
#define MOUSE_WEIGHT 25.0
#define COLLISION_DIST 0.07

//////////////////////////////
// call back method definitions
//////////////////////////////

void onReshape(int winWidth0, int winHeight0);
void onKeyboard(unsigned char key, int x, int y);
void onMotion(int x, int y);
void onIdle();
void onDisplay();

//////////////////////////////
// global variables and types
//////////////////////////////

typedef enum{
    selection,
    straightGame,
    diagonalGame,
    sinGame,
    jitterGame
}mode;

mode gameMode = selection;

int score = 0;

//////////////////////////////
// game objects
//////////////////////////////

class Block {
    float baseYOffset = 0.5; // allows for centered behavior such as sine curves
    float yOffset = 0.5;
    float xOffset = -1.0;
    
    float size = 0.05;
    
    float speedMultiplier = 1.0;
    float verticalMovement = 0.0;
    
    float rColor = 0.0;
    float gColor = 0.0;
    float bColor = 0.0;
public:
    Block() {
        this->setRandValues();
        xOffset += 1.0;
    }
    void moveX(float i)
    {
        xOffset += i * speedMultiplier;
        switch (gameMode) {
            case straightGame:
                // nothing to do
                break;
            case diagonalGame:
                yOffset += verticalMovement/10.0;
                break;
            case sinGame:
                yOffset = baseYOffset + sin(xOffset*10)*verticalMovement*30.0;
                break;
            case jitterGame:
                // 1/10 chance that it will change direction
                if (rand()%15 == 0) {
                    verticalMovement = -1 * verticalMovement;
                }
                yOffset = yOffset + 0.8*verticalMovement;
                
            default:
                break;
        }
    }
    void setRandValues()
    {
        baseYOffset = LITTLE_RAND * 2.0 - 1.0;
        yOffset = baseYOffset;
        xOffset = LITTLE_RAND * 3.0 - 4.0;
        size = LITTLE_RAND * 0.07 + 0.02;
        speedMultiplier = LITTLE_RAND * 0.5 + 0.8;
        verticalMovement = LITTLE_RAND * 0.01 - 0.005;
        rColor = LITTLE_RAND;
        gColor = LITTLE_RAND;
        bColor = LITTLE_RAND;
    }
    void draw()
    {
        // blocks recycle themselves when they go off screen
        if (xOffset > 1.09) {
            setRandValues();
            // if a block is reset, it has passed the player so the score is incremented
            score += 1;
        }
        glBegin(GL_POLYGON);
        glColor3d(rColor,gColor,bColor);
        glVertex2d(xOffset, yOffset + size);
        glVertex2d(xOffset - 0.8 * size, yOffset - size);
        glVertex2d(xOffset + 0.8 * size, yOffset - size);
        glEnd();
    }
    bool hasCollided(float x, float y) {
        if (fabsf(x - xOffset) < COLLISION_DIST && fabsf(y - yOffset) < COLLISION_DIST ) {
            return true;
        }
        return false;
    }
};

class Player {
public:
    float yOffset = 0.0;
    float death = 0.0;
    
    Player() {
        yOffset = ((double)arc4random() / ARC4RANDOM_MAX) * 1.5 - 0.5;
    }
    void stepUp() {
        if (yOffset < 0.91) { yOffset += STEP_SIZE; }
    }
    void stepDown() {
        if (yOffset > -0.91) { yOffset -= STEP_SIZE; }
    }
    void draw(float pos)
    {
        yOffset = pos;
        glBegin(GL_POLYGON);
        glColor3d(1.0, 0.1, 0.1);
        // upper right edge
        glVertex2d(0.90, yOffset + 0.02);
        glVertex2d(0.95, yOffset + 0.05);
        // right inset
        glVertex2d(0.92, yOffset );
        // lower edge
        glVertex2d(0.95, yOffset - 0.05);
        glVertex2d(0.90, yOffset - 0.02);
        glVertex2d(0.85, yOffset - 0.05);
        // upper left
        glVertex2d(0.88, yOffset );
        glVertex2d(0.85, yOffset + 0.05);
        glEnd();
    }
    void drawDeath(float pos) {
        
    }
};

//////////////////////////////
// global variables for game code
//////////////////////////////

bool showPlayer = false;
bool newPlayer = false;

bool alive = false;

float weightedMousePos = 0.0;
float curMousePos = 0.0;

float gameStart = 0;

Block *blocks[NUM_BLOCKS];

Player *player;

//////////////////////////////
// game actions
//////////////////////////////

void returnToLife( )
{
    if (alive) {
        return;
    }
    
    alive = true;
    score = 0;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i]->setRandValues();
        blocks[i]->moveX(0.5);
    }
}

//////////////////////////////
// drawing code
//////////////////////////////

void drawSelection( )
{
    for (int i = 1; i <= 4; i++) {
        // draw each of the blocks
    }
    glBegin(GL_POLYGON);
    
    glColor3d(0.0f, 0.0f, 0.0f);
    glVertex2d(-0.7, 0.3);
    glVertex2d(0.7, 0.3);
    glVertex2d(0.7, -0.3);
    glVertex2d(-0.7, -0.3);
    
    glEnd();
    
    char buf[BUFSIZ];
    char name[] = "Straight:1 Diagonal:2\n Sine:3 Jitter:4";
    
    snprintf(buf, sizeof(buf), "%s", name);
    
    //printf("chars: %s\n",buf);
    //int len = (int)strlen(buf);
    glColor3d(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRasterPos2f(-0.55,0.0);
    for (char *p = buf; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    glPopMatrix();
}

void drawGround( )
{
    glBegin(GL_POLYGON);
    
    if (alive) {
        glColor3d(0.9f, 0.7f, 0.3f);
    } else {
        glColor3d(0.9f, 0.1f, 0.3f);
    }
    glVertex2d(-1.0, -1.0);
    glColor3d(0.5f, 0.7f, 0.3f);
    for (int i = -10; i < 10; i++) {
        i%2 == 0 ? glVertex2d(0.1*i, -0.8) : glVertex2d(0.1*i, -0.5);
    }
    if (alive) {
        glColor3d(0.9f, 0.7f, 0.3f);
    } else {
        glColor3d(0.9f, 0.1f, 0.3f);
    }
    glVertex2d(1.0, -1.0);
    
    glEnd();
}

void drawScore( )
{
    char buf[BUFSIZ];
    char name[] = "Score";
    
    snprintf(buf, sizeof(buf), "%s: %08d", name, score);
    
    //printf("chars: %s\n",buf);
    //int len = (int)strlen(buf);
    glPushMatrix();
    glRasterPos2f(-0.97,0.9);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (char *p = buf; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    glPopMatrix();
}

//////////////////////////////
// callback functions
//////////////////////////////

void onReshape(int winWidth0, int winHeight0)
{
    glViewport(0, 0, winWidth0, winHeight0);
}

void onKeyboard(unsigned char key, int x, int y)
{
    // if the game is happening
    switch (key) {
            // restart the game
        case 'r':
            returnToLife();
            break;
            
            // pause the game
        case 'p':
            // do something helpful
            break;
            
        case '1':
            gameMode = straightGame;
            if (!alive) {
                returnToLife();
            }
            break;
            
        case '2':
            gameMode = diagonalGame;
            if (!alive) {
                returnToLife();
            }
            break;
            
        case '3':
            gameMode = sinGame;
            if (!alive) {
                returnToLife();
            }
            break;
            
        case '4':
            gameMode = jitterGame;
            if (!alive) {
                returnToLife();
            }
            break;
            
        default:
            break;
    }
    
    glutPostRedisplay();
}

// move player up and down based on keyboard input
void onMotion(int x, int y)
{
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    curMousePos = -2.0 * (float)y / (float)h + 1.0;
    if (weightedMousePos == 0.0) {
        weightedMousePos = curMousePos;
    }
    weightedMousePos = ((weightedMousePos * (MOUSE_WEIGHT - 1)) + curMousePos) / MOUSE_WEIGHT;
    //printf("cur: %f\nscore: %i\n",curMousePos,score);
}

void onIdle()
{
    // time elapsed since program started, in seconds
    double t = glutGet(GLUT_ELAPSED_TIME) * 0.001;
    
    // variable to remember last time idle was called
    static double lastTime = 0.0;
    // time difference between calls: time step
    double dt = t - lastTime;
    // store time
    if (alive) {
        lastTime = t;
    } else {
        gameStart = t;
    }
    
    // base speed of 0.2 units/sec, increases as game progresses
    float inc = dt * 0.2 + 0.01 * (t - gameStart);
    for (int i = 0; alive && i < NUM_BLOCKS; i++)
    {
        blocks[i]->moveX(dt * inc);
    }
    
    weightedMousePos = ((weightedMousePos * (MOUSE_WEIGHT - 1)) + curMousePos) / MOUSE_WEIGHT;
    
    // show
    glutPostRedisplay();
}


void onDisplay()
{
    // draw here
    glClearColor(0.4f, 0.7f, 0.9f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawGround();
    
    if (gameMode == selection) {
        // show
        drawSelection();
        
        glutSwapBuffers();
        return;
    }

    drawScore();
    
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (blocks[i]->hasCollided(0.9, player->yOffset)) {
            alive = false;
        } else {
            blocks[i]->draw();
        }
        
    }
    player->draw(weightedMousePos);
    
    glutSwapBuffers();
}

//////////////////////////////
// main function, entry point to program
//////////////////////////////

int main(int argc, char *argv[]) {
    *blocks = new Block[NUM_BLOCKS];
    int i;
    Block * currentPointer;
    for (i = 0; i < NUM_BLOCKS; i++)
    {
        currentPointer = new Block();
        blocks[i] = currentPointer;
        //delete currentPointer;
    }
    
    player = new Player();
    
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    // rgb to specify color, double specifies two buffers, depth is 3d stuff
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutCreateWindow("Dodge");
    
    // Register event handlers here
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutPassiveMotionFunc(onMotion);
    glutIdleFunc(onIdle);
	
    glutMainLoop();
    return 0;
}
