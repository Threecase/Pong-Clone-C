/*
 * Graphics header
 *
 */

#ifndef __GRAPHICS_H
#define __GRAPHICS_H


int get_input(void);

void draw_frame(void);

void draw_number (int x, int y, int n);
void draw_rect (int x, int y, int w, int h);

void init_graphics(void);
void shutdown_graphics(void);


#endif

