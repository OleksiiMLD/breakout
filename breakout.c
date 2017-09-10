//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GLabel initLivesboard(GWindow window);
void updateLivesboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);


char * colors[]  = { "RED", "BLUE","ORANGE", "GREEN", "YELLOW"};
GRect bricks[ROWS][COLS];
const int brickH = 10;
const int brickW = 30;
const int gapH = 9;
const int gapV = 5;


int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // instantiate livesboard, centered in middle of window, just above ball
    GLabel labelLive = initLivesboard(window);
    
    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;
    
    updateLivesboard(window, labelLive, lives);

    // number of points initially
    int points = 0;

    int linger = 3;

    double velocityXS = 0.05 + drand48();
    if (velocityXS > 0.6) velocityXS = 0.6;
    double velocityYS = 1.1 - velocityXS;

	// velocity for X and Y axes
    double velocityX = velocityXS;
    double velocityY = velocityYS;

	// velocity increase coefficient
    double veloCoef = 0.05;

	// directions for X and Y axes
    int dirX = 1;
    int dirY = 1;

    waitForClick();

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        GEvent event = getNextEvent(MOUSE_EVENT);
        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure circle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
                if (x < 0) x = 0;
                if (x > getWidth(window) - getWidth(paddle)) x = getWidth(window) - getWidth(paddle);
                double y = getY(paddle);
                setLocation(paddle, x, y);
            }
        }

        // move circle along both axis
        move(ball, velocityX, velocityY);
        // bounce off right and left edge of window
        if ( ( getX(ball) + getWidth(ball) >= getWidth(window) ) || ( getX(ball) <= 0 ) )
        {
            velocityX = -velocityX;
        }
        // bounce off top edge of window
        if ( getY(ball) <= 0 )
        {
            velocityY = -velocityY;
        }
        // touch bottom edge of window
        else if ( getY(ball) + getHeight(ball) >= getHeight(window) )
        {
            --lives;
            updateLivesboard(window, labelLive, lives);
            removeGWindow(window, ball);
            ball = initBall(window);
            waitForClick();
        }
        
        GObject object = detectCollision(window, ball);
        // if collision
        if (object)
        {
               
            double x_l = getX(ball);
            double x_r = x_l + 2 * RADIUS;
            double y_t = getY(ball);
            double y_b = y_t + 2 * RADIUS;
            
            // object is paddle
            if (object == paddle)
            {
                // fall            
                if ( ( ( ( y_t + y_b ) / 2 ) > getY(paddle) ) || ( velocityY < 0 ) )
                {
                    ;
                }
                // move to right
                else if ( ( velocityX > 0 ) )
                {
                    // bounce up
                    if ( ( ( y_b - velocityY ) < getY(paddle) ) && ( x_r > getX(paddle) ) )
                    {
                        velocityY = -velocityY;
                    }
                    // bounce up and left
                    else if ( ( ( y_b - velocityY ) < getY(paddle) ) && ( ( x_r == getX(paddle) ) ) )
                    {
                        velocityX = -velocityX;
                        velocityY = -velocityY;
                    }
                    // bounce left
                    else if ( ( ( y_b ) > getY(paddle) ) && ( x_r >= getX(paddle) ) )
                    {
                        velocityX = -velocityX;
                    }
                }
                // move to left
                else if ( velocityX < 0 )
                {
                    // bounce up
                    if ( ( ( y_b - velocityY ) < getY(paddle) ) && ( x_l < ( getX(paddle) + getWidth(paddle) ) ) )
                    {
                        velocityY = -velocityY;
                    }
                    // bounce up and right
                    else if ( ( ( y_b - velocityY) < getY(paddle) ) && ( x_l == ( getX(paddle) + getWidth(paddle) ) ) )
                    { 
                        velocityX = -velocityX;
                        velocityY = -velocityY;
                    }
                    // bounce right
                    else if ( ( ( y_b ) > getY(paddle) ) && ( x_l <= ( getX(paddle) + getWidth(paddle) ) ) )
                    {
                        velocityX = -velocityX;
                    }
                }
            }
            // object is brick
            else if (strcmp(getType(object), "GRect") == 0)
            {
                // move down
                if ( velocityY > 0 )
                {
                    dirY = 1;
                    // move to right
                    if ( velocityX > 0 )
                    {
                        dirX = 1;
                        // bounce up
                        if ( ( ( ( y_b ) >= getY(object) ) && ( x_r > getX(object) ) ) )
                        {
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce up and left
                        else if ( ( ( y_b ) == getY(object) ) && ( x_r == getX(object) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce left
                        else if ( ( ( y_b ) > getY(object) ) && ( x_r >= getX(object) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                        }
                    }
                    // move to left
                    else if ( velocityX < 0 )
                    {
                        dirX = -1;
                        // bounce up
                        if ( ( ( y_b ) >= getY(object) ) && ( x_l < ( getX(object) + getWidth(object) ) ) )
                        {
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce up and right
                        else if ( ( ( y_b ) == getY(object) ) && ( x_l == ( getX(object) + getWidth(object) ) ) )
                        { 
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce right
                        else if ( ( ( y_b ) > getY(object) ) && ( x_l <= ( getX(object) + getWidth(object) ) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                        }
                    }
                }
                // move up
                else if ( velocityY < 0 )
                {
                    dirY = -1;
                    // move to right
                    if ( velocityX > 0 )
                    {
                        dirX = 1;
                        // bounce down
                        if ( ( ( y_t ) <= ( getY(object) + getHeight(object) ) ) && ( x_r > getX(object) ) )
                        {
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce down and left
                        else if ( ( ( y_t ) == ( getY(object) + getHeight(object) ) ) && ( x_r == getX(object) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce left
                        else if ( ( ( y_t ) < ( getY(object) + getHeight(object) ) ) && ( x_r >= getX(object) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                        }
                    }
                    // move to left
                    else if ( velocityX < 0 )
                    {
                        dirX = -1;
                        // bounce down
                        if ( ( ( y_t ) <= ( getY(object) + getHeight(object) ) ) && ( x_l < ( getX(object) + getWidth(object) ) ) )
                        {
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce down and right
                        else if ( ( ( y_t ) == ( getY(object) + getHeight(object) ) ) && ( x_l == ( getX(object) + getWidth(object) ) ) )
                        { 
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                            velocityY = -( velocityY + dirY * veloCoef * velocityYS );
                        }
                        // bounce right
                        else if ( ( ( y_t ) < ( getY(object) + getHeight(object) ) ) && ( x_l <= ( getX(object) + getWidth(object) ) ) )
                        {
                            velocityX = -( velocityX + dirX * veloCoef * velocityXS );
                        }
                    }
                }
                removeGWindow(window, object);
                --bricks;
                ++points;
                updateScoreboard(window, label, points);
            }
        }
        // linger before moving again
        pause( linger );
        
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    GRect brick;
    int x = ( WIDTH - ( (brickW + gapH) * (COLS - 1) + brickW ) ) / 2;
    int y = 90;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            bricks[i][j] = brick = newGRect(x + j*(brickW + gapH), y + i*(brickH + gapV), brickW, brickH);
            setFilled(brick, true);
            setColor(brick, colors[i]);
            add(window, brick);            
        }
    }
}


/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    double x = (WIDTH / 2) - RADIUS;
    double y = (HEIGHT / 2) - RADIUS;
   
    GOval ball = newGOval(x, y, 2 * RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);

    if (ball) return ball;
    return NULL;
}


/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    int widthP = 3 * (2 * RADIUS);
    double x = ( WIDTH - widthP ) / 2;
    double y = ( HEIGHT - 2 * (2 * RADIUS) - brickH);
    GRect paddle = newGRect(x, y, widthP, brickH); 
    
    setFilled(paddle, true);
    setColor(paddle, "DARK_GRAY");
    add(window, paddle);            
    
    if (paddle) return paddle;
    return NULL;
}


/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // instantiate label
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-36");
    setColor(label, "LIGHT_GRAY");
    add(window, label);
    
    // center label
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    
    if (label) return label;
    return NULL;
}


/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}



/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initLivesboard(GWindow window)
{

    // instantiate label
    GLabel label = newGLabel("");
    setFont(label, "SansSerif-36");
    setColor(label, "MAGENTA");
    add(window, label);
    setLocation(label, 5, 30);

    if (label) return label;
    return NULL;
}


/**
 * Updates Livesboard's label
 */
void updateLivesboard(GWindow window, GLabel label, int lives)
{
    // update label
    char s[12];
    sprintf(s, "Lives: %i", lives);
    setLabel(label, s);
//    setLocation(label, 1, 1);
}







/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
