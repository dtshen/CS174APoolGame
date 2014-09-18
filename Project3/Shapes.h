#ifndef _SHAPES_H_
#define _SHAPES_H_

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

struct ShapeData
{
    GLuint vao;
    int numVertices;
};

void generateCube(GLuint program, ShapeData* cubeData, float zoom);
void generateSphere(GLuint program, ShapeData* sphereData);
void generateCone(GLuint program, ShapeData* coneData);
void generateCylinder(GLuint program, ShapeData* cylData);
void generateHole(GLuint program, ShapeData* holeData);
void generateTriPrism(GLuint program, ShapeData* triPrismData);
void generateQuarterHole(GLuint program, ShapeData* quarterHoleData);
void generateEdgeCorner(GLuint program, ShapeData* quarterHoleData);
void generateRoundCorner(GLuint program, ShapeData* roundCornerData);
void genWall(GLuint program, ShapeData* walls);
#endif
