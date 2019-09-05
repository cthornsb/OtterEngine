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
#include "scene.hpp"

int main(){
	// Define a new cube
	cube myCube(vector3(), 1, 1, 1);

	// Set the render mode for our cube
	myCube.setDrawingMode(scene::RENDER); // Currently, draw options [WIREFRAME, MESH, SOLID, RENDER] are supported
	
	// Setup the camera at z=-5 m
	camera cam(vector3(0, 0, -5));
	
	// Show some info about the camera
	cam.dump();
	
	// Setup the scene with our camera
	scene myScene(&cam);

	// Set the camera to draw surface normal vectors
	//myScene.setDrawNormals();

	// Add the cube to the scene
	myScene.addObject(&myCube);

	// Variable to track the "time"
	float dummyTime = 0;

	// "Animate" the cube by rotating it and moving the camera
	while(myScene.getStatus()){
		// Update the screen by drawing the geometry
		myScene.update(); // 
		
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
