#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <unistd.h>

#include "vector3.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "cube.hpp"

int main(){
	/*triangle tri(vector3(1, -1.25, 0),
	             vector3(0, 1, 0),
	             vector3(-1, -1, 0));

	camera cam(vector3(0, 0, -5));
	cam.clear();
	cam.draw(tri, camera::SOLID);
	cam.render();
	cam.loop();*/

	cube myCube(vector3(), 1, 1, 1);
	myCube.rotate(30*deg2rad, 30*deg2rad, 30*deg2rad);
	std::vector<triangle> *tris = myCube.getPolygons();
	
	camera cam(vector3(0, 0, -5));
	//cam.setDrawNormals();

	double angle[3] = {0.12, 0.07, 0.17};
	for(int i = 0; i < 1000; i++){
		cam.clear();
		cam.draw(tris, camera::RENDER);
		cam.render();
		
		cam.moveCam(0.002);
		myCube.rotate(angle[0]*deg2rad, angle[1]*deg2rad, angle[2]*deg2rad);
		usleep(16700); // ~60 Hz
	}
	
	return 0;
}
