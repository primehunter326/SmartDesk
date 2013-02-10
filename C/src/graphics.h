
#ifndef __GRAPHICS_H__
	#define __GRAPHICS_H__

#include <SDL.h>
#include <SDL/SDL.h>

#include "menu.h"

typedef struct{
	void * memShareAddr;
	int memShareFD;
	SDL_Surface *screen;
	Menu * menu;
	int stopFlag;
} GraphicModule;

/*Set's a pixel on the screen at the x,y coordinates to the r g b value.
*/
void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b);

/*Clears the screen to all white
* 	screen: SDL_Surface to draw on
*/
void clearScreen(SDL_Surface* screen);

/*Runs the GraphicModule for the program
*	module: GraphicModule to run
*/
void runGraphics(GraphicModule * module);

/*Handles all graphics events. Welcome to the bottleneck boys
*	eventType: An SDL event type
*	module: GraphicModule currently in use
*/
void handleGraphicEvent(SDL_Event event, GraphicModule * module);

/*
*Set's up the graphics module
*	fd: File Descriptor of the memory mapped file shared between graphics and network
*	module: Pointer to a GraphicModule struct to setup
*	returns -1 on failure to set up the graphics module, 0 on success
*/
int setupGraphicModule(int fd, GraphicModule * module);

/*Handles all key presses
*	event: The SDL_Event to handle
*	module: Pointer to a GraphicModule where we are handling events
*/
void handleKeyEvent(SDL_Event  event, GraphicModule * module);

/*Handles all Mouse events
*	event: The SDL_Event to handle
*	module: GraphicModule currently in use
*/
void handleMouseEvent(SDL_Event event, GraphicModule * module);

/*Takes the current mouse coordinates and there relative position and smooths a line between
*	x: X coordinate of current mouse position
*	y: Y coordinate of current mouse position
*	xrel: x's relative change since the last mouse event
*	yrel: y's relative change since the last mouse event
*/
void smoothPath(Uint16 x, Uint16 y, Sint16 xrel, Sint16 yrel);

/*Loads the User Interface to the graphics module
*	module: The GraphicModule to load a UserInterface to
*	returns -1 on failing to load the user interface, 0 on successs
*/
int loadUI(GraphicModule * module);

#endif