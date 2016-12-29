
#version 150

in vec3 a_vertex, a_normal, a_tangent;
in vec2 a_tex_coord;
//uniform mat3 m_3x3_inv_transp;
uniform float intime;
uniform mat4 M_matrix, P_matrix, V_matrix;
uniform vec3 cam_pos;

out vec4 tex_coord;
out vec2 uv_coord;
out vec4 position;
out vec4 direction;
out vec4 ori;
out vec4 light;
out float outtime;
//out mat3 localSurface2World; // mapping from local surface coordinates to world coordinates
//out mat3 World2localSurface; 

void main (void) {
  gl_Position = P_matrix * V_matrix * M_matrix * vec4 (a_vertex, 1.0);
  ori =  vec4(cam_pos,1.0);
  position  =    M_matrix * vec4 (a_vertex, 1.0);
  direction =  V_matrix * vec4(0.0,0.0,-1.0,0.0);
  light = V_matrix*vec4(normalize(vec3(0.2,-1.0,0.0)),1.0);
  //tex_coord = vec4 (a_vertex, 1.0);
  uv_coord = a_tex_coord;
  outtime = intime;
  /*
    // the signs and whether tangent is in localSurface2View[1] or
  // localSurface2View[0] depends on the tangent attribute, texture
  // coordinates, and the encoding of the normal map
  localSurface2World[0] = normalize(vec3(M_matrix * vec4(a_tangent, 0.0)));
  localSurface2World[2] = normalize(m_3x3_inv_transp * a_normal);
  localSurface2World[1] = normalize(cross(localSurface2World[2], localSurface2World[0]));
   //kludge to get around early shader versions not having transpose
   World2localSurface[0][0]=localSurface2World[0][0];
   World2localSurface[0][1]=localSurface2World[1][0];
   World2localSurface[0][2]=localSurface2World[2][0];

   World2localSurface[1][0]=localSurface2World[0][1];
   World2localSurface[1][1]=localSurface2World[1][1];
   World2localSurface[1][2]=localSurface2World[2][1];

   World2localSurface[2][0]=localSurface2World[0][2];
   World2localSurface[2][1]=localSurface2World[1][2];
   World2localSurface[2][2]=localSurface2World[2][2];
   */
}
