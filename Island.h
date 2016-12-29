#ifndef ISLAND_H
#define ISLAND_H

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

typedef struct {
    float x;
    float y;
    float z;
} vertex;

typedef struct {
    float x;
    float y;
    float z;
} normal;

typedef struct {
    float u;
    float v;
} uv;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} colour;

typedef struct {
	float height;
} height;


typedef struct {
    vertex* verts;
    GLuint* indices;
    uv* uvs;
    colour* colours;
    height* heights;
    normal* normals;
    unsigned int resolution;
    unsigned int num_verts;
    unsigned int index_count;
} grid;

class Island
{
public:
	Island (const char* heightMap,
	  float scalex,
	  float scaley,
	  float scalez,
	  float posx,
	  float posy,
	  float posz);
	virtual ~Island();
	GLuint vao;
	GLuint vbo;
	GLuint tex_cube;  
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
        glm::vec3 cameraPosition;
	float timer;
	float boxScale;
	float yPlane;

	void render();
	void updateView(glm::mat4 viewMatrix);
	void updateProjection(glm::mat4 projectionMatrix);	
	void updatePosition(glm::vec3 cameraPosition);
	GLuint progID;
private:
	grid createSquareGrid(char* fileInput);

};

extern Island theIsland;

#endif
