#ifndef _DRAWER_H_
#define _DRAWER_H_

////////////////////////////////////////////////////
//
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#  include <GL/glew.h>
// #  include <GL/glut.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
#include "GL/freeglut.h"
#else
//#include <GLUT/glut.h>
#endif

#include "sphere.h"
#include "Timer.h"
#include "Shapes.h"

#include "Angel/Angel.h"
#include "Constants.h"
/********************************************************************************************************/

class Drawer {
public:
	// Structs that hold the Vertex Array Object index and number of vertices of each shape.
	ShapeData m_cubeData;
	//ShapeData sphereData;
	ShapeData m_coneData;
	ShapeData m_cylData;

	ShapeData m_holeData;
	ShapeData m_quarterHoleData;
	ShapeData m_triPrismData;
	ShapeData m_edgeCornerData;
	ShapeData m_roundCornerData;

	mat4 m_model_view;
	mat4 m_original_model_view;
	shaderVariables m_vars;
	/*********************************************************
		PROC: set_colour();
		DOES: sets all material properties to the given colour
		-- don't change
	**********************************************************/

//	Drawer();
//	virtual ~Drawer();
	void set_colour(float r, float g, float b)
	{
		float ambient  = 0.2f;
		float diffuse  = 0.6f;
		float specular = 0.5f;
		glUniform4f(m_vars.uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
		glUniform4f(m_vars.uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
		glUniform4f(m_vars.uSpecular, specular*r, specular*g, specular*b, 1.0f);
	}

	void initDrawer(GLuint program);

	void setDrawerParam(mat4 m_view, shaderVariables m_vars);

	/////////////////////////////////////////////////////
	//    PROC: drawCylinder()
	//    DOES: this function
	//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1.
	//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
	//
	//
	// Don't change.
	//////////////////////////////////////////////////////
	void drawCylinder(void);

	//////////////////////////////////////////////////////
	//    PROC: drawCone()
	//    DOES: this function
	//          render a solid cone oriented along the Z axis with base radius 1.
	//          The base of the cone is placed at Z = 0, and the top at Z = 1.
	//
	// Don't change.
	//////////////////////////////////////////////////////
	void drawCone(void);

	//////////////////////////////////////////////////////
	//    PROC: drawCube()
	//    DOES: this function draws a cube with dimensions 1,1,1
	//          centered around the origin.
	//
	// Don't change.
	//////////////////////////////////////////////////////
	void drawCube(void);

	//////////////////////////////////////////////////////
	//    PROC: drawQuarterHole()
	//////////////////////////////////////////////////////
	void drawQuarterHole(void);

	//////////////////////////////////////////////////////
	//    PROC: drawHole()
	//////////////////////////////////////////////////////
	void drawHole(void);

	//////////////////////////////////////////////////////
	//    PROC: drawTriPrism()
	//////////////////////////////////////////////////////
	void drawTriPrism(void);

	//////////////////////////////////////////////////////
	//    PROC: drawEdgeCorner()
	//////////////////////////////////////////////////////
	void drawEdgeCorner(void);

	//////////////////////////////////////////////////////
	//    PROC: drawSphere()
	//    DOES: this function draws a sphere with radius 1
	//          centered around the origin.
	//
	// Don't change.
	//////////////////////////////////////////////////////
	void drawSphere(void);

	void drawTable(void);

private:
	void drawHolesOnTable(void);
	void drawTablePlane(void);
	void drawTableEdge(void);
	void drawTableBottom(void);
	void drawRoundCorner(void);
};


#endif /* DRAWER_H_ */
