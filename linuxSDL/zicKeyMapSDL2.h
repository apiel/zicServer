#ifndef ZIC_KEY_MAP_SDL_H_
#define ZIC_KEY_MAP_SDL_H_

#include <SDL2/SDL.h>

// LAPTOP
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#define KEY_A SDL_SCANCODE_S
#define KEY_B SDL_SCANCODE_A
#define KEY_MENU SDL_SCANCODE_ESCAPE

#if ZIC_TARGET == 1
// OPENDINGUX
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#define KEY_A 224
#define KEY_B 226
#define KEY_MENU 40
#endif

#if ZIC_TARGET == 2
// MIYOO
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#define KEY_A 224
#define KEY_B 226
#define KEY_MENU 40
#endif

#endif