#include "Vertex.hpp"
#include "matrix3.hpp"

void Vertex::transform(const matrix3* mat) { 
	mat->transform(pos); 
}

/** Reset position and normal to initial values
  */
void Vertex::reset() { 
	pos = pos0;
}