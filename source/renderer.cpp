#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <unistd.h>

#include "vector3.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "colors.hpp"

int main(){
	// Define a new cube
	cube myCube(vector3(), 1, 1, 1);
	std::vector<triangle> *tris = myCube.getPolygons();
	
	// Setup the camera at z=-5 m
	camera cam(vector3(0, 0, -5));
	
	// Show some info about the camera
	cam.dump();
	
	// Set the camera to draw surface normal vectors
	//cam.setDrawNormals();

	// "Animate" the cube by rotating it and moving the camera
	for(int i = 0; i < 1000; i++){
		// Clear the screen with a color
		cam.clear(Colors::BLACK);
		
		// Draw the cube mesh
		cam.draw(tris, camera::SOLID); // Currently, draw options [WIREFRAME, MESH, SOLID, RENDER] are supported
		
		// Update the screen
		cam.render();
		
		// Move the camera forward
		cam.moveCam(0.002);
		
		// Rotate the cube
		myCube.rotate(0.12*deg2rad, 0.07*deg2rad, 0.17*deg2rad);
		
		// Sleep for a short time (16.7 ms ~= 60 fps) to avoid rendering too fast
		usleep(16700);
	}
	
	return 0;
}
