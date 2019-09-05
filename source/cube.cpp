#include "cube.hpp"

cube::cube(const vector3 &pos_, const double &X, const double &Y, const double &Z) : object(pos_), hX(X/2), hY(Y/2), hZ(Z/2) {
	build();
}

void cube::build(){
	// Add all vertices
	addVertex(hX, hY, hZ); 
	addVertex(hX, -hY, hZ); 
	addVertex(-hX, -hY, hZ);
	addVertex(-hX, hY, hZ);
	addVertex(hX, hY, -hZ); 
	addVertex(hX, -hY, -hZ);
	addVertex(-hX, -hY, -hZ); 
	addVertex(-hX, hY, -hZ);

	// Add all polygons (using clockwise winding)
	addPolygon(0, 1, 2);
	addPolygon(2, 3, 0);
	addPolygon(1, 0, 4);
	addPolygon(4, 5, 1);
	addPolygon(5, 4, 7);
	addPolygon(7, 6, 5);
	addPolygon(7, 3, 2);
	addPolygon(2, 6, 7);
	addPolygon(7, 4, 0);
	addPolygon(0, 3, 7);
	addPolygon(6, 2, 1);
	addPolygon(1, 5, 6);
}
