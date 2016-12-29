

#version 150

in vec4 eye_pos;
uniform samplerCube cube_texture;

out vec4 fragColor;

				
void main( void ) {


	fragColor = 1.0*vec4(0.5f*(vec3(1.0) + normalize(vec3(eye_pos))),1.0);
    
	fragColor += texture(cube_texture,normalize(vec3(eye_pos)));
	//fragColor += vec4(0.5,0.0,0.0,0.0);
}