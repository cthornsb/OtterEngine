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
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, sdlColor::convertToUChar(alpha));
}

void sdlWindow::clear(const sdlColor &color/*=Colors::BLACK*/){
	setDrawColor(color);
	SDL_RenderClear(renderer);
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

void sdlWindow::drawTriangle(const int *x, const int *y, const bool &fill/*=false*/){
	if(!fill){
		for(size_t i = 0; i < 2; i++)
			drawLine(x[i], y[i], x[i+1], y[i+1]);
		drawLine(x[2], y[2], x[0], y[0]);
	}
	else{
		int x0, y0;
		int x1, y1;
		int x2, y2;
		
		x0 = x[0]; y0 = y[0];
		x1 = x[1]; y1 = y[1];
		x2 = x[2]; y2 = y[2];

		// Sort points by ascending Y
		// Insertion sort
		if(y1 < y0){
			std::swap(y0, y1);
			std::swap(x0, x1);
		}
		if(y2 < y1){
			std::swap(y1, y2);
			std::swap(x1, x2);
			if(y1 < y0){
				std::swap(y1, y0);
				std::swap(x1, x0);
			}
		}
		
		float xA, xB;
		for(int scanline = y0; scanline <= y2; scanline++){
			if(y0 == y1){ // y10 is a horizontal line
					xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
					xB = (scanline-y2)*(float(x0-x2))/(y0-y2) + x2;				
			}
			else if(y0 == y2){ // y20 is a horizontal line
					xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
					xB = (scanline-y0)*(float(x1-x0))/(y1-y0) + x0;			
			}
			else{
				if(scanline <= y1)
					xA = (scanline-y0)*(float(x1-x0))/(y1-y0) + x0;
				else
					xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
				xB = (scanline-y2)*(float(x0-x2))/(y0-y2) + x2;
			}

			// Draw the scanline
			if(xA <= xB)
				drawLine((int)xA, scanline, (int)xB, scanline);
			else
				drawLine((int)xB, scanline, (int)xA, scanline);
		}
	}
}

void drawPolygon(const int *x, const int *y, const size_t &N, const bool &fill/*=false*/){
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

	// For testing
	/*setDrawColor(Colors::RED);
	for(int i = 0; i < W; ++i)
		SDL_RenderDrawPoint(renderer, i, i);
	render();*/
	
	init = true;
}
