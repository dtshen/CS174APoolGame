#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define TABLE_LENGTH (24.0f)
#define TABLE_WIDTH  (12.0f)
#define TABLE_DEPTH  (0.3)
#define TABLE_HEIGHT (0.3)

#define TABLE_LEN (TABLE_LENGTH + 3.0f)
#define TABLE_WID (TABLE_WIDTH + 3.0f)

#define ROOM_LENGTH (200.0f)
#define ROOM_HEIGHT (100.0f)
#define ROOM_WIDTH (200.0f)

#define FRICTION_CONST 0.1
//this is for the points to draw the circles
#define trianglesPerQuadrant 10
#define totalQuadrants 4
#define numTrianglePts 3
#define numtriangles (2*trianglesPerQuadrant*3+1)
#define totalpoints (trianglesPerQuadrant*totalQuadrants*numTrianglePts)

#endif
