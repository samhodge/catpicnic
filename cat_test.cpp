#include "wander.h"
#include <iostream>

int main(int argc, char *argv[])
{
	SteeringCat* cat;
	cat = new SteeringCat(glm::vec3(0.0,0.0,0.0),glm::vec3(1.0,0.0,0.0),1.0f);
	
	for( int i=0;i<20;i++){
		cat->update();
		/*
		std::cout << cat->position[0] << ","
		 << cat->position[1] << ","
		 << cat->position[2] << std::endl;
		 */
		 float a = cat->getAngularVelocity();
		 std::cout << a << std::endl;
	}
}