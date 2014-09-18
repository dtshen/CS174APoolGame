////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#include "Angel.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "Shapes.h"
#include "tga.h"
#include "sphere.h"
#include "Timer.h"
#include <iostream>
using namespace std;
Timer TM;
double TIME = 0.0 ;
double LAST_TIME = 0.0;
//size of window
int Width = 512;
int Height = 512 ;
int i=0;
//to print instruction
void instructions();

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//texture

GLuint texture_cube;
GLuint ball_texture[16];

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphere;
sphereData balls[16];

//sphereData
mat4   model_view, view, proj;

shaderVariables vars;
shaderVariables linevar;







/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/
point4 light_position = point4(0,0,10.0f,10.0f);
float projangle = 0;
float rotatetable = 0;
bool animate = false;
float angularrotation = 0;
bool speedbaranimate = false;
//point4 speedbar[] = {point4()};
//this is to draw the line where we are shooting to
point4 lineprojection[2] = {point4(0,0,0,1), point4(0,1,0,1)}; 
// this draws a simple surface
point4 surfacetable[4] = {point4( 1, 2, -0.15, 1 ), point4( 1, -2, -0.15, 1 ), point4( -1, 2, -0.15, 1 ), point4( -1, -2, -0.15, 1 )};
//this draws a box
point4 box[8] = {point4(1,2,0,1),point4(-1,2,0,1),point4(1,2,0,1), point4(1,-2,0,1),
		point4(1,-2,0,1), point4(-1,-2,0,1), point4(-1,-2,0,1),  point4(-1,2,0,1)};
//keeps track of vertex arrays
GLuint vao[3];

void setballpos() {
	float startline = 0;
	balls[0].currpos = point4(0.0,-0.8,0.0,1.0);
	int h = 0;
	float xdistance = 0.42;
	for(h = 1; h <=5; h++ ) {
		balls[h].currpos = point4(xdistance,startline,0.0 , 1.0);
		xdistance -= (2*balls[h].radius+0.02);
	}
	startline += 0.19;
	xdistance = 0.31;
	for(h = 6; h<=9;h++) {
		balls[h].currpos = point4(xdistance,startline,0.0 , 1.0);
		xdistance -= (2*balls[h].radius+0.02);
	}
	startline += 0.19;
	xdistance =0.21;
	for (h=10; h <=12;h++) {
		balls[h].currpos = point4(xdistance,startline,0.0 , 1.0);
		xdistance -= (2*balls[h].radius+0.02);
	}
	startline += 0.19;
	xdistance = 0.11;
	for (h = 13; h <= 14; h++) {
		balls[h].currpos = point4( xdistance, startline, 0.0 , 1.0 );
		xdistance -= ( 2*balls[h].radius+0.02 );
	}
	startline+=0.19;
	balls[15].currpos = point4( 0.0, startline, 0.0,1.0 );
}
void myinit(void) {
	GLuint buffer;
	GLuint vPosition;
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

	/***********************************************************/
	//the projection line
	glGenVertexArrays( 1, &vao[0] );
	glGenBuffers( 1, &buffer );
	glBindVertexArray( vao[0] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(lineprojection), lineprojection, GL_STATIC_DRAW );
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	/***************************************************************/
	/**********************************************************************************************************/
	//the box
	glGenVertexArrays( 1, &vao[1] );
	glGenBuffers( 1, &buffer );
	glBindVertexArray( vao[1] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW );
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	/*********************************************************************************************************/
	glGenVertexArrays( 1, &vao[2] );
	glGenBuffers( 1, &buffer );
	glBindVertexArray( vao[2] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(surfacetable), surfacetable, GL_STATIC_DRAW );
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	/***********************************************************************************************************/
    generateSphere( program, &sphere );

	for(i = 0; i < 16; i++) {
		balls[i].vao = sphere.vao;
		balls[i].numvertices = sphere.numVertices;
		balls[i].center = point4( 0.0, 0.0, 0.0, 1.0 );
		balls[i].radius = 0.10;
		balls[i].velocity = vec3( 0.0, 0.0, 0.0 );
		balls[i].friction = vec3( 0.01, 0.01, 0.0 );
		balls[i].moving = false;
		
	}
	//model, view projection matrices
    vars.uModel      = glGetUniformLocation( program, "ModelView"  );
    vars.uProjection = glGetUniformLocation( program, "Projection" );
    vars.uView       = glGetUniformLocation( program, "View"       );


	//texture and shading
    vars.uAmbient        = glGetUniformLocation( program, "AmbientProduct"  );
    vars.uDiffuse        = glGetUniformLocation( program, "DiffuseProduct"  );
    vars.uSpecular       = glGetUniformLocation( program, "SpecularProduct" );
    vars.uLightPosition  = glGetUniformLocation( program, "LightPosition"   );
    vars.uShininess      = glGetUniformLocation( program, "Shininess"       );
    vars.uTex            = glGetUniformLocation( program, "Tex"             );
    vars.uEnableTex      = glGetUniformLocation( program, "EnableTex"       );

    

	setballpos();

    glEnable(GL_DEPTH_TEST);

    TgaImage balltex[16];
	
	if (!balltex[0].loadTGA("Untitled.tga")) { printf("Error loading image file ball0.tga\n");/* exit(1);*/}
	if (!balltex[1].loadTGA("ball1.tga")) { printf("Error loading image file ball1.tga\n"); /*exit(1);*/}
	if (!balltex[2].loadTGA("ball2.tga")) { printf("Error loading image file ball2.tga\n"); /*exit(1);*/}
	if (!balltex[3].loadTGA("ball3.tga")) { printf("Error loading image file ball3.tga\n"); /*exit(1);*/}
	if (!balltex[4].loadTGA("ball4.tga")) { printf("Error loading image file ball4.tga\n"); /*exit(1);*/}
	if (!balltex[5].loadTGA("ball5.tga")) { printf("Error loading image file ball5.tga\n"); /*exit(1);*/}
	if (!balltex[6].loadTGA("ball6.tga")) { printf("Error loading image file ball6.tga\n"); /*exit(1);*/}
	if (!balltex[7].loadTGA("ball7.tga")) { printf("Error loading image file ball7.tga\n"); /*exit(1);*/}
	if (!balltex[8].loadTGA("ball8.tga")) { printf("Error loading image file ball8.tga\n"); /*exit(1);*/}
	if (!balltex[9].loadTGA("ball9.tga")) { printf("Error loading image file ball9.tga\n"); /*exit(1);*/}
	if (!balltex[10].loadTGA("ball10.tga")) { printf("Error loading image file ball10.tga\n"); /*exit(1);*/}
	if (!balltex[11].loadTGA("ball11.tga")) { printf("Error loading image file ball11.tga\n"); /*exit(1);*/}	
	if (!balltex[12].loadTGA("ball12.tga")) { printf("Error loading image file ball12.tga\n"); /*exit(1);*/}	
	if (!balltex[13].loadTGA("ball13.tga")) { printf("Error loading image file ball13.tga\n"); /*exit(1);*/}	
	if (!balltex[14].loadTGA("ball14.tga")) { printf("Error loading image file ball14.tga\n"); /*exit(1);*/}	
	if (!balltex[15].loadTGA("ball15.tga")) { printf("Error loading image fileball15.tga\n"); /*exit(1);*/}	
	
   
	for(i = 0; i < 16;i++) {
		glGenTextures( 1, &ball_texture[i] );
		glBindTexture( GL_TEXTURE_2D, ball_texture[i] );
		glTexImage2D(GL_TEXTURE_2D, 0, 4, balltex[i].width, balltex[i].height, 0,
                 (balltex[i].byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, balltex[i].data );  
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
    
    
	 /***************************************************************************************/
  
    glUniform1i( vars.uTex, 0);
    glClearColor( 0, 0, 0, 1.0 ); // dark blue background
	
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
float angle = 0;
float thetax = 0;
float thetay = 0;
float thetaz = 0;
float azimuth = 0;
float xpos = 0, ypos = 0, zpos = 0, zoom = 0;
float angletracker = 0;
int j = 0;
void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	proj = Perspective( 30-zoom, (float)Width/(float)Height, 1.0f,1000.0f);
	
	glUniformMatrix4fv(vars.uProjection, 1, GL_TRUE, proj);
	//rotate  set th/
	/*
	if ( balls[0].currpos.y>0.5 && !animate ) { rotatetable=180; }
	else if ( balls[0].currpos.y<0.5 && !animate ) { rotatetable=0; }
	*/
	glUniform4fv( vars.uAmbient,1 ,   color4( 0.2, 0.2, 0.2, 1.0 ) );
	glUniform4fv( vars.uDiffuse,  1,  color4( 0.6, 0.6, 0.6, 1.0 ) );
	glUniform4fv( vars.uSpecular,  1, color4( 0.2, 0.2, 0.2, 1.0 ) );
    glUniform4fv( vars.uLightPosition, 1,  light_position );
    glUniform1f( vars.uShininess, 100.0f );

	view = Translate( xpos-balls[0].currpos.x, ypos - sin(balls[0].currpos.y), zpos-3-cos(balls[0].currpos.y) )*RotateY( azimuth )*RotateZ( rotatetable )*RotateX(0);
	for (j=1; j<16; j++) {
		//the rotation along z-axis is the direction on which it is going
		//the rotation along x-axis is how fast it should rotate
		model_view = Translate(balls[j].currpos.x, balls[j].currpos.y, 0)*RotateX(balls[j].angular_rot)*RotateZ(balls[j].vangle)*Scale( balls[j].radius, balls[j].radius, balls[j].radius );
	//	model_view = Translate(balls[j].move.x, balls[j].move.y, balls[j].move.z)*RotateZ(-90)*RotateX(-angle)*Scale( balls[j].radius, balls[j].radius, balls[j].radius );
   
		drawBall(&balls[j], &vars, model_view, view, ball_texture[j]);
	}
	
	model_view = Translate(balls[0].currpos.x, balls[0].currpos.y, 0)*RotateZ(thetaz)*RotateX(thetax)*RotateY(thetay)*Scale( balls[0].radius, balls[0].radius, balls[0].radius );
	
		drawBall(&balls[0], &vars, model_view, view, ball_texture[1]);
	/*******************************************************/
	 glUniform4fv( vars.uAmbient,1 ,   color4( 1, 1, 1, 1.0 ) );
	glUniform4fv( vars.uDiffuse,  1,  color4( 0.6, 0.6, 0.6, 1.0 ) );
	glUniform4fv( vars.uSpecular,  1, color4( 0.2, 0.2, 0.2, 1.0 ) );
	//the projection line
	if(!animate) {

		model_view = Translate(balls[0].currpos.x,balls[0].currpos.y,0)*RotateZ(projangle);
	 
		glBindVertexArray( vao[0] );

		glUniform1i( linevar.uEnableTex, 0);
		glUniformMatrix4fv( vars.uModel, 1, GL_TRUE, model_view );
		glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
		glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj );
		
		glDrawArrays( GL_LINES, 0, 2 );
	}
   /***********************************************************************/
	//box lines
   model_view = Translate(0,0,balls[0].radius/2);
	glBindVertexArray(vao[1]);
	glUniformMatrix4fv( vars.uModel, 1, GL_TRUE, model_view );
	glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
	glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj );

	glDrawArrays( GL_LINES, 0, 8 );


	glUniform4fv( vars.uAmbient,1 ,   color4( 0, 0.6, 0, 1.0 ) );
	glUniform4fv( vars.uDiffuse,  1,  color4( 0.6, 0.6, 0.6, 1.0 ) );
	glUniform4fv( vars.uSpecular,  1, color4( 0.2, 0.2, 0.2, 1.0 ) );

	glBindVertexArray(vao[2]);
	glUniformMatrix4fv( vars.uModel, 1, GL_TRUE, model_view );
	glUniformMatrix4fv( vars.uView, 1, GL_TRUE, view );
	glUniformMatrix4fv( vars.uProjection, 1, GL_TRUE, proj );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glutSwapBuffers();

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
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

//keyboard functions for changing camera coordinates
void keyboard(unsigned char key, int x, int y) {
	if( key =='q' || key=='Q') exit(0);
	if( key =='r') {
		for(i=0; i < 16;i++) {
			balls[i].velocity = vec3(0,0,0);
			balls[i].moving = false;
		}
		setballpos();
		azimuth=0;xpos=0,ypos=0,zpos=0;zoom=0;
	}
	if(key == '1') {angle=0; azimuth=0;}
	//move up
	if(key=='w') {
		ypos-=0.05;
	}
	//move down
	else if(key=='s') {
		ypos +=0.05;
	}
	//move left
	if(key =='a') {
		xpos +=0.09;
	}
	//move right
	else if(key =='d') {
		xpos -=0.09;
	}
	//to change the field of view
	if(key =='n') {
		zoom+=0.5;
	}
	else if(key =='m') {
		zoom -=0.5;
	}
	
	if( key == 'z' && !animate) {projangle += 5;}
	if( key == 'x' && !animate) {projangle -= 5;}
	if(key =='b') {
		animate = false;
		for(i = 0; i < 16; i++) {
			balls[i].velocity = vec3(0,0,0);  
		}
		projangle= 0;
	//	if(balls[0].currpos.y>0.5) projangle=180;
	

		animate = false;
	}
	if(key == ' ') {
		float constant = 0.5;
		
		//balls[0].velocity = vec3(-0.09,-0.09,0);
		//balls[0].velocity = vec3( constant*sin(-projangle*M_PI/180),constant*cos(-projangle*M_PI/180), 0 ); 
		balls[0].velocity = vec3( constant*cos(((projangle+90)*M_PI/180)),constant*sin(((projangle+90)*M_PI/180)), 0 ); 
		//friction is always in the direction of velocity
		balls[0].friction.x = 0.009*balls[0].velocity.x;
		balls[0].friction.y = 0.009*balls[0].velocity.y;
		balls[0].moving = true;
		animate = true;
	}
	
 	glutPostRedisplay();
}

void dkeys(int key, int x, int y) {
	//move forward
	if(key == GLUT_KEY_UP) {
		zpos +=0.05;
	}
	//move backward
	if(key == GLUT_KEY_DOWN) {
		zpos-=0.05;
	}
	//rotate to left
	if(key == GLUT_KEY_LEFT) {
		azimuth += 0.57;
	}
	//rotate to right
	if (key==GLUT_KEY_RIGHT) {
		azimuth-=0.57;	
	}
	
	glutPostRedisplay();
}

int k=0;

void idleCB(void)
{
	
	//thetax  -= 1; 
	TIME = TM.GetElapsedTime();
	float diff = TIME-LAST_TIME;
	if(animate) {
		
		
		

		//check if balls have collided
		

		
		//check all balls for collision against each other
		/*
		for ( i = 0; i < 16; i++) {
			for(k = i+1; k < 16; k++) {
					//if(k!=i) {
						ballCollision( &balls[i], &balls[k] );
					//}
		
			}
		}*/
		
//		ballCollision(&balls[0], & balls[1], collided[0], collided[1]);
		for(int k = 0 ; k < 1; k++) { 
			//check for wall collision
			wallCollision(&balls[k]);
			//if balls did not collide we move them based on the direction they are going
				//decrease velocity with friction
				
			/*balls[k].angular_rot -=sqrt(pow(balls[k].velocity.x,2)+pow(balls[k].velocity.y,2))/(balls[k].radius);
			if(balls[k].velocity.x != 0 || balls[k].velocity.y !=0 ) {
					balls[k].vangle = atan(balls[k].velocity.y/balls[k].velocity.x);
				}
			else if(balls[k].velocity.x == 0) {balls[k].vangle = 0;}
			else if(balls[k].velocity.y == 0) {balls[k].vangle = 90;}
			*/
			moveBall(&balls[k], diff);
				
				
		}
		animate = false;
		for(i=0; i < 16; i++) {
			if(balls[i].moving) animate = true;
		}
		if(!animate) {projangle= 0; angletracker+=balls[0].vangle;} 

		
		
	}
	LAST_TIME = TIME;
	glutPostRedisplay();
    
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
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
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
	glutKeyboardFunc( keyboard );
	glutSpecialFunc(dkeys);
    //glutMouseFunc(myMouseCB) ;
   // glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

  //  TM.Reset() ;
    return 0;         // never reached
}




