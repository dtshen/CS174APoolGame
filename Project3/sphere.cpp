#include "Sphere.h"


int numBallsIn = 0;
/*******************************************************************************************/
//functions for the circle
void drawCircle(shaderVariables* var, circle* c, mat4 model_view) {

	model_view *= Translate(c->pos.x, c->pos.y, c->pos.z)*Scale(c->radius);
	glUniformMatrix4fv(var->uModelView, 1 , GL_TRUE, model_view);
		
	glDrawArrays(GL_TRIANGLE_STRIP, 0, numtriangles);
}


void createCircle(point4 v[], point4 p1[]) {
	float angle = 0; //the angle to calculate
	float angleincrement = 90.0/(totalpoints/12); //by how much should we increment the angle
		for (int i = 0; i < totalpoints/3; i++) {
		p1[i] = point4( cos(angle*M_PI/180), 0.0f, sin(angle*M_PI/180), 1.0f );
		angle += angleincrement;
	}
	p1[totalpoints/3] = p1[0];
	int j = 0;
	int counter =0;
	for (int i = 0; i < numtriangles; i++) {
		if(counter==2) {
			v[i] = point4(0,0,0,1);
			counter=0;
		}
		else {
			v[i] = p1[j];
			j++;
			counter++;
		}
	}
}


//returning true means that there WAS NO COLLISION
//returning false means that there WAS A  POCKET COLLISION
bool ballFallsInPocket(Sphere* s, circle* c) {
	//if it already fell into one of the pockets then exit
	if(s->falling) return false;

	
	float dx = s->pos.x - c->pos.x;
	float dz = s->pos.z - c->pos.z;
	float dist = sqrt(dx*dx + dz*dz);
	//this means the ball should fall
	if(dist < c->radius) {
		numBallsIn++;
		//printf("ball fell down. num balls fallen: %i \n", numBallsIn);
		s->falling = true;;
		//set the velocity of ball as if going to the center
		vec3 C = vec3(c->pos.x-s->pos.x,c->pos.y - s->pos.y, c->pos.z- s->pos.z);
		s->velocity = C;
		return false;
	}
	return true;
}

void applyGravity(Sphere* s, float diff) {
	if(s->pos.y <= 1.0f - TABLE_DEPTH) {
		s->moving = false;
		s->velocity = vec3(0,0,0);
		s->friction = vec3(0,0,0);
		s->angular_rot = 0;
		
		if(numBallsIn%2 == 1) {
		s->pos = point4((TABLE_LENGTH/2)*(numBallsIn),0,TABLE_WIDTH/2+3,1);
		}
		else {
		s->pos = point4((TABLE_LENGTH/2)*(numBallsIn),0,2*(TABLE_WIDTH/2)+3,1);
		}
		return;
	}
	float gravity = -5*diff;
	float dx = s->pos.x + s->velocity.x*diff;
	float dy = s->pos.y + gravity;
	float dz = s->pos.z + s->velocity.z*diff;
	s->pos = point4(dx, dy, dz, 1);
}
/*********************************************************************************************************/

void reset(Sphere* s) {
		s->rad = 0.5;
		s->velocity = vec3(0,0,0);
		s->friction = vec3(0,0,0);
		s->moving = false;
		s->rotateMat = Quaternions(0,0,0,0);
		s->angular_rot = 0;
		s->falling = false;
}

void drawBall(ShapeData* s, shaderVariables* var, mat4 model_view, GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(var->uEnableTex, 1);
	glUniformMatrix4fv(var->uModelView, 1, GL_TRUE, model_view);
	glBindVertexArray(s->vao);
	
	glDrawArrays(GL_TRIANGLES,0,s->numVertices);
	glUniform1i( var->uEnableTex, 0 );
	}
	
void moveBall(Sphere* s, float diff) {
	if(s->velocity.x != 0) s->velocity.x -= diff*s->friction.x;
	if(s->velocity.z != 0) s->velocity.z -= diff*s->friction.z;

	//update position with velocity
	s->pos.x += s->velocity.x*diff;
	s->pos.z += s->velocity.z*diff;

	if( s->friction.x >= 0 && s->friction.z >= 0 ) {
		if( s->velocity.x <= 0 && s->velocity.z <= 0 ) {s->velocity = vec3(0,0,0); s->moving = false;}
	}
	//if the ball is moving in positive z direction, negative x direction
	if( s->friction.z >= 0 && s->friction.x <= 0 ) {
		if( s->velocity.x >= 0 && s->velocity.z <= 0 ) {s->velocity = vec3(0,0,0); s->moving = false;}
	}
	//ball moving negative x-direction, negative z-direction
	if( s->friction.y <= 0 && s->friction.x <= 0 ) {
		if( s->velocity.x >= 0 && s->velocity.z >= 0 ) {s->velocity = vec3(0,0,0); s->moving = false;}
	}

	//ball moving positive x-direction, negative z-direction
	if( s->friction.z <= 0 && s->friction.x >= 0 ) {
		if( s->velocity.x<=0 && s->velocity.z >= 0 ) {s->velocity = vec3(0,0,0); s->moving = false;}
	}
}
float distance(point4 from, point4 to) {
	return sqrt(pow(from.x-to.x,2)+pow(from.y-to.y,2)+pow(from.z-to.z,2));
}

void wallCollision(Sphere* s) {
	
	//collision with top wall, which is in in the positive x axis parallel to z axis
	float distx = distance(s->pos, point4(TABLE_LENGTH/2 +0.8, s->pos.y, s->pos.z,1)); 
	//collision with bottom wall, which is in the negative x axis, parallel to z-axis 
	float distnx = distance(s->pos, point4(-TABLE_LENGTH/2-0.8, s->pos.y, s->pos.z,1)); 
	float distz = distance(s->pos, point4(s->pos.x, s->pos.y, TABLE_WIDTH/2+0.8,1));
	float distnz = distance(s->pos, point4(s->pos.x, s->pos.y, -TABLE_WIDTH/2-0.8,1));
	//printf("distance to top wall: %f \n", distx);
	float c;

	if(distx < s->rad || s->pos.x > TABLE_LENGTH/2 +0.8 ) {
		//printf("wall collision  \n.");
		s->pos.x += - s->rad+distx;
		s->velocity.x = -s->velocity.x;
		s->friction.x = -s->friction.x;
	}
	if(distnx < s->rad || s->pos.x < -TABLE_LENGTH/2 -0.8) {
		s->pos.x += s->rad-distnx;
		s->velocity.x = -s->velocity.x;
		s->friction.x = -s->friction.x;
	}
	if(distz < s->rad || s->pos.z > TABLE_WIDTH/2+0.8) {
		s->pos.z += -s->rad+distz;
		s->velocity.z = -s->velocity.z;
		s->friction.z = -s->friction.z;
	}
	if(distnz < s->rad || s->pos.z < -TABLE_WIDTH/2-0.8) {
		s->pos.z += s->rad - distnz;
		s->velocity.z = -s->velocity.z;
		s->friction.z = -s->friction.z;
	}
}



bool willBallsCollide(Sphere* s1, Sphere* s2) {
	float r = s1->rad;
	float dx = s2->pos.x - s1->pos.x;
	float dz = s2->pos.z - s1->pos.z;
	float dist1 = sqrt(dx*dx + dz*dz);
	if(dist1 >  2*r) return false;
	
	float dist = dist1-2*r;
	if(length(s1->velocity) < dist) return false;

	if(length(s1->velocity) == 0.0) return false;
	vec2 N = normalize(vec2(s1->velocity.x, s1->velocity.z));
	vec2 C = vec2(dx,dz);
	float D = dot(N,C);
	if(D<=0) return false;

	float lenC = length(C);
	float F = pow(lenC, 2) - pow(D, 2);
	if(F >= pow(2*r,2)) return false;

	float T = pow(2*r,2) -F;

	if(T < 0) return false;

	float distance2 = D-sqrt(T);
	double mag = length(s1->velocity);

	if(mag < distance2) return false;
	
	//vec3 vn = normalize(s1->velocity);
	//if all above tests did not pass then the balls will collide
	//s1->setPos(point4(s1->pos.x+vn.x*distance2, s1->pos.y, s1->pos.z+vn.y*distance2,1));
	return true;
}



void doBallCollision(Sphere* s1, Sphere* s2, float diff) {
	
	//printf("s2 vel: ( %f, %f, %f ) \n", s2->velocity.x, s2->velocity.y, s2->velocity.z);
	//cout << "s2 vel: " << s2->velocity << endl;
	float ox1 = s1->pos.x;
	float oz1 = s1->pos.z;
	float ox2 = s2->pos.x;
	float oz2 = s2->pos.z;

	//sets x and z coordinates for both balls
	float x1 = s1->pos.x;
	float z1 = s1->pos.z;
	float x2 = s2->pos.x;
	float z2 = s2->pos.z;

	float dx = x2-x1; //distance between x axis of balls
	float dz = z2-z1; //distance between z axis of balls
	float d = sqrt(dx*dx+dz*dz); //distance between center of balsl

	//check if a collision did occur
	if(d < 2*s1->rad) {
		//velocities of the balls before collision
		
		float vx1 = s1->velocity.x;
		float vz1 = s1->velocity.z;
		float vx2 = s2->velocity.x;
		float vz2 = s2->velocity.z;
		//if(d == 0.0) d = 0.0001;
		//component of the velocity

		float vp1 = (vx1*dx+vz1*dz)/d;
		float vp2 = (vx2*dx+vz2*dz)/d;
		//double dt = 0;
		//collision occurs at time when they are touching
		//if(vp1-vp2 != 0.0) {
		float dt = (2*s1->rad-d)/(vp1-vp2);
		//}
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
		
		/*
			if( abs(vx1) >= 0.7 || abs(vz1)>=0.7) {
		//	while (abs(vx1) > 0.7) {
				vx1 *= 0.70;
			//}
			//while (abs(vz1) > 0.7) {
				vz1 *= 0.70;
		//	}
		}
			if( abs(vx2) >= 0.7  || abs(vz2) >=0.7) {
			//while (abs(vx2) > 0.7) {
				vx2 *= 0.70;
			//}
			//while (abs(vz2) > 0.7) {
				vz2 *= 0.70;
			//}
		}*/
		s1->velocity = vec3(vx1,0,vz1); 
		s2->velocity = vec3(vx2,0 , vz2);
		
		
		if(x1 > TABLE_LENGTH/2 || x1 < -TABLE_LENGTH/2 || z1 > TABLE_WIDTH/2 || z1 < -TABLE_WIDTH/2) {
			//s1->setPos(point4(ox1, s1->pos.y,oz1,1));
			s1->pos.x = ox1;
			s1->pos.z = oz1;
			
		}
		else {
			//s1->setPos(point4(x1, s1->pos.y,z1,1));
			s1->pos.x = x1;
			s1->pos.z = z1;
		}

		if(x2 >TABLE_LENGTH/2 || x2 < -TABLE_LENGTH/2 || z2 > TABLE_WIDTH/2 || z2 < -TABLE_WIDTH/2) {
			//s2->setPos(point4(ox2, s2->pos.y,oz2,1));
			s1->pos.x = ox2;
			s2->pos.z = oz2;
		}
		else {
			//s2->setPos(point4(x2, s2->pos.y,z2,1));
			s2->pos.x = x2;
			s2->pos.z = z2;
		}

		s1->friction = FRICTION_CONST*s1->velocity;
		s2->friction = FRICTION_CONST*s2->velocity;
		s1->moving = true;
		s2->moving = true;
	}
}

