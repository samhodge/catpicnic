#ifndef WANDER_H
#define WANDER_H

#include <stdlib.h>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <time.h>

class SteeringCat
{

public:
	SteeringCat ( 	glm::vec3 initPosition, 
				glm::vec3 initVelocity, 
				float initMass);
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 direction;
	float mass;
	void update();
	glm::mat4 getTransform();
	float getAngularVelocity();
	virtual ~SteeringCat();  
protected:
	glm::vec3 steeringForce;
	glm::vec3 boundingForce;
	void wander();
	void boundary();
	glm::vec3 previousVelocity;
	glm::vec3 previousPosition;
	glm::vec3 previousDirection;
private:
	float wanderAngle;
	float CIRCLE_RADIUS;
	float CIRCLE_DISTANCE;
	float ANGLE_CHANGE;
	glm::vec3 circleCentre;
	glm::vec3 displacement;
	//TODO Add avoidance field
	glm::vec3 PEN_CENTRE;
	float PEN_RADIUS;
	float PEN_INNER_EDGE;
	float PEN_OUTER_EDGE;
	float PEN_FORCE_AMOUNT;
	float MAX_PEN_FORCE;
	float MAX_STEER_FORCE;
	float MAX_STEER_SPEED;
	float MAX_PEN_SPEED;
	float MAX_SPEED;	
	glm::vec3 UP;
	glm::vec3 truncate( glm::vec3 input, float maxiumMagnitude);
	int timer;
	float STEER_NOISE_FREQ;
	};


extern SteeringCat theSteerer;

#endif
