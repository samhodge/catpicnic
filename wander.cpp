#include "wander.h"
#include <iostream>

SteeringCat::SteeringCat (	glm::vec3 initPosition, 
							glm::vec3 initVelocity, 
							float initMass) {
	position = glm::vec3(initPosition[0],0.0f,initPosition[2]);
	velocity = glm::vec3(initVelocity[0],0.0f,initVelocity[2]);
	mass = initMass;
	previousVelocity = initVelocity;
  	CIRCLE_RADIUS=0.01;
	CIRCLE_DISTANCE=0.080;
	ANGLE_CHANGE=12.0f;
	PEN_CENTRE=glm::vec3(0.0f,0.0f,0.0f);
	PEN_RADIUS=50;
	PEN_INNER_EDGE=0.4f;
	PEN_OUTER_EDGE=1.0f;
	PEN_FORCE_AMOUNT=2.00f;
	UP=glm::vec3(0,1.0,0);
	steeringForce=glm::vec3(0.0,0.0,0.0);
	boundingForce=glm::vec3(0.0,0.0,0.0);
	MAX_PEN_FORCE=0.50;
	MAX_STEER_FORCE=0.80;
	MAX_STEER_SPEED=0.30;
	MAX_PEN_SPEED=1.0;
	MAX_SPEED=0.068;
	STEER_NOISE_FREQ=0.001f;
	wanderAngle = 0.1f;
	timer=float(rand()%1337)/1337.0f;
	//Simplex.init();

#ifdef DEBUG	
		std::cout << position[0] << ","
		 << position[1] << ","
		 << position[2] << std::endl;
#endif
	srand(time(NULL));
}

SteeringCat::~SteeringCat(){
}


glm::vec3 SteeringCat::truncate( glm::vec3 input, float maxiumMagnitude){
	glm::vec3 truncated;
	if (glm::length(input) > maxiumMagnitude){
#ifdef DEBUG		
		std::cout << "truncated" << std::endl;
#endif	
		truncated = maxiumMagnitude*glm::normalize(input);
	}else{
		truncated = glm::vec3(input);
	}
	return truncated;
}

void SteeringCat::update(){
	timer+=STEER_NOISE_FREQ;
	boundary();
	wander();	
	boundingForce = truncate(boundingForce,MAX_PEN_FORCE);
#ifdef DEBUG	
	std::cout << "Bounding Force: " << boundingForce[0] << ","
		 << boundingForce[1] << ","
		 << boundingForce[2] << std::endl;
	std::cout << "Mass: " << mass << std::endl;
#endif		
	steeringForce = truncate(steeringForce,MAX_STEER_FORCE);
	
#ifdef DEBUG
		std::cout << "Steering Force: " << steeringForce[0] << ","
		 << steeringForce[1] << ","
		 << steeringForce[2] << std::endl;
#endif
	glm::vec3 totalForce =  steeringForce;
	glm::vec3 updateVelocity = truncate(totalForce/mass,MAX_STEER_SPEED);
	previousVelocity = velocity;
	previousPosition = position;
	velocity = truncate(velocity + updateVelocity,MAX_SPEED);
#ifdef DEBUG	
	std::cout << "New Velocity: "<< velocity[0] << ","
		 << velocity[1] << ","
		 << velocity[2] << std::endl;
#endif
	position = position + velocity;
#ifdef DEBUG	
	std::cout << "New Position: "<< position[0] << ","
		 << position[1] << ","
		 << position[2] << std::endl;
#endif	
	previousDirection = direction;
	direction =  position - previousPosition;
}

glm::mat4 SteeringCat::getTransform(){
	glm::mat4 rotation=glm::orientation(glm::normalize(direction),glm::vec3(1.0,0.0,0.0));
	glm::mat4 translate=glm::translate(glm::mat4(),position);
	//return orientation;
	return translate*rotation*glm::rotate(glm::mat4(),glm::radians(90.0f),glm::vec3(0,1,0)); 
}

float SteeringCat::getAngularVelocity(){
	float angle =0.0;
	if ((glm::length(velocity) == 0.0) || (glm::length(previousVelocity) == 0.0)) return 0.0;
	glm::vec3 normVel = glm::normalize(velocity);
	glm::vec3 prevVel = glm::normalize(previousVelocity);
	float dot = glm::dot(normVel,prevVel);
	if (dot > 1.0) return 0.0;
	angle = glm::acos(dot);
	float direction = prevVel.x*normVel.z - prevVel.z*normVel.x;
	if (direction < 0.0){
		angle *= -1.0;
	}
	return angle;
}


void SteeringCat::wander(){
	if (glm::length(velocity + boundingForce/mass) < 0.0001) {
			steeringForce = glm::vec3(); 
			return ;
	}
	circleCentre = CIRCLE_DISTANCE * glm::normalize(velocity + boundingForce/mass);
	glm::vec3 circleForwardAxis = glm::normalize(velocity + boundingForce/mass);
	glm::vec3 circleSideAxis = glm::normalize(glm::cross(glm::normalize(velocity + boundingForce/mass),UP));

#ifdef DEBUG	
		std::cout << "circleCentre: " << circleCentre[0] << ","
		 << circleCentre[1] << ","
		 << circleCentre[2] << std::endl;	
#endif	
	 	displacement = CIRCLE_RADIUS*glm::normalize(glm::sin(glm::radians(wanderAngle))*circleForwardAxis + glm::cos(glm::radians(wanderAngle))*circleSideAxis);
#ifdef DEBUG	
	std::cout << "displacement: " << displacement[0] << ","
		 << displacement[1] << ","
		 << displacement[2] << std::endl;
#endif
	wanderAngle += ANGLE_CHANGE*float(rand()%2)/1.0 - 0.5*ANGLE_CHANGE;
#ifdef DEBUG	
	std::cout << "wanderAngle: " << wanderAngle << std::endl;
#endif
	steeringForce = circleCentre + displacement;
#ifdef DEBUG
	std::cout << "steeringForce: " << steeringForce[0] << ","
		 << steeringForce[1] << ","
		 << steeringForce[2] << std::endl;
#endif
}


float clamp( float x, float a, float b){
	if ( x < a) return a;
	if ( x > b) return b;
	return x;
	
}

float smoothstep(float edge0, float edge1, float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x*x*(3 - 2 * x);
}

void SteeringCat::boundary(){
	glm::vec3 distanceFromCentre = (PEN_CENTRE - position);
	if (glm::length(distanceFromCentre) < 0.0001){
		boundingForce = glm::vec3(0.0,0.0,0.0);
		return ;
	}
	glm::vec3 boundaryForceDirection = glm::normalize(distanceFromCentre);
	float normalisedRadialDistance = glm::length(position-PEN_CENTRE)/PEN_RADIUS;
	float magnitude = smoothstep(PEN_INNER_EDGE,PEN_OUTER_EDGE,normalisedRadialDistance);
	boundingForce = magnitude*MAX_PEN_FORCE*boundaryForceDirection;
}

