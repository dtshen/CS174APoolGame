#ifndef SPHERE_H
#define SPHERE_H


#include "Angel/Angel.h"
#include "Shapes.h"
#include "Constants.h"

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;

extern int numBallsIn;

struct shaderVariables {
	GLuint uModelView, uProjection, uView;
	GLint uAmbient, uDiffuse, uSpecular, uLightPos, uShininess, uTex, uEnableTex; 
};

struct circle {
	point4 pos;
	float radius;

};
struct Sphere {
	point4 pos; //current position of ball
	mat4 rotateMat; //matrix to rotate the ball
	vec3 velocity;
	vec3 friction;
	GLuint texture; //texture of ball
	float rad; //radius
	bool moving;//if the ball is moving
	bool falling; //if the ball falls into a pocket
	float angular_rot; //angular rotation
};

void reset(Sphere* s);
void applyGravity(Sphere* s, float diff);
bool willBallsCollide(Sphere* s1, Sphere* s2);
void doBallCollision(Sphere* s1, Sphere* s2, float diff);
void wallCollision(Sphere* s);
void drawBall(ShapeData* s, shaderVariables* var, mat4 model_view, GLuint tex);
void moveBall(Sphere* s, float diff)
;
float distance(point4 from, point4 to);


bool ballFallsInPocket(Sphere* s, circle* c);
void drawCircle(shaderVariables* var, circle* c, mat4 model_view);
void createCircle(point4 v[], point4 p1[]);

#endif