#ifndef __OBJ_LOADER_H
#define __OBJ_LOADER_H

#include <vector>
#include "Angel.h"
typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;
// Load an OBJ model into the out parameters.
// Note only simple OBJ files are supported.
bool loadObject(const char * filename, 
	std::vector<point4> &outPositions, 
	std::vector<int> &outIndices,
	std::vector<point3> &outNormal = std::vector<point3>(), 
	std::vector<point2> &outUv = std::vector<point2>(),
	float scale = 1.0f
	);
#endif