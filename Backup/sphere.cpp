#include "sphere.h"
#include "Constants.h"
//for the table width and height
float tableWidth = 12.0f;
float tableHeight = 24.0f;
//friction constant
float frictionConst = 0.1;
void drawBall( ShapeData* sphere1, shaderVariables* var, mat4 model, mat4 view, GLuint texture ) {
	glBindTexture( GL_TEXTURE_2D, texture );
	glUniform1i( var->uEnableTex, 1 );
    glUniformMatrix4fv( var->uModelView, 1, GL_TRUE, model);
	glBindVertexArray( sphere1->vao );
	
	//sphere->currpos = model*sphere->center;
	//update the current position of the sphere
	

   //send to variables to shaders
//    glUniformMatrix4fv( var->uView, 1, GL_TRUE, view );

	
    glDrawArrays( GL_TRIANGLES, 0, sphere1->numVertices );

    glUniform1i( var->uEnableTex, 0 );
   
}

float distance(point4 from, point4 to) {
	return sqrt(pow(from.x-to.x,2)+pow(from.y-to.y,2)+pow(from.z-to.z,2));
}


void wallCollision(sphere* sphere1) {
	
	/*
	float distz =distance(sphere1->currpos, point4(sphere1->currpos.x,tableHeight,0,1)); //collision with top wall
	float distnz = distance(sphere1->currpos, point4(sphere1->currpos.x,-tableHeight,0,1)); //collision with bottom wall
	float distx = distance(sphere1->currpos, point4(tableWidth,sphere1->currpos.z,0,1)); //collision with right wall
	float distnx = distance(sphere1->currpos, point4(-tableWidth,sphere1->currpos.z,0,1)); //collision with left wall
	*/
	//collision with top wall, which is in in the positive x axis parallel to z axis
	float distx = distance(sphere1->currpos, point4(TABLE_LENGTH/2 +0.8, sphere1->currpos.y, sphere1->currpos.z,1)); 
	//collision with bottom wall, which is in the negative x axis, parallel to z-axis 
	float distnx = distance(sphere1->currpos, point4(-TABLE_LENGTH/2-0.8, sphere1->currpos.y, sphere1->currpos.z,1)); 
	float distz = distance(sphere1->currpos, point4(sphere1->currpos.x, sphere1->currpos.y, TABLE_WIDTH/2+0.8,1));
	float distnz = distance(sphere1->currpos, point4(sphere1->currpos.x, sphere1->currpos.y, -TABLE_WIDTH/2-0.8,1));
	//printf("distance to top wall: %f \n", distx);
	if(distx < sphere1->radius || sphere1->currpos.x > TABLE_LENGTH/2 +0.8 ) {
		//printf("wall collision  \n.");
		sphere1->currpos.x -= (sphere1->radius-distx);
		sphere1->friction.x = -sphere1->friction.x;
		sphere1->velocity.x = -sphere1->velocity.x;
	}
	if(distnx < sphere1->radius || sphere1->currpos.x < -TABLE_LENGTH/2 -0.8) {
		sphere1->currpos.x += (sphere1->radius-distnx);
		sphere1->friction.x = -sphere1->friction.x;
		sphere1->velocity.x = -sphere1->velocity.x;
	}
	if(distz < sphere1->radius || sphere1->currpos.z > TABLE_WIDTH/2+0.8) {
		sphere1->currpos.z -= (sphere1->radius-distz);
		sphere1->friction.z = -sphere1->friction.z;
		sphere1->velocity.z = -sphere1->velocity.z;
	}
		if(distnz < sphere1->radius || sphere1->currpos.z < -TABLE_WIDTH/2-0.8) {
		sphere1->currpos.z += (sphere1->radius-distnz);
		sphere1->friction.z = -sphere1->friction.z;
		sphere1->velocity.z = -sphere1->velocity.z;
	}
	/*float distx =abs(sphere1->currpos.x - TABLE_LEN/2.0f);//distance(sphere1->currpos, point4(0.5,0,sphere1->currpos.z,1)); //collision with top wall
	float distnx = abs(sphere1->currpos.x + TABLE_LEN/2.0f); //distance(sphere1->currpos, point4(sphere1->currpos.x,0,-TABLE_LENGTH/2,1)); //collision with bottom wall
	float distz = abs(sphere1->currpos.z - TABLE_WID/2.0f); //distance(sphere1->currpos, point4(TABLE_LENGTH/2.0f,0,sphere1->currpos.z,1)); //collision with right wall
	float distnz = abs(sphere1->currpos.z + TABLE_WID/2.0f); //distance(sphere1->currpos, point4(-TABLE_LENGTH/2.0f,0,sphere1->currpos.z,1)); //collision with left wall
	
	if( distz < sphere1->radius || sphere1->currpos.z > TABLE_WID / 2.0f )
	{
		sphere1->currpos.z-=(sphere1->radius-distz);//+0.05);
		sphere1->friction.z= -sphere1->friction.z;
		sphere1->velocity.z = -sphere1->velocity.z;
	}
	if( distnz < sphere1->radius || sphere1->currpos.z < -TABLE_WID / 2.0f )
	{
		sphere1->currpos.z+=(/*sphere1->radius-distnz);//+0.05);
		sphere1->friction.z = -sphere1->friction.z;
		sphere1->velocity.z = -sphere1->velocity.z;
	}
	if( distx < sphere1->radius || sphere1->currpos.x > TABLE_LEN /2.0f )
	{
		sphere1->currpos.x-=(sphere1->radius-distx);//+0.05);
		sphere1->friction.x = -sphere1->friction.x;
		sphere1->velocity.x = -sphere1->velocity.x;
	}
	if( distnx < sphere1->radius || sphere1->currpos.x <= -TABLE_LEN / 2.0f )
	{
		sphere1->currpos.x+=(sphere1->radius-distnx);//+0.05);
		sphere1->friction.x = -sphere1->friction.x;
		sphere1->velocity.x = -sphere1->velocity.x;
	}*/
	
//collision with top wall
	/*if( distz < sphere1->radius || sphere1->currpos.z > tableHeight ) { sphere1->currpos.z-=(sphere1->radius-distz+0.05);
	sphere1->friction.z= -sphere1->friction.z;
	sphere1->velocity.z = -sphere1->velocity.z;
	}
	if( distnz < sphere1->radius || sphere1->currpos.z < -tableHeight ) { sphere1->currpos.z+=(sphere1->radius-distnz+0.05);
	sphere1->friction.z = -sphere1->friction.z;
	sphere1->velocity.z = -sphere1->velocity.z;
	}
	if( distx < sphere1->radius || sphere1->currpos.x > tableWidth ) {sphere1->currpos.x-=(sphere1->radius-distx+0.05);
	sphere1->friction.x = -sphere1->friction.x;
	sphere1->velocity.x = -sphere1->velocity.x;
	}
	if( distnx < sphere1->radius || sphere1->currpos.x <= -tableWidth ) { sphere1->currpos.x+=(sphere1->radius-distnx+0.05);
	sphere1->friction.x = -sphere1->friction.x;
	sphere1->velocity.x = -sphere1->velocity.x;
	}*/
}
bool pocketCollision(sphere* sphere1)
{
	float disty =distance(sphere1->currpos, point4(sphere1->currpos.x,2,0,1)); //collision with top wall
	float distny = distance(sphere1->currpos, point4(sphere1->currpos.x,-2,0,1)); //collision with bottom wall
	float distx = distance(sphere1->currpos, point4(1,sphere1->currpos.y,0,1)); //collision with right wall
	float distnx = distance(sphere1->currpos, point4(-1,sphere1->currpos.y,0,1)); //collision with left wall
	//collision with top wall
	//top left
	if((sphere1->currpos.y>1.86) && (sphere1->currpos.x<=-.9))
	{ 
		return false;
	}
	//top right
	if((sphere1->currpos.y>1.86) && (sphere1->currpos.x>=.9))
	{ 
		return false;
	}
	//left
	if((sphere1->currpos.x<=-.9) && (sphere1->currpos.y<.1 && sphere1->currpos.y>-.1)) { 
		return false;
	}
	//right
	if((sphere1->currpos.x>=.9) && (sphere1->currpos.y<.1 && sphere1->currpos.y>-.1)) {
		return false;
	}
	//bottom left
	if((sphere1->currpos.y<-1.86) && (sphere1->currpos.x<=-.88)) { 
		return false;
	}
	//bottom right
	if((sphere1->currpos.y<-1.86) && (sphere1->currpos.x>=.88)) { 
		return false;
	}

	return true;
}
void moveBall(sphere* sphere1, float diff) {
	

	if (sphere1->velocity.x !=0) sphere1->velocity.x -= diff*sphere1->friction.x;
	if (sphere1->velocity.z !=0) sphere1->velocity.z -= diff*sphere1->friction.z;

	//update position of velocity
	sphere1->currpos.x += sphere1->velocity.x*(diff);
	sphere1->currpos.z += sphere1->velocity.z*(diff);

	//if the ball is moving in positive direction in both x and z
	if( sphere1->friction.x>=0 && sphere1->friction.z>=0 ) {
		if( sphere1->velocity.x<=0 && sphere1->velocity.z<=0 ) {sphere1->velocity = vec3(0,0,0); sphere1->moving = false;}
	}
	//if the ball is moving in positive z direction, negative x direction
	if( sphere1->friction.z>=0 && sphere1->friction.x<=0 ) {
		if( sphere1->velocity.x>=0 && sphere1->velocity.z<=0 ) {sphere1->velocity = vec3(0,0,0); sphere1->moving = false;}
	}
	//ball moving negative x-direction, negative z-direction
	if( sphere1->friction.z<=0 && sphere1->friction.x<=0 ) {
		if( sphere1->velocity.x>=0 && sphere1->velocity.z>=0 ) {sphere1->velocity = vec3(0,0,0); sphere1->moving = false;}
	}

	//ball moving positive x-direction, negative z-direction
	if( sphere1->friction.z<=0 && sphere1->friction.x>=0 ) {
		if( sphere1->velocity.x<=0 && sphere1->velocity.z>=0 ) {sphere1->velocity = vec3(0,0,0); sphere1->moving = false;}
	}

}




/************************
test for ball collision and changes the velocity directions




***************************/
void ballCollision( sphere* circle1, sphere* circle2, float diff ) {
	float ox1 = circle1->currpos.x;
	float oz1 = circle1->currpos.z;
	float ox2 = circle2->currpos.x;
	float oz2 = circle2->currpos.z;
	
	//sets x and z coordinates for both balls
	float x1 = circle1->currpos.x;
	float z1 = circle1->currpos.z;
	float x2 = circle2->currpos.x;
	float z2 = circle2->currpos.z;

	float dx = x2-x1; //distance between x axis of balls
	float dz = z2-z1; //distance between z axis of balls
	float d = sqrt(dx*dx+dz*dz); //distance between center of balsl
	//check if a collision did occur
	if(d<circle1->radius+circle2->radius) {
		//velocities of the balls before collision
		float vx1 = circle1->velocity.x;
		float vz1 = circle1->velocity.z;
		float vx2 = circle2->velocity.x;
		float vz2 = circle2->velocity.z;
		if(d == 0.0) d = 0.0001;
		//component of the velocity
		float vp1 = (vx1*dx+vz1*dz)/d;
		float vp2 = (vx2*dx+vz2*dz)/d;
		double dt = 0;
		//collision occurs at time when they are touching
		if(vp1-vp2 != 0.0) {
			dt = (circle1->radius+circle2->radius-d)/(vp1-vp2);
		}
		//move circles backward in time
		x1 -=vx1*dt;
		z1 -=vz1*dt;
		x2 -=vx2*dt;
		z2 -=vz2*dt;
		
		dx = x2-x1;
		dz = z2-z1;
		d = sqrt(dx*dx+dz*dz);

		float ax = dx/d;
		float az = dz/d;

		//components of velocity in direction of center
		float va1 = vx1*ax + vz1*az;
		float vb1 = -vx1*az + vz1*ax;
		float va2 =  vx2*ax + vz2*az;
		float vb2 = -vx2*az + vz2*ax;

		// calculate new velocity after collision
		float ed = 1;
		float vaP1 = va1 + (1 + ed) * (va2 - va1) / (1 + 1);
		float vaP2 = va2 + (1 + ed) * (va1 - va2) / (1 + 1);

		// undo projections
		vx1 = vaP1*ax - vb1*az;
		vz1 = vaP1*az + vb1*ax;
		vx2 = vaP2*ax - vb2*az;
		vz2 = vaP2*az + vb2*ax;

		
	
			
		// move time dt forward
		x1 += vx1 * dt;
		z1 += vz1 * dt;
		x2 += vx2 * dt;
		z2 += vz2 * dt;
		

			if( abs(vx1+vz1) >= 0.7 ) {
		//	while (abs(vx1) > 0.7) {
				vx1 *= 0.70;
			//}
			//while (abs(vz1) > 0.7) {
				vz1 *= 0.70;
		//	}
		}
		if( abs(vx2+vz2) >= 0.7) {
			//while (abs(vx2) > 0.7) {
				vx2 *= 0.70;
			//}
			//while (abs(vz2) > 0.7) {
				vz2 *= 0.70;
			//}
		}
		
		circle1->velocity.x = vx1;
		circle1->velocity.z = vz1;
		circle2->velocity.x = vx2;
		circle2->velocity.z = vz2;
		
		
		if(x1 > TABLE_LENGTH/2 || x1 < -TABLE_LENGTH/2 || z1 > TABLE_WIDTH/2 || z1 < -TABLE_WIDTH/2) {
			circle1->currpos.x = ox1;
			circle1->currpos.z = oz1;
			
		}
		else {
			circle1->currpos.x = x1;
			circle1->currpos.z = z1;
		}

		if(x2 >TABLE_LENGTH/2 || x2 < -TABLE_LENGTH/2 || z2 > TABLE_WIDTH/2 || z2 < -TABLE_WIDTH/2) {
			
			circle2->currpos.x = ox2;
			circle2->currpos.z = oz2;
		}
		else {
			circle2->currpos.x = x2;
			circle2->currpos.z = z2;
		}


		
		/*
			circle1->currpos.x = x1;
			circle1->currpos.z = z1;
			circle2->currpos.x = x2;
			circle2->currpos.z = z2;
		*/

		circle1->friction.x = frictionConst*circle1->velocity.x;
		circle1->friction.z = frictionConst*circle1->velocity.z;
		circle2->friction.x = frictionConst*circle2->velocity.x;
		circle2->friction.z = frictionConst*circle2->velocity.z;
		circle1->moving = true;
		circle2->moving = true;
		circle1->collided = true;
		circle1->collided = true;
	
	}
	
}
