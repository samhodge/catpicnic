#ifndef MESH_H
#define MESH_H

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
#include <vector>
class Mesh {
public:
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;
  std::vector<glm::vec3> tangents;
  std::vector<GLushort> elements;
  glm::mat4 object2world;
  Mesh();
  ~Mesh();
  /**
   * Express surface tangent in object space
   * http://www.terathon.com/code/tangent.html
   * http://www.3dkingdoms.com/weekly/weekly.php?a=37
   */
  void compute_tangents();

  /**
   * Store object vertices, normals and/or elements in graphic card
   * buffers
   */
  void upload(); 

  /**
   * Draw the object
   */
  void draw();
private:
  GLuint vbo_vertices, vbo_normals, vbo_texcoords, vbo_tangents, ibo_elements;


};

#endif