#ifndef SPHERE_H
#define SPHERE_H

#include "Angel/Angel.h"
#include "Shapes.h"

extern float tableWidth;
extern float tableHeight;
extern float frictionConst;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;
/*struct shaderVariables {
	GLuint uModel, uProjection, uView;
	GLint uAmbient, uDiffuse, uSpecular, uLightPosition, uShininess, uText, uEnableText; 
};*/
struct shaderVariables {
	GLuint uModelView, uProjection, uView;
	GLint uAmbient, uDiffuse, uSpecular, uLightPos, uShininess, uTex, uEnableTex; 
};

/*************

**************/
struct sphere {
	point4 currpos; //keeps the current position of the ball, which is the center 
	point4 center; // keeps track of the center of the ball, but probably dont need it anymore
	vec3 velocity; //the velocity of vall. the maximum should be 0.08 for x and y 
	float radius; //keeps the radius
	//vec3 move; //this is passed to the translate matrix and is updated by the velocity
	GLuint vao; //to bind the vertex array
	vec3 friction; //friction constant that is proportional to the velocity
	bool moving; //to keep track if the ball is moving
	int numvertices; //keep track of how many vertices are there in a sphere
	float angular_rot;
	float vangle; // the angle from the x-axis to the velocity direction
	bool collided;
	mat4 rotateMat;
};

void wallCollision(sphere* sphere1);
bool pocketCollision(sphere* sphere1);
void moveBall(sphere* sphere1, float diff);
void ballCollision(sphere* circle1, sphere* circle2, float diff);
float distance(point4 from, point4 to);
void drawBall(ShapeData* sphere1, shaderVariables* var, mat4 model, mat4 view, GLuint texture);
#endif
