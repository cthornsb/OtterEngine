#include <iostream>
#include <algorithm>

// Include Simple DirectMedia Layer (SDL)
#include <SDL2/SDL.h>

#include "sdlWindow.hpp"

void sdlKeypress::decode(const SDL_Keysym* sym, const bool &isDown){
	key = sym->sym;
	none   = sym->mod & KMOD_NONE;
	lshift = sym->mod & KMOD_LSHIFT;
	rshift = sym->mod & KMOD_RSHIFT;
	lctrl  = sym->mod & KMOD_LCTRL;
	rctrl  = sym->mod & KMOD_RCTRL;
	lalt   = sym->mod & KMOD_LALT;
	ralt   = sym->mod & KMOD_RALT;	
	lgui   = sym->mod & KMOD_LGUI;
	rgui   = sym->mod & KMOD_RGUI;
	num    = sym->mod & KMOD_NUM;
	caps   = sym->mod & KMOD_CAPS;
	mode   = sym->mod & KMOD_MODE;
	keyDown = isDown;
}

sdlWindow::~sdlWindow(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void sdlWindow::setDrawColor(const sdlColor &color, const float &alpha/*=1*/){
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, sdlColor::toUChar(alpha));
}

void sdlWindow::clear(const sdlColor &color/*=Colors::BLACK*/){
	setDrawColor(color);
	SDL_RenderClear(renderer);
}

void sdlWindow::drawPixel(const int &x, const int &y){
	SDL_RenderDrawPoint(renderer, x, y);
}

void sdlWindow::drawPixel(const int *x, const int *y, const size_t &N){
	for(size_t i = 0; i < N; i++) // Draw N pixels
		drawPixel(x[i], y[i]);
}

void sdlWindow::drawLine(const int &x1, const int &y1, const int &x2, const int &y2){
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void sdlWindow::drawLine(const int *x, const int *y, const size_t &N){
	if(N == 0) // Nothing to draw
		return;
	for(size_t i = 0; i < N-1; i++)
		drawLine(x[i], y[i], x[i+1], y[i+1]);
}

void sdlWindow::render(){
	SDL_RenderPresent(renderer);
}

bool sdlWindow::status(){
	static SDL_Event event;
	if(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_KEYDOWN:
				lastKey.decode(&event.key.keysym, true);
				break;
			case SDL_KEYUP:
				lastKey.decode(&event.key.keysym, false);
				break;
			case SDL_QUIT:
				return false;
			default:
				break;
		}
	}
	return true;
}

void sdlWindow::initialize(){
	if(init) return;

	// Open the SDL window
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(W, H, 0, &window, &renderer);
	clear();
	
	init = true;
}
