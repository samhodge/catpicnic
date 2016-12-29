#include "Mesh.h"



Mesh::Mesh(){
  vbo_vertices=0;
  vbo_normals=0;
  vbo_texcoords=0;
  vbo_tangents=0;
  ibo_elements=0;
  object2world=glm::mat4(1);
}
Mesh::~Mesh() {
  if (vbo_vertices != 0)
    glDeleteBuffers(1, &vbo_vertices);
  if (vbo_normals != 0)
    glDeleteBuffers(1, &vbo_normals);
  if (vbo_texcoords != 0)
    glDeleteBuffers(1, &vbo_texcoords);
  if (vbo_tangents != 0)
    glDeleteBuffers(1, &vbo_tangents);
  if (ibo_elements != 0)
    glDeleteBuffers(1, &ibo_elements);
}

/**
 * Express surface tangent in object space
 * http://www.terathon.com/code/tangent.html
 * http://www.3dkingdoms.com/weekly/weekly.php?a=37
 */
void compute_tangents() {
  tangents.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));

  for (int i = 0; i < elements.size(); i+=3) {
    int i1 = elements.at(i);
    int i2 = elements.at(i+1);
    int i3 = elements.at(i+2);
    glm::vec3 p1(vertices.at(i1));
    glm::vec3 p2(vertices.at(i2));
    glm::vec3 p3(vertices.at(i3));
    glm::vec2 uv1 = texcoords.at(i1);
    glm::vec2 uv2 = texcoords.at(i2);
    glm::vec2 uv3 = texcoords.at(i3);

    glm::vec3 p1p2 = p2 - p1;
    glm::vec3 p1p3 = p3 - p1;
    glm::vec2 uv1uv2 = uv2 - uv1;
    glm::vec2 uv1uv3 = uv3 - uv1;

    float c = uv1uv2.s * uv1uv3.t - uv1uv3.s * uv1uv2.t;
    if (c != 0) {
      float mul = 1.0 / c;
      glm::vec3 tangent = (p1p2 * uv1uv3.t - p1p3 * uv1uv2.t) * mul;
      tangents.at(i1) = tangents.at(i2) = tangents.at(i3) = glm::normalize(tangent);
    }
  }
}

/**
 * Store object vertices, normals and/or elements in graphic card
 * buffers
 */
void Mesh::upload() {
  if (this->vertices.size() > 0) {
    glGenBuffers(1, &this->vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
	   this->vertices.data(), GL_STATIC_DRAW);
  }

  if (this->normals.size() > 0) {
    glGenBuffers(1, &this->vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
	   this->normals.data(), GL_STATIC_DRAW);
  }

  if (this->texcoords.size() > 0) {
    glGenBuffers(1, &this->vbo_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
    glBufferData(GL_ARRAY_BUFFER, this->texcoords.size() * sizeof(this->texcoords[0]),
	   this->texcoords.data(), GL_STATIC_DRAW);
  }
  
  if (this->tangents.size() > 0) {
    glGenBuffers(1, &this->vbo_tangents);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
    glBufferData(GL_ARRAY_BUFFER, this->tangents.size() * sizeof(this->tangents[0]),
	   this->tangents.data(), GL_STATIC_DRAW);
  }
  
  if (this->elements.size() > 0) {
    glGenBuffers(1, &this->ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
	   this->elements.data(), GL_STATIC_DRAW);
  }
}

/**
 * Draw the object
 */
void Mesh::draw() {
  if (this->vbo_vertices != 0) {
    glEnableVertexAttribArray(attribute_v_coord);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(
      attribute_v_coord,  // attribute
      4,                  // number of elements per vertex, here (x,y,z,w)
      GL_FLOAT,           // the type of each element
      GL_FALSE,           // take our values as-is
      0,                  // no extra data between each position
      0                   // offset of first element
    );
  }

  if (this->vbo_normals != 0) {
    glEnableVertexAttribArray(attribute_v_normal);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glVertexAttribPointer(
      attribute_v_normal,  // attribute
      3,                   // number of elements per vertex, here (x,y,z)
      GL_FLOAT,            // the type of each element
      GL_FALSE,            // take our values as-is
      0,                   // no extra data between each position
      0                    // offset of first element
    );
  }

  if (this->vbo_texcoords != 0) {
    glEnableVertexAttribArray(attribute_v_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
    glVertexAttribPointer(
      attribute_v_texcoords, // attribute
      2,                     // number of elements per vertex, here (x,y)
      GL_FLOAT,              // the type of each element
      GL_FALSE,              // take our values as-is
      0,                     // no extra data between each position
      0                      // offset of first element
    );
  }

  if (this->vbo_tangents != 0) {
    glEnableVertexAttribArray(attribute_v_tangent);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
    glVertexAttribPointer(
      attribute_v_tangent, // attribute
      3,                   // number of elements per vertex, here (x,y,z)
      GL_FLOAT,            // the type of each element
      GL_FALSE,            // take our values as-is
      0,                   // no extra data between each position
      0                    // offset of first element
    );
  }
  
  /* Apply object's transformation matrix */
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(this->object2world));
  /* Transform normal vectors with transpose of inverse of upper left
     3x3 model matrix (ex-gl_NormalMatrix): */
  glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(this->object2world)));
  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
  
  /* Push each element in buffer_vertices to the vertex shader */
  if (this->ibo_elements != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
  }

  if (this->vbo_tangents != 0)
    glDisableVertexAttribArray(attribute_v_tangent);
  if (this->vbo_texcoords != 0)
    glDisableVertexAttribArray(attribute_v_texcoords);
  if (this->vbo_normals != 0)
    glDisableVertexAttribArray(attribute_v_normal);
  if (this->vbo_vertices != 0)
    glDisableVertexAttribArray(attribute_v_coord);
}
