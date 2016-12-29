#ifndef CUBEMAP_H
#define CUBEMAP_H
#include <stdlib.h>
#include <string>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
bool load_cube_map_side ( GLuint texture, GLenum side_target, const char* file_name) ;
  
void create_cube_map (
    const char* posx,
    const char* posy,
    const char* posz,
    const char* negx,
    const char* negy,
    const char* negz,
  GLuint* tex_cube
) ;

#endif