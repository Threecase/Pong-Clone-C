/*
 * Pong clone in C using SDL2
 *
 * FIXME Enemy bumper sections not moving perfectly
 *  - Flickering between sections
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "ball.h"
#include "bumper.h"
#include "consts.h"
#include "util.h"


// define global variables
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int* LEFT = &_LEFT;
const int* RIGHT = &_RIGHT;
const int* TOP = &_TOP;
const int* BOTTOM = &_BOTTOM;
int GAME_SPEED = 4;

int main (int argc, char* argv[]) {

    // initialize
    if (!init()) return 1;
    if (argc > 1) GAME_SPEED = atoi (argv[1]);

    // score
    int leftscore = 0, rightscore = 0;

    SDL_Window* win = SDL_CreateWindow ("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!win) SDL_Log ("Failed to create window: %s\n", SDL_GetError());

    SDL_Renderer* ren = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) SDL_Log ("Failed to create renderer: %s\n", SDL_GetError());

    // bumper variables
    int bumpersections = 3;
    int size = 75/bumpersections;

    // create the bumpers
    Bumper enemy[bumpersections];
    Bumper bumper[bumpersections];
    for (int i = 0; i < bumpersections; i++) {
        Bumper bump;
        //                  renderer,   x,                  y,                                          width,  height, xreflect,   yreflect
        newbumper (&bump,   ren,        50,                 (SCREEN_HEIGHT/2) - size + (size * i),      15,     size,   1,          abs (bumpersections/2 - i));
        bumper[i] = bump;

        newbumper (&bump,   ren,        SCREEN_WIDTH-50,    (SCREEN_HEIGHT/2) - size + (size * i),      15,     size,   1,          bumpersections/2 - i);
        enemy[i] = bump;
    }

    Bumper* bumpers[] = { bumper, enemy };

    // create the game ball
    Ball ball;
    newball (&ball, ren, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 15, 15);

    // main loop
    int lastthing = -1;
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        // event handling
        while (SDL_PollEvent (&e)) {
            switch (e.type) {
                // quit event
                case SDL_QUIT:
                quit = 1;
                break;

                // mouse movement
                case SDL_MOUSEMOTION: {
                int x, y;
                SDL_GetMouseState (&x, &y);
                for (int i = 0; i < bumpersections; i++)
                    bumper[i].setpos (&bumper[i], 50, (y - size) + (size * i));
                break; }
            }
        }

        // background color is black
        SDL_SetRenderDrawColor (ren, 0, 0, 0, 255);
        // clear the screen
        SDL_RenderClear (ren);


        // bumper actions
        int delay = random() % 2;
        for (int i = 0; i < bumpersections; i++) {
            enemy[i].ai (&enemy[i], &ball, enemy[bumpersections/2].gety (&enemy[bumpersections/2]), -size + (size * i), delay);
        }

        // ball actions
        int thing = ball.main (&ball, bumpers, 2, 3);

        // adjust the score if necessary
        if (thing != lastthing && thing == *LEFT) rightscore++;
        if (thing != lastthing && thing == *RIGHT) leftscore++;
        if (leftscore > 9 || rightscore > 9) break;
        lastthing = thing;

        // draw game entities
        for (int i = 0; i < bumpersections; i++) {
            bumper[i].draw (&bumper[i]);
            enemy[i].draw (&enemy[i]);
        }

        // draw the score
        drawnumber (ren, leftscore, LEFT);
        drawnumber (ren, rightscore, RIGHT);

        // render window
        SDL_RenderPresent (ren);
    }

    SDL_Log ("GAME OVER!\n");

    cleanup (ren, win);

    return 0;
}
