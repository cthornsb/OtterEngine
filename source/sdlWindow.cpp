#include <iostream>
#include <algorithm>

// Include Simple DirectMedia Layer (SDL)
#include <SDL2/SDL.h>

#include "sdlWindow.hpp"

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
	if(SDL_PollEvent(&event) && event.type == SDL_QUIT) // Window is closed
		return false;
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
