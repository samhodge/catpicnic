#include "cubemap.h"

#include <stdlib.h>
#include <string>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool load_cube_map_side (
  GLuint texture, GLenum side_target, const char* file_name) 
  {
  glBindTexture (GL_TEXTURE_CUBE_MAP, texture);

  int x, y, n;
  int force_channels = 4;
  unsigned char*  image_data = stbi_load (
    file_name, &x, &y, &n, force_channels);
  if (!image_data) {
    fprintf (stderr, "ERROR: could not load %s\n", file_name);
    return false;
  }
  // non-power-of-2 dimensions check
  if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
    fprintf (
      stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name
    );
    return true;
  }  
  glTexImage2D (
    side_target,
    0,
    GL_RGBA,
    x,
    y,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    image_data
  );
  free (image_data);
  return true;
}
  
  
void create_cube_map (
    const char* posx,
    const char* posy,
    const char* posz,
    const char* negx,
    const char* negy,
    const char* negz,
  GLuint* tex_cube
) {
  // generate a cube-map texture to hold all the sides
  glActiveTexture (GL_TEXTURE0);
  glGenTextures (1, tex_cube);
  
  // load each image and copy into a side of the cube-map texture
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, negz));
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, posz));
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, posy));
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, negy));
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, negx));
  assert (
    load_cube_map_side (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, posx));
  // format cube map texture
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
