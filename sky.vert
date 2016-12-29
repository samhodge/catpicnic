

#version 150


uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

in vec3 a_vertex;
out vec4 eye_pos;

void main( void ) {
	
	gl_Position = projection_matrix * modelview_matrix * vec4( a_vertex, 1.0 );
	eye_pos=vec4(a_vertex,1.0);
}