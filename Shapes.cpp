#include "Angel/Angel.h"
#include "Shapes.h"
#include <cmath>

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;


void addTriangle(point4* destPoints, point3* destNormal, point3& p1, point3& p2, point3& p3, int& Index)
{
	point3 crs = cross(p2-p1, p3-p2);
	destPoints[Index] = p1; destNormal[Index] = crs; Index++;
	destPoints[Index] = p2; destNormal[Index] = crs; Index++;
	destPoints[Index] = p3; destNormal[Index] = crs; Index++;
}

void setVertexAttrib(GLuint program, 
                     GLfloat* points,    GLsizeiptr psize, 
                     GLfloat* normals,   GLsizeiptr nsize,
                     GLfloat* texcoords, GLsizeiptr tsize)
{
    GLuint buffer[2];
    glGenBuffers( 2, buffer );

    glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, psize, points, GL_STATIC_DRAW );
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
    glBufferData( GL_ARRAY_BUFFER, nsize, normals, GL_STATIC_DRAW );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    if (texcoords)
    {
        GLuint tbuffer;
        glGenBuffers( 1, &tbuffer );
        glBindBuffer( GL_ARRAY_BUFFER, tbuffer );
        glBufferData( GL_ARRAY_BUFFER, tsize, texcoords, GL_STATIC_DRAW );
        GLuint vTexCoords = glGetAttribLocation( program, "vTexCoords" );
        glEnableVertexAttribArray( vTexCoords );
        glVertexAttribPointer( vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------
// Cube

const int numCubeVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 cubePoints [numCubeVertices];
point3 cubeNormals[numCubeVertices];
point2 cubeUV     [numCubeVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// quad generates two triangles for each face and assigns normals and texture coordinates
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d, const point3& normal )
{
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal; 
    cubeUV[Index] = point2(0.0f, 1.0f); Index++;
    cubePoints[Index] = vertices[b]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(0.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(0.0f, 1.0f); Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[d]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 1.0f); Index++;
}

// generate 12 triangles: 36 vertices, 36 normals, 36 texture coordinates
void colorcube()
{
    quad( 1, 0, 3, 2, point3( 0.0f,  0.0f,  1.0f) );
    quad( 2, 3, 7, 6, point3( 1.0f,  0.0f,  0.0f) );
    quad( 3, 0, 4, 7, point3( 0.0f, -1.0f,  0.0f) );
    quad( 6, 5, 1, 2, point3( 0.0f,  1.0f,  0.0f) );
    quad( 4, 5, 6, 7, point3( 0.0f,  0.0f, -1.0f) );
    quad( 5, 4, 0, 1, point3(-1.0f,  0.0f,  0.0f) );
}

// initialization
void generateCube(GLuint program, ShapeData* cubeData)
{
    colorcube();
    cubeData->numVertices = numCubeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &cubeData->vao );
    glBindVertexArray( cubeData->vao );

    // Set vertex attributes
    setVertexAttrib(program, 
        (float*)cubePoints,  sizeof(cubePoints), 
        (float*)cubeNormals, sizeof(cubeNormals),0,0);
//        (float*)cubeUV,      sizeof(cubeUV));
}


//----------------------------------------------------------------------------
// Sphere approximation by recursive subdivision of a tetrahedron

const int N = 4;  // number of subdivisions
const int numSphereVertices = 1024*3;//16*256*3;  // number of resulting points

point4 spherePoints [numSphereVertices];
point3 sphereNormals[numSphereVertices];
point2 sphereUVs[numSphereVertices];

// four equally spaced points on the unit circle

point4 v[4] = {
    vec4(0.0, 0.0, 1.0, 1.0), 
    vec4(0.0, 0.942809, -0.333333, 1.0),
    vec4(-0.816497, -0.471405, -0.333333, 1.0),
    vec4(0.816497, -0.471405, -0.333333, 1.0)
};

static int k = 0;

// move a point to unit circle

point4 unit(const point4 &p)
{
    point4 c;
    double d=0.0;
    for(int i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0) for(int i=0; i<3; i++) c[i] = p[i]/d;
    c[3] = 1.0;
    return c;
}

void triangle(point4 a, point4 b, point4 c)
{
    spherePoints[k] = a;
    k++;
    spherePoints[k] = b;
    k++;
    spherePoints[k] = c;
    k++;
}

void divide_triangle(point4 a, point4 b, point4 c, int n)
{
    point4 v1, v2, v3;
    if(n>0)
    {
        v1 = unit(a + b);
        v2 = unit(a + c);
        v3 = unit(b + c);   
        divide_triangle(a , v2, v1, n-1);
        divide_triangle(c , v3, v2, n-1);
        divide_triangle(b , v1, v3, n-1);
        divide_triangle(v1, v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void tetrahedron(int n)
{
    divide_triangle(v[0], v[1], v[2], n);
    divide_triangle(v[3], v[2], v[1], n);
    divide_triangle(v[0], v[3], v[1], n);
    divide_triangle(v[0], v[3], v[2], n);
}

// initialization

void generateSphere(GLuint program, ShapeData* sphereData)
{
    tetrahedron(N);

    sphereData->numVertices = numSphereVertices;

    // Normals
    for (int i = 0; i < numSphereVertices; i++)
    {
        sphereNormals[i] = point3(spherePoints[i].x, spherePoints[i].y, spherePoints[i].z);
    }
    
    // TexCoords
    double u, v;
    for (int i = 0; i < numSphereVertices; i++)
    {
        v = 0.5 - asin(spherePoints[i].y)/M_PI ; //0~1
        u = 0.5*(atan2(spherePoints[i].z,spherePoints[i].x)/M_PI + 1); //0~1
        sphereUVs[i] = point2(u,v);
    }

    // Create a vertex array object
    glGenVertexArrays( 1, &sphereData->vao );
    glBindVertexArray( sphereData->vao );

    // Set vertex attributes
    setVertexAttrib(program,
        (float*)spherePoints,  sizeof(spherePoints),
        (float*)sphereNormals, sizeof(sphereNormals),
        (float*)sphereUVs, sizeof(sphereUVs));
}

//----------------------------------------------------------------------------
// Cone

const int numConeDivisions = 32;
const int numConeVertices = numConeDivisions * 6;

point4 conePoints [numConeVertices];
point3 coneNormals[numConeVertices];

point2 circlePoints[numConeDivisions];

void makeCircle(point2* dest, int numDivisions)
{
    for (int i = 0; i < numDivisions; i++)
    {
        float a = i * 2.0f * M_PI / numDivisions;
        dest[i] = point2(cosf(a), sinf(a));
    }
}

void makeHalfCircle(point2* dest, int numDivisions)
{
	for( int i = 0; i <= numDivisions; i++)
	{
		float a = i * M_PI / numDivisions;
		dest[i] = point2(cosf(a), sinf(a));
	}
}

void makeConeWall(point4* destp, point3* destn, int numDivisions, float z1, float z2, int& Index, int dir)
{
    for (int i = 0; i < numDivisions; i++)
    {
        point3 p1(circlePoints[i].x, circlePoints[i].y, z1);
        point3 p2(0.0f, 0.0f, z2);
        point3 p3(circlePoints[(i+1)%numDivisions].x, circlePoints[(i+1)%numDivisions].y, z1);
        if (dir == -1)
        {
            point3 temp = p1;
            p1 = p3;
            p3 = temp;
        }
        point3 n = cross(p1-p2, p3-p2);
        destp[Index] = p1; destn[Index] = n; Index++;
        destp[Index] = p2; destn[Index] = n; Index++;
        destp[Index] = p3; destn[Index] = n; Index++;
    }    
}

void generateCone(GLuint program, ShapeData* coneData)
{
    makeCircle(circlePoints, numConeDivisions);
    int Index = 0;
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1.0f, 1.0f, Index, 1);
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1.0f, -1.0f, Index, -1);
    
    coneData->numVertices = numConeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &coneData->vao );
    glBindVertexArray( coneData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)conePoints,  sizeof(conePoints),
                    (float*)coneNormals, sizeof(coneNormals),
                    0, 0);
}

//----------------------------------------------------------------------------
// Cylinder

const int numCylDivisions = 32;
const int numCylVertices = numCylDivisions * 12;

point4 cylPoints [numCylVertices];
point3 cylNormals[numCylVertices];

void generateCylinder(GLuint program, ShapeData* cylData)
{
    makeCircle(circlePoints, numCylDivisions);
    int Index = 0;
    makeConeWall(cylPoints, cylNormals, numCylDivisions, 1.0f, 1.0f, Index, 1);
    makeConeWall(cylPoints, cylNormals, numCylDivisions, -1.0f, -1.0f, Index, -1);
    
    for (int i = 0; i < numCylDivisions; i++)
    {
        int i2 = (i+1)%numCylDivisions;
        point3 p1(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        point3 p2(circlePoints[i2].x, circlePoints[i2].y, 1.0f);
        point3 p3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        //point3 n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
        p1 = point3(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        p2 = point3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        p3 = point3(circlePoints[i].x,  circlePoints[i].y,  -1.0f);
        //n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
    }
    
    cylData->numVertices = numCylVertices;
    
    // Create a vertex array object
    glGenVertexArrays( 1, &cylData->vao );
    glBindVertexArray( cylData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)cylPoints,  sizeof(cylPoints),
                    (float*)cylNormals, sizeof(cylNormals),
                    0, 0);
}

//----------------------------------------------------------------------------
// Half Hole

const int numHoleDivisions = 16;
const int numHoleVertices = numHoleDivisions * 18 + 18;

point4 holePoints [numHoleVertices];
point3 holeNormals[numHoleVertices];

// The height of the hole
GLfloat holeHeight = 1.0f;

void generateHole(GLuint program, ShapeData* holeData)
{
	makeHalfCircle(circlePoints, numHoleDivisions);
	int Index = 0;

	point3 p1, p2, p3, p4, p5, p6, p7, p8;

    for (int i = 0; i < numHoleDivisions; i++)
    {
    	// First, make the curvy wall of the hole
    	p1 = point3(circlePoints[i].x, circlePoints[i].y-1, 1.0f);
    	p2 = point3(circlePoints[i+1].x, circlePoints[i+1].y-1, -1.0f);
    	p3 = point3(circlePoints[i].x, circlePoints[i].y-1, -1.0f);

    	GLfloat norm_x1 = -p1.x, norm_y1 = -p1.y, norm_x2 = -p2.x, norm_y2 = -p2.y, norm_x3 = -p3.x, norm_y3 = -p3.y;
    	if( i >= numHoleDivisions/2) {
    		norm_x1 = p1.x;
    		norm_y1 = p1.y;
    		norm_x2 = p2.x;
    		norm_y2 = p2.y;
    		norm_x3 = p3.x;
    		norm_y3 = p3.y;
    	}
        holePoints[Index] = p1; holeNormals[Index] = point3(norm_x1, norm_y1, 0.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(norm_x2, norm_y2, 0.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(norm_x3, norm_y3, 0.0f); Index++;

        p3 = p2;
        p2 = point3(circlePoints[i+1].x,  circlePoints[i+1].y-1,  1.0f);
        holePoints[Index] = p1; holeNormals[Index] = point3(norm_x1, norm_y1, 0.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(norm_x2, norm_y2, 0.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(norm_x3, norm_y3, 0.0f); Index++;

        // Then, make the front all of the hole
        p3 = point3(circlePoints[i].x, holeHeight-1, 1.0f);
        holePoints[Index] = p1; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;

        p1 = point3(circlePoints[i+1].x, holeHeight-1, 1.0f);
        holePoints[Index] = p1; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(0.0f, 0.0f, 1.0f); Index++;

        // Then, make the back wall of the hole
        p1.z = -1.0f * p1.z;
        p2.z = -1.0f * p2.z;
        p3.z = -1.0f * p3.z;
        holePoints[Index] = p1; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;

        p1 = point3(circlePoints[i].x, circlePoints[i].y-1, -1.0f);
        holePoints[Index] = p1; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
        holePoints[Index] = p2; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
        holePoints[Index] = p3; holeNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
    }

    // Generate the top, left, right rectangles
    p1 = point3(1.0f, 0.0f-1, 1.0f);
    p2 = point3(1.0f, 0.0f-1, -1.0f);
    p3 = point3(1.0f, holeHeight-1, -1.0f);
    p4 = point3(1.0f, holeHeight-1, 1.0f);
    p5 = point3(-1.0f, 0.0f-1, 1.0f);
    p6 = point3(-1.0f, 0.0f-1, -1.0f);
    p7 = point3(-1.0f, holeHeight-1, -1.0f);
    p8 = point3(-1.0f, holeHeight-1, 1.0f);

    // Draw right rectangles
    holePoints[Index] = p1; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p2; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p4; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;

    holePoints[Index] = p2; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p3; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p4; holeNormals[Index] = point3(1.0f, 0.0f, 0.0f); Index++;

    // Draw top rectangles
    holePoints[Index] = p4; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
    holePoints[Index] = p3; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
    holePoints[Index] = p7; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;

    holePoints[Index] = p4; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
    holePoints[Index] = p7; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
    holePoints[Index] = p8; holeNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;

    // Draw left rectangles
    holePoints[Index] = p6; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p8; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p7; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

    holePoints[Index] = p5; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p8; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
    holePoints[Index] = p6; holeNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

    holeData->numVertices = numHoleVertices;

    glGenVertexArrays( 1, &holeData->vao );
    glBindVertexArray( holeData->vao );

    setVertexAttrib(program,
    				(float*)holePoints, sizeof(holePoints),
    				(float*)holeNormals, sizeof(holeNormals),
    				0, 0);
}

//----------------------------------------------------------------------------
// Quarter Hole

const int numQuarterHoleDivisions = 8;
const int numQuarterHoleVertices = numQuarterHoleDivisions * 18 + 12;

point4 quarterHolePoints [numQuarterHoleVertices];
point3 quarterHoleNormals[numQuarterHoleVertices];

void generateQuarterHole(GLuint program, ShapeData* quarterHoleData)
{
	point2 cirPoints[numQuarterHoleDivisions+1];
	int i, Index = 0;
	for(i = 0; i <= numQuarterHoleDivisions; i++)
	{
		float a =  M_PI / 2.0f + i * M_PI / numQuarterHoleDivisions / 2.0f;
		cirPoints[i] = point2(1.0f + cosf(a), 1.0f - sinf(a));
	}

	point3 p1, p2, p3, p4, p5, p6, p7, p8;
	for(i = 0; i < numQuarterHoleDivisions; i++)
	{
		p1 = point3(cirPoints[i].x,    0.5f, cirPoints[i].y);
		p2 = point3(cirPoints[i].x,   -0.5f, cirPoints[i].y);
		p3 = point3(cirPoints[i+1].x,  0.5f, cirPoints[i+1].y);
		p4 = point3(cirPoints[i+1].x, -0.5f, cirPoints[i+1].y);

		p5 = point3(p1.x, p1.y, 0.0f);
		p6 = point3(p2.x, p2.y, 0.0f);
		p7 = point3(p3.x, p3.y, 0.0f);
		p8 = point3(p4.x, p4.y, 0.0f);

		// Draw the curly phase of the Quarter Hole
		quarterHolePoints[Index] = p1; quarterHoleNormals[Index] = point3(p1.x, 0.0f, p1.z); Index++;
		quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(p3.x, 0.0f, p3.z); Index++;
		quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(p2.x, 0.0f, p2.z); Index++;

		quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(p2.x, 0.0f, p2.z); Index++;
		quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(p3.x, 0.0f, p3.z); Index++;
		quarterHolePoints[Index] = p4; quarterHoleNormals[Index] = point3(p4.x, 0.0f, p4.z); Index++;

		// Draw the Top of the Quarter Hole
		quarterHolePoints[Index] = p1; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p7; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;

		quarterHolePoints[Index] = p1; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p5; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p7; quarterHoleNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;

		// Draw the Bottom of the Quarter Hole
		quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p4; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p8; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;

		quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p8; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
		quarterHolePoints[Index] = p6; quarterHoleNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
	}

	p1 = point3(1.0f,  0.5f, 0.0f);
	p2 = point3(1.0f, -0.5f, 0.0f);
	p3 = point3(0.0f,  0.5f, 0.0f);
	p4 = point3(0.0f, -0.5f, 0.0f);
	p5 = point3(0.0f,  0.5f, 1.0f);
	p6 = point3(0.0f, -0.5f, 1.0f);

	// Draw the back of the Quarter Hole
	quarterHolePoints[Index] = p1; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;

	quarterHolePoints[Index] = p2; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	quarterHolePoints[Index] = p4; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;

	// Draw the left of the Quarter Hole
	quarterHolePoints[Index] = p3; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	quarterHolePoints[Index] = p4; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	quarterHolePoints[Index] = p5; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

	quarterHolePoints[Index] = p4; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	quarterHolePoints[Index] = p6; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	quarterHolePoints[Index] = p5; quarterHoleNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

	quarterHoleData->numVertices = numQuarterHoleVertices;

    glGenVertexArrays( 1, &quarterHoleData->vao );
    glBindVertexArray( quarterHoleData->vao );

    setVertexAttrib(program,
    				(float*)quarterHolePoints, sizeof(quarterHolePoints),
    				(float*)quarterHoleNormals, sizeof(quarterHoleNormals),
    				0, 0);
}

/***************************************************
 * generateEdgeCorner
 ***************************************************/
const int numEdgeCornerDivisions = 16;
const int numEdgeCornerVertices = numEdgeCornerDivisions * 24 + 12;

point4 edgeCornerPoints [numEdgeCornerVertices];
point3 edgeCornerNormals[numEdgeCornerVertices];

void generateEdgeCorner(GLuint program, ShapeData* edgeCornerData)
{
	point2 outerPoints[numEdgeCornerDivisions+1];
	point2 innerPoints[numEdgeCornerDivisions+1];
	int i, Index = 0;
	float a;

	for(i = 0; i <= numEdgeCornerDivisions; i++)
	{
		// First, generate outer points
		a = M_PI / 2.0f + i * M_PI / numEdgeCornerDivisions / 2.0f;
		outerPoints[i] = point2(2.0f * cosf(a), -2.0f * sinf(a));
		// Then, generate inner points
		a = M_PI / 4.0f + i * M_PI / numEdgeCornerDivisions;
		innerPoints[i] = point2(sqrt(0.5)*cosf(a)-0.5f, -1.0f*sqrt(0.5)*sinf(a)-0.5f);
	}

	point3 p1, p2, p3, p4, p5, p6, p7, p8, crs;
	for(i = 0; i < numEdgeCornerDivisions; i++)
	{
		p1 = point3(innerPoints[i].x,    0.5f, innerPoints[i].y);
		p2 = point3(innerPoints[i].x,   -0.5f, innerPoints[i].y);
		p3 = point3(innerPoints[i+1].x,  0.5f, innerPoints[i+1].y);
		p4 = point3(innerPoints[i+1].x, -0.5f, innerPoints[i+1].y);

		p5 = point3(outerPoints[i].x,    0.5f, outerPoints[i].y);
		p6 = point3(outerPoints[i].x,   -0.5f, outerPoints[i].y);
		p7 = point3(outerPoints[i+1].x,  0.5f, outerPoints[i+1].y);
		p8 = point3(outerPoints[i+1].x, -0.5f, outerPoints[i+1].y);

		// Generate Inner Curve phase.
		addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p3, p2, Index);
		addTriangle(edgeCornerPoints, edgeCornerNormals, p2, p3, p4, Index);

		// Generate Outer Curve phase.
		addTriangle(edgeCornerPoints, edgeCornerNormals, p5, p6, p7, Index);
		addTriangle(edgeCornerPoints, edgeCornerNormals, p6, p8, p7, Index);

		// Generate Top flat phase
		addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p5, p7, Index);
		addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p7, p3, Index);

		// Generate Bottom flat phase
		addTriangle(edgeCornerPoints, edgeCornerNormals, p2, p4, p6, Index);
		addTriangle(edgeCornerPoints, edgeCornerNormals, p4, p8, p6, Index);
	}

	p1 = point3(innerPoints[0].x,  0.5f, innerPoints[0].y);
	p2 = point3(innerPoints[0].x, -0.5f, innerPoints[0].y);
	p3 = point3(outerPoints[0].x,  0.5f, outerPoints[0].y);
	p4 = point3(outerPoints[0].x, -0.5f, outerPoints[0].y);

	// Draw the right flat rectangle
	addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p2, p4, Index);
	addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p4, p3, Index);

	p1 = point3(innerPoints[numEdgeCornerDivisions].x,  0.5f, innerPoints[numEdgeCornerDivisions].y);
	p2 = point3(innerPoints[numEdgeCornerDivisions].x, -0.5f, innerPoints[numEdgeCornerDivisions].y);
	p3 = point3(outerPoints[numEdgeCornerDivisions].x,  0.5f, outerPoints[numEdgeCornerDivisions].y);
	p4 = point3(outerPoints[numEdgeCornerDivisions].x, -0.5f, outerPoints[numEdgeCornerDivisions].y);

	// Draw the right flat rectangle
	addTriangle(edgeCornerPoints, edgeCornerNormals, p1, p3, p2, Index);
	addTriangle(edgeCornerPoints, edgeCornerNormals, p3, p4, p2, Index);

    edgeCornerData->numVertices = numEdgeCornerVertices;

    glGenVertexArrays( 1, &edgeCornerData->vao );
    glBindVertexArray( edgeCornerData->vao );

    setVertexAttrib(program,
    				(float*)edgeCornerPoints, sizeof(edgeCornerPoints),
    				(float*)edgeCornerNormals, sizeof(edgeCornerNormals),
    				0, 0);
}







/***************************************************
 * generateRoundCorner
 ***************************************************/
const int numRoundCornerDivisions = 16;
const int numRoundCornerVertices = numRoundCornerDivisions * 18 + 12;

point4 roundCornerPoints [numRoundCornerVertices];
point3 roundCornerNormals[numRoundCornerVertices];

void generateRoundCorner(GLuint program, ShapeData* roundCornerData)
{
	point2 outerPoints[numRoundCornerDivisions+1];
	int i, Index = 0;
	float a;

	for(i = 0; i <= numRoundCornerDivisions; i++)
	{
		// First, generate outer points
		a = M_PI / 2.0f + i * M_PI / numRoundCornerDivisions / 2.0f;
		outerPoints[i] = point2(2.0f * cosf(a), -2.0f * sinf(a));
	}

	point3 p1, p2, p3, p4, p5, p6, p7, p8, crs;
	for(i = 0; i < numRoundCornerDivisions; i++)
	{
		p1 = point3(outerPoints[i].x,    0.5f, 0.0f);
		p2 = point3(outerPoints[i].x,   -0.5f, 0.0f);
		p3 = point3(outerPoints[i+1].x,  0.5f, 0.0f);
		p4 = point3(outerPoints[i+1].x, -0.5f, 0.0f);

		p5 = point3(outerPoints[i].x,    0.5f, outerPoints[i].y);
		p6 = point3(outerPoints[i].x,   -0.5f, outerPoints[i].y);
		p7 = point3(outerPoints[i+1].x,  0.5f, outerPoints[i+1].y);
		p8 = point3(outerPoints[i+1].x, -0.5f, outerPoints[i+1].y);

		// Generate Outer Curve phase.
		addTriangle(roundCornerPoints, roundCornerNormals, p5, p6, p7, Index);
		addTriangle(roundCornerPoints, roundCornerNormals, p6, p8, p7, Index);

		// Generate Top flat phase
		addTriangle(roundCornerPoints, roundCornerNormals, p1, p5, p7, Index);
		addTriangle(roundCornerPoints, roundCornerNormals, p1, p7, p3, Index);

		// Generate Bottom flat phase
		addTriangle(roundCornerPoints, roundCornerNormals, p2, p4, p6, Index);
		addTriangle(roundCornerPoints, roundCornerNormals, p4, p8, p6, Index);
	}

	p1 = point3(0.0f,  0.5f, 0.0f);
	p2 = point3(0.0f, -0.5f, 0.0f);
	p3 = point3(outerPoints[0].x,  0.5f, outerPoints[0].y);
	p4 = point3(outerPoints[0].x, -0.5f, outerPoints[0].y);

	// Draw the right flat rectangle
	addTriangle(roundCornerPoints, roundCornerNormals, p1, p2, p4, Index);
	addTriangle(roundCornerPoints, roundCornerNormals, p1, p4, p3, Index);

	p3 = point3(outerPoints[numRoundCornerDivisions].x,  0.5f, outerPoints[numRoundCornerDivisions].y);
	p4 = point3(outerPoints[numRoundCornerDivisions].x, -0.5f, outerPoints[numRoundCornerDivisions].y);

	// Draw the right flat rectangle
	addTriangle(roundCornerPoints, roundCornerNormals, p1, p3, p2, Index);
	addTriangle(roundCornerPoints, roundCornerNormals, p3, p4, p2, Index);

    roundCornerData->numVertices = numRoundCornerVertices;

    glGenVertexArrays( 1, &roundCornerData->vao );
    glBindVertexArray( roundCornerData->vao );

    setVertexAttrib(program,
    				(float*)roundCornerPoints, sizeof(roundCornerPoints),
    				(float*)roundCornerNormals, sizeof(roundCornerNormals),
    				0, 0);
}








const int numTriPrismVertices = 24;

point4 triPrismPoints [numTriPrismVertices];
point3 triPrismNormals[numTriPrismVertices];
void generateTriPrism(GLuint program, ShapeData* triPrismData)
{
	int Index = 0;
	point3 p1(1.0f, -0.5f, 0.0f);
	point3 p2(0.0f, -0.5f, 0.0f);
	point3 p3(0.0f, -0.5f, 1.0f);
	point3 p4(1.0f,  0.5f, 0.0f);
	point3 p5(0.0f,  0.5f, 0.0f);
	point3 p6(0.0f,  0.5f, 1.0f);

	// Bottom of the triangular prism
	triPrismPoints[Index] = p1; triPrismNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
	triPrismPoints[Index] = p3; triPrismNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;
	triPrismPoints[Index] = p2; triPrismNormals[Index] = point3(0.0f, -1.0f, 0.0f); Index++;

	// Top of the triangular prism
	triPrismPoints[Index] = p4; triPrismNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
	triPrismPoints[Index] = p5; triPrismNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;
	triPrismPoints[Index] = p6; triPrismNormals[Index] = point3(0.0f, 1.0f, 0.0f); Index++;

	// Back of the triangular prism
	triPrismPoints[Index] = p1; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	triPrismPoints[Index] = p2; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	triPrismPoints[Index] = p4; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;

	triPrismPoints[Index] = p2; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	triPrismPoints[Index] = p5; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;
	triPrismPoints[Index] = p4; triPrismNormals[Index] = point3(0.0f, 0.0f, -1.0f); Index++;

	// Left of the triangular prism
	triPrismPoints[Index] = p2; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	triPrismPoints[Index] = p3; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	triPrismPoints[Index] = p6; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

	triPrismPoints[Index] = p2; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	triPrismPoints[Index] = p6; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;
	triPrismPoints[Index] = p5; triPrismNormals[Index] = point3(-1.0f, 0.0f, 0.0f); Index++;

	// Front of the triangular prism
	triPrismPoints[Index] = p1; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;
	triPrismPoints[Index] = p6; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;
	triPrismPoints[Index] = p3; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;

	triPrismPoints[Index] = p1; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;
	triPrismPoints[Index] = p4; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;
	triPrismPoints[Index] = p6; triPrismNormals[Index] = point3(1.0f, 0.0f, 1.0f); Index++;

    triPrismData->numVertices = numTriPrismVertices;

    glGenVertexArrays( 1, &triPrismData->vao );
    glBindVertexArray( triPrismData->vao );

    setVertexAttrib(program,
    				(float*)triPrismPoints, sizeof(triPrismPoints),
    				(float*)triPrismNormals, sizeof(triPrismNormals),
    				0, 0);
}





