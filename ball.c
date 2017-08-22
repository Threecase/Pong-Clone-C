
#include <SDL2/SDL.h>

#include "bumper.h"
#include "consts.h"
#include "ball.h"


// set the x/y coords of the ball
void ball_setpos (Ball* b, int x, int y) {
    b->x = (x <= SCREEN_WIDTH) ? x : b->x;
    b->y = (y <= SCREEN_HEIGHT) ? y : b->y;
}

// get the x/y coords of the ball
int ball_getx (Ball* b) { return b->x; }
int ball_gety (Ball* b) { return b->y; }

// set the width/height of the ball
void ball_setsize (Ball* b, int w, int h) {
    b->w = w;
    b->h = h;
}

// get the width/height of the ball
int ball_getw (Ball* b) { return b->w; }
int ball_geth (Ball* b) { return b->h; }

// render the ball
void ball_draw (Ball* b) {

    // the rect to draw
    SDL_Rect r = { b->x - b->w/2, b->y - b->h/2, b->w, b->h };

    // store the previous draw color
    unsigned char red, green, blue, alpha;
    SDL_GetRenderDrawColor (b->ren, &red, &green, &blue, &alpha);

    // set the draw color to ball color + render ball
    SDL_SetRenderDrawColor (b->ren, 0, 0, 255, 255);
    SDL_RenderFillRect (b->ren, &r);

    // reset the draw color
    SDL_SetRenderDrawColor (b->ren, red, green, blue, alpha);
}



// checks if ball is colliding with bumper
// returns 1 if it is, 0 otherwise
int ball_checkcollisions (Ball* b, Bumper* bumperlist, int listsize) {

    for (int i = 0; i < listsize; i++) {
        // bumper variables
        int bumperx = bumperlist[i].getx (&bumperlist[i]), bumpery = bumperlist[i].gety (&bumperlist[i]);
        int bumperw = bumperlist[i].getw (&bumperlist[i]), bumperh = bumperlist[i].geth (&bumperlist[i]);

        // ball variables
        int x = b->getx (b), y = b->gety (b);
        int w = b->getw (b), h = b->geth (b);

        // bools
        int xbound = 0, ybound = 0;

        // check if the ball is within the x constraints of the bumper
        if ((x - w/2 >= bumperx - bumperw/2 && x - w/2 <= bumperx + bumperw/2) || (x + w/2 >= bumperx - bumperw/2 && x + w/2 <= bumperx + bumperw/2)) {
            xbound = 1;
        }
        // check if the ball is within the y constraints of the bumper
        if ((y - h/2 >= bumpery - bumperh/2 && y - h/2 <= bumpery + bumperh/2) || (y + h/2 >= bumpery - bumperh/2 && y + h/2 <= bumpery + bumperh/2)) {
            ybound = 1;
        }

        // reverse movement direction if there is a collision with a new bumper
        int lastcollidedinlist = 1;
        for (int n = 0; n < listsize; n++) {
            if (b->lastcollided == (int*)&bumperlist[n]) {
                lastcollidedinlist = 0;
                break;
            }
        }
        if (xbound && ybound && lastcollidedinlist) {
            b->lastcollided = (int*)&bumperlist[i];
            // change the ball's direction
            b->deltax = bumperlist[i].getxreflect (&bumperlist[i]);
            b->deltay = bumperlist[i].getyreflect (&bumperlist[i]);
            b->xdirection = -b->xdirection;
        }
    }
}

// ball motion
void ball_movement (Ball* b) {

    // throttle movement speed
    static int count = 0;
    count = (count >= GAME_SPEED) ? 0 : count + 1;
    if (count != 0) return;

    // new coords
    int newx = b->x, newy = b->y;

    // ball bounces off of screen edges
    if (b->x >= SCREEN_WIDTH || b->x <= 0) {
        b->xdirection = -b->xdirection;
        if (b->x <= 0) b->lastcollided = LEFT;
        else b->lastcollided = RIGHT;
        b->setpos (b, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
        b->deltax = 1, b->deltay = 0;
        b->xdirection = 1, b->ydirection = 1;
    }
    if (b->y >= SCREEN_HEIGHT || b->y <= 0) {
        b->ydirection = -b->ydirection;
        if (b->y <= 0) b->lastcollided = TOP;
        else b->lastcollided = BOTTOM;
    }

    // set new position
    newx = b->x + (b->deltax * b->xdirection);
    newy = b->y + (b->deltay * b->ydirection);

    // set ball's location
    b->setpos (b, newx, newy);
}

// main loop of the ball
int ball_main (Ball* ball, Bumper** bumpers, int numbumpers, int bumperlength) {

    // check for collisions
    for (int i = 0; i < numbumpers; i++)
        ball->checkcollisions (ball, bumpers[i], bumperlength);

    // move the ball
    ball->movement (ball);

    // draw the ball
    ball->draw (ball);

    return *ball->lastcollided;
}

// initialize a new ball
void newball (Ball* ball, SDL_Renderer* ren, int x, int y, int w, int h) {

    // set methods
    // coord functions
    ball->setpos = ball_setpos;
    ball->getx = ball_getx;
    ball->gety = ball_gety;

    // size functions
    ball->setsize = ball_setsize;
    ball->getw = ball_getw;
    ball->geth = ball_geth;

    // main loop functions
    ball->draw = ball_draw;
    ball->checkcollisions = ball_checkcollisions;
    ball->movement = ball_movement;
    ball->main = ball_main;


    // initialize the ball's values
    ball->setpos (ball, x, y);
    ball->setsize (ball, w, h);
    ball->deltax = 1, ball->deltay = 0;
    ball->xdirection = 1, ball->ydirection = 1;
    ball->ren = ren;
}
