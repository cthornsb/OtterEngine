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
	
	// Setup the camera at z=-5 m
	camera cam(vector3(0, 0, -5));
	
	// Show some info about the camera
	cam.dump();
	
	// Set the camera to draw surface normal vectors
	//cam.setDrawNormals();

	// Variable to track the "time"
	float dummyTime = 0;

	// "Animate" the cube by rotating it and moving the camera
	while(cam.getStatus()){
		// Clear the screen with a color
		cam.clear(Colors::BLACK);
		
		// Draw the cube mesh
		cam.draw(&myCube, camera::SOLID); // Currently, draw options [WIREFRAME, MESH, SOLID, RENDER] are supported
		
		// Update the screen
		cam.update();
		
		// Move the camera forward
		cam.moveCam(0.002);
		
		// Rotate the cube
		myCube.rotate(0.12*deg2rad, 0.07*deg2rad, 0.17*deg2rad);
		
		// Move the cube along the x-axis
		myCube.setPosition(vector3(std::sin(0.02*dummyTime), 0, 0));
		
		// Sleep for a short time (16.7 ms ~= 60 fps) to avoid rendering too fast
		usleep(16700);
		
		// Update the "time"
		dummyTime += 1;
	}
	
	return 0;
}
