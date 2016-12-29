#ifndef SKYBOX_H
#define SKYBOX_H

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

class Skybox
{
public:
	Skybox (  
  	const char* posx,
  	const char* posy,
  	const char* posz,
  	const char* negx,
  	const char* negy,
  	const char* negz,
	  float initScale);
	virtual ~Skybox();
	GLuint vao;
	GLuint vbo;
	GLuint tex_cube;  
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	float boxScale;

	void render();
	void updateView(glm::mat4 viewMatrix);
	void updateProjection(glm::mat4 projectionMatrix);	
	
	GLuint progID;
};

extern Skybox theSkybox;

#endif
