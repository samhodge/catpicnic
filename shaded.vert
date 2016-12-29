/*********************************************************
Version 1.000

Code provided by Michael Hemsley and Anthony Dick
for the course 
COMP SCI 3014/7090 Computer Graphics
School of Computer Science
University of Adelaide

Permission is granted for anyone to copy, use, modify, or distribute this
program and accompanying programs and documents for any purpose, provided
this copyright notice is retained and prominently displayed, along with
a note saying that the original programs are available from the aforementioned 
course web page. 

The programs and documents are distributed without any warranty, express or
implied.  As the programs were written for research purposes only, they have
not been tested to the degree that would be advisable in any important
application.  All use of these programs is entirely at the user's own risk.
*********************************************************/

#version 150

// manyAttributes.vp
// This shader passes on colour values to be interpolated by the fixed functionality
// 

//Model, view, perspective matrix baked down into one uniform 4x4
uniform mat4 M_matrix;
uniform mat4 V_matrix;
uniform mat4 P_matrix;
uniform mat3 N_matrix;

in vec3 a_color;
in vec3 a_vertex;	
in vec2 a_tex_coord;
in vec3 a_normal;
in vec4 a_tangent;

// This colour attribute will be sent out per-vertex and the fixed-functionality
// interpolates the colour (smooths the colour between) with connected vertices in the triangle
out vec2 st;
out vec3 color;
out vec3 normal;
out vec4 vertex;
out vec3 tangent;
out vec3 lightDir;

void main(void) {

    st = a_tex_coord;
	vertex = V_matrix * M_matrix * vec4(a_vertex, 1.0);
	gl_Position = P_matrix*V_matrix*M_matrix*vec4(a_vertex, 1.0);
	color = a_color;
	normal = normalize(N_matrix * a_normal);
	tangent = normalize(N_matrix * vec3(a_tangent));
	lightDir = N_matrix*normalize(vec3(0.1,-0.8,0.2));
}
