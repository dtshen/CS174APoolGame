////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
/**************************
Paul Rodriguez
Wei Zou
Dennis Shen 
all rights reserved
**************************/
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

//#include "collision.h"
#include "Sphere.h"
#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "tga.h"

#include "Angel/Angel.h"
#include "Constants.h"
#include "Drawer.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif
/********************************************************************************************************/

//typedef Angel::vec4 color4;
//counters for loops
int i = 0;
int j = 0;
int k = 0;


#define CAMERA_INIT_X 0.0f
#define CAMERA_INIT_Y -5.0f
#define CAMERA_INIT_Z -50.0f
#define CAMERA_INIT_AZIMUTH 90.0f

//to control the camera
float angle = 0;
float azimuth = 0;
float xpos = CAMERA_INIT_X, ypos = CAMERA_INIT_Y, zpos = CAMERA_INIT_Z, zoom = 0;
float theta;
bool animate = false;
float power = 0.0f;
float POWER_LIMIT=15.0f;
float projangle = 0.0f;

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 600;
int Height = 600 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
/*********************************************************
Turn based variables
**********************************************************/
bool drawable[16]={true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true};
bool tempPockets[16]={true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true};
bool turn=true;
int player1type=0;
int player2type=0;
bool firstballin=false;
bool testPocket=true;
bool SpacePressed=false;
int win=0;
/**************************************************************/
const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//texture

GLuint texture_cube;
GLuint texture_earth;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
//ShapeData cubeData;
ShapeData sphereData;
//ShapeData coneData;
//ShapeData cylData;

//ShapeData holeData;
//ShapeData quarterHoleData;
//ShapeData triPrismData;
//ShapeData edgeCornerData;

//store the balls here
Sphere balls[16];
GLuint ball_texture[16];
GLuint exTex[1];
shaderVariables vars;

Drawer mDrawer;


#ifdef __linux__
double keyPressTime = 0.0f, keyReleaseTime = -1.0f;
#endif

void set_colour(float r, float g, float b)
{
	float ambient  = 0.0f;
	float diffuse  = 1.2f;
	float specular = 0.3f;
	glUniform4f(vars.uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
	glUniform4f(vars.uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
	glUniform4f(vars.uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view, original_model_view, view, proj, rotate;
/*GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;
*/
// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye = Angel::vec4(0, 0.0, 50.0,1.0);
Angel::vec4 ref = Angel::vec4(0.0, 0.0, 0.0,1.0);
Angel::vec4 up = Angel::vec4(0.0,1.0,0.0,0.0);

double TIME = 0.0 ;
float LAST_TIME = 0.0;
float diff;

GLuint vao[4]; //keeps track of vertex arrays
ShapeData wall;


circle pockets[6];

point4 circlePts[numtriangles]; 
point4 p[totalpoints/3+1];
//to draw the line where we are shooting to
point4 lineprojection[2] = {point4(0,0,0,1), point4(10,0,0,1)}; 
//draws the powerbar
point4 powerbox[4]={point4( .5, -.75, .15, 1 ), point4( .5, -.9, .15, 1 ), point4( -.5, -.75, .15, 1 ), point4( -.5, -.9, .15, 1 )};

/*******************************

this is to set the positions of the balls

*******************************/


void setballpos() {

	
	float startline = 0;
	float movetoy = 1.0f - TABLE_DEPTH;
	balls[0].pos = point4(-8,movetoy,0.0,1.0);
	balls[15].pos = point4( startline+5, movetoy, 0.0, 1.0 );
	//balls[0].setPos();
	int h = 0; //counter for array
	float xdistance;
	//middle pockets
	pockets[0].pos = point4(0,movetoy-balls[0].rad,TABLE_WIDTH/2+1,1.0f);
	pockets[1].pos = point4(0,movetoy-balls[0].rad,-TABLE_WIDTH/2-1,1.0f);
	//top pockets
	pockets[2].pos = point4(TABLE_LENGTH/2+0.5,movetoy-balls[0].rad,-TABLE_WIDTH/2-0.5,1.0f);
	pockets[3].pos = point4(TABLE_LENGTH/2+0.5,movetoy-balls[0].rad,TABLE_WIDTH/2+0.5,1.0f);
	//bottom pockets
	pockets[4].pos = point4(-TABLE_LENGTH/2-0.5,movetoy-balls[0].rad,-TABLE_WIDTH/2-0.5,1.0f);
	pockets[5].pos = point4(-TABLE_LENGTH/2-0.5,movetoy-balls[0].rad,TABLE_WIDTH/2+0.5,1.0f);


	//balls[15].setPos();

	startline += 2*balls[0].rad+5;
	xdistance = balls[0].rad +0.1;
	for (h = 13; h <= 14; h++) {
		balls[h].pos = point4( startline, movetoy , xdistance, 1.0 );
		//balls[h].setPos();
		xdistance -= ( 2*balls[h].rad+0.1 );
	}

	//third row
	startline += 2*balls[0].rad;
	xdistance = 2*balls[0].rad+0.1;
	for (h=10; h <=12;h++) {
		balls[h].pos = point4( startline, movetoy , xdistance, 1.0 );
	//	balls[h].setPos();
		xdistance -= (2*balls[h].rad+0.1);
	}

	//second row
	startline += 2*balls[0].rad;
	xdistance = balls[0].rad*3+0.1;
	for(h = 6; h<=9;h++) {
		balls[h].pos = point4( startline, movetoy , xdistance, 1.0 );
	//	balls[h].setPos(point4( startline, movetoy , xdistance, 1.0 )) ;
		xdistance -= (2*balls[h].rad+0.1);
	}

	startline += 2*balls[0].rad;
	xdistance = balls[0].rad*4 + 0.1;
	//first row
	for(h = 1; h <=5; h++ ) {
		balls[h].pos =point4( startline, movetoy , xdistance, 1.0 );
		//balls[h].setPos(point4( startline, movetoy , xdistance, 1.0 ));
		xdistance -= (2*balls[h].rad + 0.1);
	}
}

void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}
/*********************************************************
    PROC: drawText()
    DOES: draws text, x,y position on an orthographic
     

**********************************************************/
void drawText(const char *text, int length, int x, int y){
 glMatrixMode(GL_PROJECTION); // change the current matrix to PROJECTION
 double *matrix = new double[16]; // allocate 16-byte in memory
 glGetDoublev(GL_PROJECTION_MATRIX, matrix); // get the values from PROJECTION matrix to local variable
 glLoadIdentity(); // reset PROJECTION matrix to identity matrix
 glOrtho(0, 800, 0, 600, -5, 5); // orthographic perspective
 glMatrixMode(GL_MODELVIEW); // change current matrix to MODELVIDE matrix again
 glLoadIdentity(); // reset it to identity matrix
 glPushMatrix(); // push current state of MODELVIEW matrix to stack
 glLoadIdentity(); // reset it again. (may not be required, but it my convention
 glRasterPos2i(x, y); // raster position in 2D
 for(int i=0; i<length; i++){
	 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)text[i]); // generation of characters in our text with 9 by 15 GLU font
 }
 glPopMatrix(); // get MODELVIEW matrix value from stack
 glMatrixMode(GL_PROJECTION); // change current matrix mode to PROJECTION
 glLoadMatrixd(matrix); // reset
 glMatrixMode(GL_MODELVIEW); // change current matrix mode to MODELVIEW
}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
	
	GLuint buffer;
	GLuint vPosition;
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

	/**************************************************************************/
	//circles for the pockets
	createCircle(circlePts, p);
	for(i = 0; i < 6; i++) pockets[i].radius = 0.9;

	glGenVertexArrays(1, &vao[1]);
	glBindVertexArray(vao[1]);
	glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(circlePts) ,circlePts, GL_STATIC_DRAW );
	
	vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	/************************************************************************************************/
	//this is to draw the line projection
	glGenVertexArrays( 1, &vao[0] );
	glGenBuffers( 1, &buffer );
	glBindVertexArray( vao[0] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(lineprojection), lineprojection, GL_STATIC_DRAW );
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	/**********************************************************************************************/
	//this is to draw the power bar
	glGenVertexArrays( 1, &vao[3] );
	glGenBuffers( 1, &buffer );
	glBindVertexArray( vao[3] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(powerbox), powerbox, GL_STATIC_DRAW );
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // Generate vertex arrays for geometric shapes
   // generateCube(program, &wall);
//    generateSphere(program, &sphereData);
//    generateCone(program, &coneData);
//    generateCylinder(program, &cylData);

//    generateHole(program, &holeData);
//    generateQuarterHole(program, &quarterHoleData);
//    generateTriPrism(program, &triPrismData);
	
//	generateEdgeCorner(program, &edgeCornerData);

	generateSphere( program, &sphereData );

	genWall(program, &wall);

	mDrawer.initDrawer(program);
	// all the ball's radius
	for(i=0; i < 16; i++) reset(&balls[i]);
	setballpos();
	//get the shader variables
    vars.uModelView  = glGetUniformLocation( program, "ModelView"  );
    vars.uProjection = glGetUniformLocation( program, "Projection" );
    vars.uView       = glGetUniformLocation( program, "View"       );

    

    vars.uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    vars.uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    vars.uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    vars.uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    vars.uShininess = glGetUniformLocation( program, "Shininess"       );
    vars.uTex       = glGetUniformLocation( program, "Tex"             );
    vars.uEnableTex = glGetUniformLocation( program, "EnableTex"       );

    glUniform4f(vars.uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(vars.uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(vars.uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(vars.uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(vars.uShininess, 100.0f);



	glClearColor( 0.1, 0.1, 0.2, 1.0 ); // dark blue background
    glEnable(GL_DEPTH_TEST);

    TgaImage tex[16];
	//load all images
	if (!tex[0].loadTGA("textures/Untitled.tga")) { printf("Error loading image file ball0.tga\n");/* exit(1);*/}
	if (!tex[1].loadTGA("textures/ball1.tga")) { printf("Error loading image file ball1.tga\n"); /*exit(1);*/}
	if (!tex[2].loadTGA("textures/ball2.tga")) { printf("Error loading image file ball2.tga\n"); /*exit(1);*/}
	if (!tex[3].loadTGA("textures/ball3.tga")) { printf("Error loading image file ball3.tga\n"); /*exit(1);*/}
	if (!tex[4].loadTGA("textures/ball4.tga")) { printf("Error loading image file ball4.tga\n"); /*exit(1);*/}
	if (!tex[5].loadTGA("textures/ball5.tga")) { printf("Error loading image file ball5.tga\n"); /*exit(1);*/}
	if (!tex[6].loadTGA("textures/ball6.tga")) { printf("Error loading image file ball6.tga\n"); /*exit(1);*/}
	if (!tex[7].loadTGA("textures/ball7.tga")) { printf("Error loading image file ball7.tga\n"); /*exit(1);*/}
	if (!tex[8].loadTGA("textures/ball8.tga")) { printf("Error loading image file ball8.tga\n"); /*exit(1);*/}
	if (!tex[9].loadTGA("textures/ball9.tga")) { printf("Error loading image file ball9.tga\n"); /*exit(1);*/}
	if (!tex[10].loadTGA("textures/ball10.tga")) { printf("Error loading image file ball10.tga\n"); /*exit(1);*/}
	if (!tex[11].loadTGA("textures/ball11.tga")) { printf("Error loading image file ball11.tga\n"); /*exit(1);*/}	
	if (!tex[12].loadTGA("textures/ball12.tga")) { printf("Error loading image file ball12.tga\n"); /*exit(1);*/}	
	if (!tex[13].loadTGA("textures/ball13.tga")) { printf("Error loading image file ball13.tga\n"); /*exit(1);*/}	
	if (!tex[14].loadTGA("textures/ball14.tga")) { printf("Error loading image file ball14.tga\n"); /*exit(1);*/}	
	if (!tex[15].loadTGA("textures/ball15.tga")) { printf("Error loading image fileball15.tga\n"); /*exit(1);*/}	
	


	
		
	//bind images to textures
	for(i = 0; i < 16;i++) {
		glGenTextures( 1, &ball_texture[i] );
		glBindTexture( GL_TEXTURE_2D, ball_texture[i] );
		glTexImage2D(GL_TEXTURE_2D, 0, 4, tex[i].width, tex[i].height, 0,
                 (tex[i].byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, tex[i].data );  
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		balls[i].texture = ball_texture[i];
	}


    TgaImage coolImage;
    if (!coolImage.loadTGA("textures/corr1.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
//
//    TgaImage earthImage;
//    if (!earthImage.loadTGA("earth.tga"))
//    {
//        printf("Error loading image file\n");
//        exit(1);
//    }
//
//
    glGenTextures( 1, &texture_cube );
	glBindTexture( GL_TEXTURE_2D, texture_cube );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, coolImage.data );
//    glGenerateMipmap(GL_TEXTURE_2D);
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//
//    glGenTextures( 1, &texture_earth );
//    glBindTexture( GL_TEXTURE_2D, texture_earth );
//    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
//                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
//                 GL_UNSIGNED_BYTE, earthImage.data );
//
//
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//    // Set texture sampler variable to texture unit 0
//    // (set in glActiveTexture(GL_TEXTURE0))
//
    glUniform1i( vars.uTex, 0);
    
    Arcball = new BallData ;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
	
}


/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --

        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/

void display(void)
{
    // Clear the screen with the background colour (set in myinit)
	proj = Perspective(30-zoom,(float)Width/(float)Height,0.01f, 1000.0f);
	glUniformMatrix4fv(vars.uProjection,1,GL_TRUE, proj);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::string text;
	std::string p1win="Player 1 Wins";
std::string p2win="Player 2 Wins";
 text = "Player 1";
 glUniform4fv( vars.uAmbient,1 ,   color4( 1, 0, 0, 1.0 ) );
 drawText(text.data(), text.size(), 50, 400);
 text= "Player 2";
  drawText(text.data(), text.size(), 650, 400);
 if(player1type!=0)
 {
	 std::string stripes="Stripes";
	 std::string colores="Colored";
	 if(player1type==1)
	 {
		 drawText(colores.data(), colores.size(), 50, 375);
		  drawText(stripes.data(), stripes.size(), 650, 375);
	 }
	 else{
		  drawText(stripes.data(), stripes.size(), 50, 375);
		  drawText(colores.data(), colores.size(), 650, 375);
	 }
 }
 if(win!=0)
 {
	 if(win==1)
		 drawText(p1win.data(), p1win.size(), 325, 375);
	 if(win==2)
		 drawText(p2win.data(), p2win.size(), 325, 375);
	 text="Press r to restart the game";
	 drawText(text.data(), text.size(), 325, 350);
 }
  if(turn)
  {
		text="Player 1 Turn"; 
  }
  else{
	  text="Player 2 Turn";
  }
  drawText(text.data(), text.size(), 325, 550);

	 
    model_view = mat4(1.0f);
    view = mat4(1.0f);
    view *= Translate(xpos, ypos, zpos) * RotateX(CAMERA_INIT_AZIMUTH);
    glUniformMatrix4fv(vars.uView, 1, GL_TRUE, view);

    model_view *= Translate(xpos, ypos, zpos);

	HMatrix r;
    Ball_Value(Arcball,r);

    mat4 mat_arcball_rot(
        r[0][0], r[0][1], r[0][2], r[0][3],
        r[1][0], r[1][1], r[1][2], r[1][3],
        r[2][0], r[2][1], r[2][2], r[2][3],
        r[3][0], r[3][1], r[3][2], r[3][3]);

    model_view *= mat_arcball_rot;
    glUniformMatrix4fv(vars.uView, 1, GL_TRUE, view);


    original_model_view = model_view*RotateX(56)*RotateY(90);

    set_colour(1.0f, 1.0f, 1.0f);
	//this draws all the balls in the xy plane
	for (j=0; j<16; j++) {
	
		model_view = original_model_view;
		
		model_view *= Translate(balls[j].pos.x, balls[j].pos.y, balls[j].pos.z);
		model_view *= Scale( balls[j].rad, balls[j].rad, balls[j].rad )*balls[j].rotateMat*RotateX(90);
		
		//this is called to draw the balls. lool at at sphere.cpp
		if(drawable[j])
			drawBall(&sphereData, &vars, model_view, balls[j].texture);
		else if (!drawable[j]){
			if(j!=0)
			{
			balls[j].pos.x=-15;
			balls[j].pos.y=-15;
			balls[j].velocity=vec3(0,0,0);
			balls[j].moving = false;
			}
			if(j==0)
			{
				balls[0].pos = point4(0.0,-0.8,0.0,1.0);
				drawable[j]=true;
				tempPockets[j]=true;
				drawBall(&sphereData, &vars, model_view, balls[j].texture);
			}
		}
	}
	
	//to be able to mvoe the camera up, down, left, right, in, out
//	view = Translate( xpos, ypos, zpos-9)*RotateY( azimuth)*RotateX(angle);
	
//    model_view = Translate(0.0f, 0.0f, -0.0f);
   
//    model_view *= mat_arcball_rot;

   // mat4 view = model_view;


    //model_view = Angel::LookAt(eye, ref, up);//just the view matrix;


    // Previously glScalef(Zoom, Zoom, Zoom);
//    model_view *= Scale(0.5)*RotateX(90)*RotateY(90);

//    original_model_view = model_view;


	
	mDrawer.setDrawerParam(original_model_view, vars, vars.uEnableTex);
   mDrawer.drawTable();
	//mDrawer.setDrawerParam(original_model_view, vars, vars.uEnableTex);

   /*******************************************************************************************/
   //draw the circles
   for (i = 0; i < 6; i++) {
	glBindVertexArray(vao[1]);
	model_view = original_model_view;
	drawCircle(&vars, &pockets[i], model_view);
   }

   /******************************************************************************************
   //the walls
  glUniform4f(vars.uAmbient,    0.20f,  0.0f,  0.0f, 1.0f);
    glUniform4f(vars.uDiffuse,    1.0f,  0.0f,  0.0f, 1.0f);
    glUniform4f(vars.uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
   // glUniform4f(vars.uLightPos,  15.0f, 15.0f, 0.0f, 0.0f);
   // glUniform1f(vars.uShininess, 100.0f);
   model_view = original_model_view;
   model_view *= Translate(60,0,0)*Scale(40,40,40)*RotateY(-90);

   glBindTexture(GL_TEXTURE_2D, texture_cube );
	glUniform1i(vars.uEnableTex, 1);
	glBindVertexArray(wall.vao);
   glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
   glDrawArrays( GL_TRIANGLES, 0, 6 );

   glUniform1i( vars.uEnableTex, 0 );


    model_view = original_model_view;
   model_view *= Translate(0,0,-60)*Scale(40,40,40)*RotateY(0);

   glBindTexture(GL_TEXTURE_2D, texture_cube );
	glUniform1i(vars.uEnableTex, 1);
	glBindVertexArray(wall.vao);
   glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
   glDrawArrays( GL_TRIANGLES, 0, 6 );

   glUniform1i( vars.uEnableTex, 0 );



   
    model_view = original_model_view;
   model_view *= Translate(0,0,60)*Scale(40,40,40)*RotateY(180);

   glBindTexture(GL_TEXTURE_2D, texture_cube );
	glUniform1i(vars.uEnableTex, 1);
	glBindVertexArray(wall.vao);
   glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
   glDrawArrays( GL_TRIANGLES, 0, 6 );

   glUniform1i( vars.uEnableTex, 0 );

   model_view = original_model_view;
   model_view *= Translate(-60,0,0)*Scale(60,60,60)*RotateY(90);

   glBindTexture(GL_TEXTURE_2D, texture_cube );
	glUniform1i(vars.uEnableTex, 1);
	glBindVertexArray(wall.vao);
   glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
   glDrawArrays( GL_TRIANGLES, 0, 6 );

   glUniform1i( vars.uEnableTex, 0 );

   /*******************************************************************************/
   //this is to draw the projection line
	glUniform4fv( vars.uAmbient,1 ,   color4( 1, 1, 1, 1.0 ) );
	glUniform4fv( vars.uDiffuse,  1,  color4( 0.6, 0.6, 0.6, 1.0 ) );
	glUniform4fv( vars.uSpecular,  1, color4( 0.2, 0.2, 0.2, 1.0 ) );
	
	//the projection line
	if(!animate) {

		model_view = original_model_view;//Translate(0,0,0);//*RotateY(projangle);
		model_view *= Translate(balls[0].pos.x, balls[0].pos.y, balls[0].pos.z)*RotateY(projangle);
		glBindVertexArray( vao[0] );

		//glUniform1i( vars.uEnableTex, 0);
		glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
		//glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
		//glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj );
		
		glDrawArrays( GL_LINES, 0, 2 );
	}


	glUniform4fv( vars.uAmbient,1 ,   color4( 0.2, 0.2, 0.2, 1.0 ) );
	glUniform4fv( vars.uDiffuse,  1,  color4( 0.6, 0.6, 0.6, 1.0 ) );
	glUniform4fv( vars.uSpecular,  1, color4( 0.2, 0.2, 0.2, 1.0 ) );
    
	
	/******************************************************************************************************/
	//draws the power bar
	//orthogonal projection
	proj = Ortho(-1.0f, 1.0f, -1.0f, 1.0, 0.05f, 1000.0f);
	model_view = Translate(0,0.03,-5);
	glBindVertexArray(vao[3]);
	glUniform4fv( vars.uAmbient,1 ,   color4( 0.8, 0.8, 0, 0.50f ) );
	glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
	glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj);
	//glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
	if(SpacePressed)
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	
	//new box - same vertex array
	glBindVertexArray(vao[3]);
	glUniform4fv( vars.uAmbient,1 ,   color4( 0.8, 0.0f, 0.0f, 0.50f ) );
	//view=Translate(0,0,-0.3);
	
	model_view= Translate(0,0,0.02)*model_view*Scale(power/POWER_LIMIT,1,1);
	
	glUniformMatrix4fv( vars.uModelView, 1, GL_TRUE, model_view );
	//glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
	glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj);
	if(SpacePressed)
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height) ;
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(30.0f-zoom, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, projection );
}

void instructions() 
{
    printf("Press:\n");
   
    printf("  r to restart game.\n") ;
    printf("  w,s to move camera up and down.\n") ;
    printf("  a,d to mvoe camera left and right.\n") ;
    printf("  left and right directional arrows to control angle of ball to hit.\n") ;
	printf("up and down directional arrows to move camera forward and back ");
	printf("press and hold spacebar to power up hitting of the ball");
	printf("1 to see a winning scenario");
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
	bool turnOver=false;
	TIME = TM.GetElapsedTime();
	
	if(animate) {
	
		diff = TIME-LAST_TIME;

	//	printf("Start detecting ball collision\n");
		//check all balls for collision against each other
		for(j = 0 ; j < 16; j++) {

			for ( i = 0; i < 16; i++) {
				for(k = i+1; k < 16; k++) {
					//to check for ball collision
					doBallCollision(&balls[i], &balls[k], diff);
					//if(willBallsCollide(&balls[i], &balls[k]))  doBallCollision(&balls[i], &balls[k], diff);
					//check that both balls are not falling
					//if(!balls[i].isFalling() && !balls[k].isFalling()) {
					
						//if(willBallsCollide(&balls[i], &balls[k])) { doBallCollision(&balls[i], &balls[k], diff);}
					//}
					//if(ballCollision(&balls[i], &balls[k], diff)) {printf("balls collided: % i %i \n", i, k);}
				
				}
			}
		}
		
		
		for(k = 0 ; k < 16; k++) {
			//if the ball is falling apply gravity
			if(balls[k].falling) {
				applyGravity(&balls[k], diff);
			}
			for( i = 0; i < 6; i++) {
				if(drawable[k])
					drawable[k]=ballFallsInPocket(&balls[k], &pockets[i]);
			}
			if(tempPockets[k] && !drawable[k])
					testPocket=false;
			
			
			if (tempPockets[k] && !drawable[k] && firstballin)
			{
				tempPockets[k]=false;
				balls[k].moving=false;
				if(turn && k==8)
				{
					
					if(player1type==1)//colored
					{
						for(int i=1;i<7;i++)
						{
							if(drawable[i]==true)
								win=2;
							printf("color %i",win);
							break;
						}
						win=1;
					}
					if(player1type==2)//striped
					{
						for(int i=9;i<16;i++)
						{
							if(drawable[i]==true)
								win=2;
							break;
						}
						win=1;
					}
				}
				else if (!turn && k==8)
				{
					if(player2type==1)//colored
					{
						for(int i=1;i<7;i++)
						{
							if(drawable[i]==true)
								win=1;

						}
						win=2;
					}
					if(player2type==2)//striped
					{
						for(int i=9;i<16;i++)
						{
							if(drawable[i]==true)
								win=1;
							
						}
						win=2;
					}
				}
				if(turn && player1type==1 && k>8)//player 1 colored and he hits a striped ball
				{
					
					turnOver=true;
					testPocket=true;

					break;
				}
				else if(turn && player1type==2 && (k<8 && k>0))//player 1 striped hits a colored
				{
					turnOver=true;
					testPocket=true;

					break;
				}
				else if(!turn && player2type==1 && k>8)//player 2 colored -hits striped in
				{
					turnOver=true;
					testPocket=true;

					break;
				}
				else if(!turn && player2type==2 && (k<8 && k>0))//player 2 striped hits colored in
				{
					turnOver=true;
					testPocket=true;

					break;
				}
				else if(k==0)
				{
					turnOver=true;
					testPocket=true;

					break;
				}
				else{
					testPocket=false;

					break;
				}
				break;
			}
			//new collisions that is the firstballin
			if(tempPockets[k] && !drawable[k] && !firstballin)
			{
				balls[k].moving=false;
				if(k!=0)//first ball can no longer be set
					firstballin=true;
				if((k<8 && k>0) && turn)//player 1 now colored
				{
					player1type=1;
					player2type=2;
				}
				if (k>8 && turn){//player 1 striped
					player1type=2;
					player2type=1;
				}
				if (k>8 && !turn)//player 2 striped
				{
					player1type=1;
					player2type=2;
				}
				if ((k>0 && k<8) && !turn)//[player 2 colored
				{
					player1type=2;
					player2type=1;
				}
				tempPockets[k]=false;
				testPocket=false;
				
			}
			if(!drawable[k])
				balls[k].moving=false;
			//only check collision with wall and move ball only if ball has not fallen into pocket
			if(!balls[k].falling) {
				wallCollision(&balls[k]);
				float angular_rot = balls[k].angular_rot; 
				angular_rot = diff*sqrt(pow(balls[k].velocity.x,2)+ pow(balls[k].velocity.z,2))/(balls[k].rad);
				balls[k].angular_rot = angular_rot;
	
				GLfloat angle_rot = diff*sqrt(pow(balls[k].velocity.x,2)+ pow(balls[k].velocity.z,2))/(balls[k].rad);
				if(angle_rot > 0)
					balls[k].rotateMat =Quaternions(-balls[k].velocity.z, 0, balls[k].velocity.x, -angle_rot) * balls[k].rotateMat;

				moveBall(&balls[k],diff);
			}
		turnOver=true;
		if(!testPocket)//if the testPocket is false than the turn continues
			turnOver=false;
		animate = false;
		for(i=0; i < 16; i++) {
			if(balls[i].moving) animate = true;
		}
		
		//if(!animate) {std::cout << "diff" << diff <<std::endl; TM.Reset();}
	
		
	
		}
	}
		if(testPocket && turnOver && !animate)
		{
			if(turn)
				turn=false;
			else
				turn=true;
			turnOver=false;
		}
	
		if(!animate)
		{
			turnOver=false;
		testPocket=true;
		}
	glutPostRedisplay() ;
#ifdef __linux__
	else if(keyReleaseTime > 0 && TIME - keyReleaseTime > 0.05){
		//balls[0].velocity = vec3( power*sin(-projangle*M_PI/180),0, power*cos(-projangle*M_PI/180+90) );
		balls[0].velocity = vec3( power*cos(-projangle*M_PI/180),0 ,power*sin(-projangle*M_PI/180) );
		//balls[0].setVel(v);
		//friction is always in the direction of velocity
		balls[0].friction = FRICTION_CONST*balls[0].velocity;
		balls[0].moving = true;
		//balls[0].setMoving(true);


		animate = true;
		power=0;
		//checkDirectionOfVelocity(&balls[0]);
		glutPostRedisplay();
	}
#endif

	LAST_TIME = TIME;

}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
//
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0);
		case 'w': //move camera up
			ypos -=0.5;
			break;
		case 's': //move camera down
			ypos +=0.5;
			break;
		case 'a': //move to left
			xpos +=0.5;
			break;
		case '1':
			resetArcball() ;
			for( i = 1; i < 8 ; i++) {
				
					drawable[i]=false;
					tempPockets[i]=false;
				
			}
			testPocket=true;
			firstballin=true;
			turn=true;
			numBallsIn = 0;
			player1type=1;
			player2type=2;
			animate =false;
		balls[0].pos = point4(0,1.0f - TABLE_DEPTH,2.0,1.0);
		drawable[8]=true;
					tempPockets[8]=true;
		balls[8].pos = point4(0,1.0f - TABLE_DEPTH,0.0,1.0);
			//setballpos();
			projangle = 90;
			xpos = CAMERA_INIT_X, ypos = CAMERA_INIT_Y, zpos = CAMERA_INIT_Z, zoom = 0;
            glutPostRedisplay() ;
			break;
		case 'd': //move to right
			xpos -=0.5;
			break;
       /* case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;*/
        case 'r':
			//reset everthing
            resetArcball() ;
			for( i = 0; i < 16 ; i++) {
				reset(&balls[i]);
				drawable[i]=true;
				tempPockets[i]=true;
			}
			player1type=0;

			testPocket=true;
			firstballin=false;
			turn=true;
			win=0;
			player2type=0;
			numBallsIn = 0;
			animate =false;
			setballpos();
			projangle = 0;
			xpos = CAMERA_INIT_X, ypos = CAMERA_INIT_Y, zpos = CAMERA_INIT_Z, zoom = 0;
            glutPostRedisplay() ;
            break ;
       /* case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;*/
		case ' ':
			//charge up power for hitting ball
#ifdef __linux__
			keyPressTime = TM.GetElapsedTime();
#endif
			if(!animate) {
#ifdef __linux__
				if(keyPressTime - keyReleaseTime < 0.005) {
#endif
					SpacePressed=true;
					power +=0.5;
					if(power>=POWER_LIMIT) {
						power = POWER_LIMIT;
					}
#ifdef __linux__
				}
#endif
			}
		break;
		/*case 'b':
			for(i=0; i < 16; i++) {
				balls[i].velocity = vec3(0,0,0); 
			}
			animate = false;
			break;
        /*case '0':
            //reset your object
            break ;
        /*case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();*/
            break;
    }
    glutPostRedisplay() ;

}

void dkeys(int key, int x, int y) {
	//move forward
	if(key == GLUT_KEY_UP) {
		zoom +=2.5;
	}
	//move backward
	if(key == GLUT_KEY_DOWN) {
		zoom-=2.5;
	}
	//rotate to left
	if(key == GLUT_KEY_LEFT) {
		if(!animate) {
			projangle += 0.85;	
		}
	}
	//rotate to right
	if (key==GLUT_KEY_RIGHT) {
		if(!animate) {
			projangle -= 0.85;
		}
	}
	
	glutPostRedisplay();
}

void keyBoardUp( unsigned char key, int x, int y )
{	
switch( key ) {
case ' ':
	if(!animate) {
#ifndef __linux__
		//balls[0].velocity = vec3( power*sin(-projangle*M_PI/180),0, power*cos(-projangle*M_PI/180+90) ); 
		balls[0].velocity = vec3( power*cos(-projangle*M_PI/180), 0,power*sin(-projangle*M_PI/180) ); 
		//friction is always in the direction of velocity
		balls[0].friction = FRICTION_CONST*balls[0].velocity; 
		
		balls[0].moving =  true;
	
		SpacePressed=false;
		animate = true;
		power=0;
		//checkDirectionOfVelocity(&balls[0]);
		glutPostRedisplay();
#endif

#ifdef __linux
		keyReleaseTime = TM.GetElapsedTime();

#endif
	}
break;
}	
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn

**********************************************************/

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
	glutSpecialFunc(dkeys);
	glutKeyboardUpFunc(keyBoardUp);

    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}
