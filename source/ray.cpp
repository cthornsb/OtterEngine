#include <iostream>

#include "ray.hpp"

void ray::dump() const {
	std::cout << " RAY::\n";
	pos.dump();
	dir.dump();
}
