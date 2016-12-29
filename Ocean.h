#ifndef OCEAN_H
#define OCEAN_H

#include <stdlib.h>
#include <string>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.h"
#include "sh_matrix.h"
#include "cubemap.h"


class Ocean
{
public:
	Ocean (const char* posx,
  	const char* posy,
  	const char* posz,
  	const char* negx,
  	const char* negy,
  	const char* negz,	 
	  float pposx,
	  float ppoxy,
	  float pposz,
	  float initScale);
	virtual ~Ocean();
	GLuint vao;
	GLuint vbo;
	GLuint tex_cube;  
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
        glm::vec3 cameraPosition;
	float timer;
	float boxScale;

	void render();
	void updateView(glm::mat4 viewMatrix);
	void updateProjection(glm::mat4 projectionMatrix);	
	void updatePosition(glm::vec3 cameraPosition);
	GLuint progID;

};

extern Ocean theOcean;

#endif
