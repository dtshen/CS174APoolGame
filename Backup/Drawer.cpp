#include "Drawer.h"
#include "Shapes.h"
void Drawer::initDrawer(GLuint program)
{
    // Generate vertex arrays for geometric shapes
    generateCube(program, &m_cubeData);
//    generateSphere(program, &sphereData);
//    generateCone(program, &coneData);
    generateCylinder(program, &m_cylData);

    generateHole(program, &m_holeData);
    generateQuarterHole(program, &m_quarterHoleData);
    generateTriPrism(program, &m_triPrismData);
//	generateSphere( program, &sphereData );
	generateEdgeCorner(program, &m_edgeCornerData);
	generateRoundCorner(program, &m_roundCornerData);
}


void Drawer::setDrawerParam(mat4 m_view, shaderVariables vars)
{
	m_model_view = m_view;
	m_original_model_view = m_view;
	m_vars = vars;
}

/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1.
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//
// Don't change.
//////////////////////////////////////////////////////
void Drawer::drawCylinder(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_cylData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function
//          render a solid cone oriented along the Z axis with base radius 1.
//          The base of the cone is placed at Z = 0, and the top at Z = 1.
//
// Don't change.
//////////////////////////////////////////////////////
void Drawer::drawCone(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_coneData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
//
// Don't change.
//////////////////////////////////////////////////////

void Drawer::drawCube(void)
{
//    glBindTexture( GL_TEXTURE_2D, texture_cube );
//    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_cubeData.numVertices );
//    glUniform1i( uEnableTex, 0 );
}


//////////////////////////////////////////////////////
//    PROC: drawQuarterHole()
//////////////////////////////////////////////////////
void Drawer::drawQuarterHole(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_quarterHoleData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_quarterHoleData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawHole()
//////////////////////////////////////////////////////
void Drawer::drawHole(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_holeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_holeData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawTriPrism()
//////////////////////////////////////////////////////
void Drawer::drawTriPrism(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_triPrismData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_triPrismData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawEdgeCorner()
//////////////////////////////////////////////////////
void Drawer::drawEdgeCorner(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_edgeCornerData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_edgeCornerData.numVertices );
}

void Drawer::drawRoundCorner(void)
{
    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
    glBindVertexArray( m_roundCornerData.vao );
    glDrawArrays( GL_TRIANGLES, 0, m_roundCornerData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
//
// Don't change.
//////////////////////////////////////////////////////

//void drawSphere(void)
//{
//    //glBindTexture( GL_TEXTURE_2D, texture_earth);
//	glBindTexture(GL_TEXTURE_2D, ball_texture[1]);
//    glUniform1i( m_vars.uEnableTex, 1);
//    glUniformMatrix4fv( m_vars.uModelView, 1, GL_TRUE, m_model_view );
//    glBindVertexArray( sphereData.vao );
//    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
//    glUniform1i( m_vars.uEnableTex, 0 );
//}
void Drawer::drawHolesOnTable(void)
{
    set_colour(0.0f, 1.0f, 0.0f);

    m_model_view = m_original_model_view;

    // Draw the hole on the top middle
    m_model_view *= Translate(0.0f, 0.0f, -TABLE_WIDTH/2.0f-0.5f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 0.8f, TABLE_DEPTH/2.0f);
    drawHole();

    m_model_view = m_original_model_view;

    // Draw the hole on the bottom middle
    m_model_view *= Translate(0.0f, 0.0f, TABLE_WIDTH/2.0f+0.5f);
    m_model_view *= RotateX(-90.0f);
    m_model_view *= Scale(1.0f, 0.8f, TABLE_DEPTH/2.0f);
    drawHole();

    // Draw the hole on the top left
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, 0.0f, -TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(180.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH, 1.0f);
    drawQuarterHole();

    // Draw the hole on the top right
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, 0.0f, -TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(90.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH, 1.0f);
    drawQuarterHole();

    // Draw the hole on the bottom left
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, 0.0f, TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(-90.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH, 1.0f);
    drawQuarterHole();

    // Draw the hole on the bottom right
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, 0.0f, TABLE_WIDTH/2.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH, 1.0f);
//    drawHole();
    drawQuarterHole();
}

void Drawer::drawTablePlane(void)
{
    set_colour(0.0f, 1.0f, 0.0f);

        m_model_view = m_original_model_view;
    // Draw Something
    // Previously glScalef(3,3,3);
    m_model_view *= Scale(TABLE_LENGTH, TABLE_DEPTH, TABLE_WIDTH);
    drawCube();

    GLfloat x_scale = TABLE_LENGTH/2.0f -1.0f;
    GLfloat x_trans = (TABLE_LENGTH/2.0f -2.0f)/2.0f + 1.5f;
    GLfloat z_trans = (TABLE_WIDTH/2.0f)+0.5f;

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-x_trans, 0.0f, -z_trans);
    m_model_view *= Scale(x_scale, 0.3f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(x_trans, 0.0f, -z_trans);
    m_model_view *= Scale(x_scale, 0.3f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-x_trans, 0.0f, z_trans);
    m_model_view *= Scale(x_scale, 0.3f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(x_trans, 0.0f, z_trans);
    m_model_view *= Scale(x_scale, 0.3f, 1.0f);
    drawCube();

    // Draw the right of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0+0.5, 0.0f, 0.0f);
    m_model_view *= Scale(1.0, 0.3f, 12.0f);
    drawCube();

    // Draw the left of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0-0.5, 0.0f, 0.0f);
    m_model_view *= Scale(1.0, 0.3f, 12.0f);
    drawCube();

    GLfloat tri_trans_x = TABLE_LENGTH/2.0f - 1.0f;
    GLfloat tri_trans_z = TABLE_WIDTH/2.0f - 1.0f;
}

void Drawer::drawTableEdge(void)
{
    set_colour(0.5f, 0.35f, 0.05f);

        m_model_view = m_original_model_view;

    // Draw the edge hole on the top middle
    m_model_view *= Translate(0.0f, 0.0f, -TABLE_WIDTH/2.0f - 2.0f);
    m_model_view *= RotateX(-90.0f);
    m_model_view *= Scale(1.0f, 1.0f, TABLE_DEPTH*2.0f);
    drawHole();

    m_model_view = m_original_model_view;

    // Draw the hole on the top middle
    m_model_view *= Translate(0.0f, 0.0f, TABLE_WIDTH/2.0f + 2.0f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 1.0f, TABLE_DEPTH*2.0f);
    drawHole();

    GLfloat x_scale = TABLE_LENGTH/2.0f -1.0f;
    GLfloat x_trans = (TABLE_LENGTH/2.0f -2.0f)/2.0f + 1.5f;
    GLfloat z_trans = (TABLE_WIDTH/2.0f)+1.5f;

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-x_trans, 0.0f, -z_trans);
    m_model_view *= Scale(x_scale, TABLE_DEPTH*4.0f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(x_trans, 0.0f, -z_trans);
    m_model_view *= Scale(x_scale, TABLE_DEPTH*4.0f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-x_trans, 0.0f, z_trans);
    m_model_view *= Scale(x_scale, TABLE_DEPTH*4.0f, 1.0f);
    drawCube();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(x_trans, 0.0f, z_trans);
    m_model_view *= Scale(x_scale, TABLE_DEPTH*4.0f, 1.0f);
    drawCube();

    // Draw the right edge of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0+1.5, 0.0f, 0.0f);
    m_model_view *= Scale(1.0, TABLE_DEPTH*4.0f, 12.0f);
    drawCube();

    // Draw the left edge of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0-1.5, 0.0f, 0.0f);
    m_model_view *= Scale(1.0, TABLE_DEPTH*4.0f, 12.0f);
    drawCube();

    // Draw the left top corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, 0.0f, -TABLE_WIDTH/2.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH*4.0f, 1.0f);
    drawEdgeCorner();

    // Draw the left bottom corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, 0.0f, TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(90.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH*4.0f, 1.0f);
    drawEdgeCorner();

    // Draw the right top corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, 0.0f, -TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(-90.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH*4.0f, 1.0f);
    drawEdgeCorner();

    // Draw the right bottom corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, 0.0f, TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(180.0f);
    m_model_view *= Scale(1.0f, TABLE_DEPTH*4.0f, 1.0f);
    drawEdgeCorner();
}

void Drawer::drawTableBottom(void)
{
    set_colour(0.0f, 0.0f, 0.0f);

    m_model_view = m_original_model_view;
    m_model_view *= Translate(0.0f, -TABLE_DEPTH*4.0f, 0.0f);
    m_model_view *= Scale(TABLE_LENGTH, 1.0f, TABLE_WIDTH + 4.0f);
    drawCube();

    // Draw the right edge of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0+1.0f, -TABLE_DEPTH*4.0f, 0.0f);
    m_model_view *= Scale(2.0, TABLE_DEPTH*4.0f, 12.0f);
    drawCube();

    // Draw the left edge of the table
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0-1.0f, -TABLE_DEPTH*4.0f, 0.0f);
    m_model_view *= Scale(2.0, TABLE_DEPTH*4.0f, 12.0f);
    drawCube();

    // Draw the left top rounded corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, -TABLE_DEPTH*4.0f, -TABLE_WIDTH/2.0f);
//    m_model_view *= Scale(1.0f, 1.0f, 1.0f);
    drawRoundCorner();

    // Draw the left bottom rounded corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(-TABLE_LENGTH/2.0f, -TABLE_DEPTH*4.0f, TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(90.0f);
//    m_model_view *= Scale(1.0f, 1.0f, 1.0f);
    drawRoundCorner();

    // Draw the right top corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, -TABLE_DEPTH*4.0f, -TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(-90.0f);
//    m_model_view *= Scale(1.0f, 1.0f, 1.0f);
    drawRoundCorner();

    // Draw the right bottom corner edge
    m_model_view = m_original_model_view;
    m_model_view *= Translate(TABLE_LENGTH/2.0f, -TABLE_DEPTH*4.0f, TABLE_WIDTH/2.0f);
    m_model_view *= RotateY(180.0f);
//    m_model_view *= Scale(1.0f, 1.0f, 1.0f);
    drawRoundCorner();

    /**********************Draw Table Legs**********************/
    set_colour(0.5f, 0.5f, 0.5f);

    m_model_view = m_original_model_view;
    m_model_view *= Translate(2.0f * TABLE_LENGTH/5.0f, -TABLE_DEPTH * 14.0f, -2.0f * TABLE_WIDTH / 5.0f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 1.0f, 3.0f);
    drawCylinder();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-2.0f * TABLE_LENGTH/5.0f, -TABLE_DEPTH * 14.0f, -2.0f * TABLE_WIDTH / 5.0f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 1.0f, 3.0f);
    drawCylinder();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(2.0f * TABLE_LENGTH/5.0f, -TABLE_DEPTH * 14.0f, 2.0f * TABLE_WIDTH / 5.0f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 1.0f, 3.0f);
    drawCylinder();

    m_model_view = m_original_model_view;
    m_model_view *= Translate(-2.0f * TABLE_LENGTH/5.0f, -TABLE_DEPTH * 14.0f, 2.0f * TABLE_WIDTH / 5.0f);
    m_model_view *= RotateX(90.0f);
    m_model_view *= Scale(1.0f, 1.0f, 3.0f);
    drawCylinder();
}


void Drawer::drawTable(void)
{
    drawTablePlane();
    drawHolesOnTable();
    drawTableEdge();
    drawTableBottom();
}
