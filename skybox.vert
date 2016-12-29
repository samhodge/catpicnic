
#version 150

in vec3 a_vertex, a_normal, a_tex_coord;
uniform mat4 M_matrix, P_matrix, V_matrix;

out vec4 tex_coord;

void main (void) {
  gl_Position = P_matrix * V_matrix * M_matrix * vec4 (a_vertex, 1.0);

  tex_coord = vec4 (a_vertex, 1.0);
}