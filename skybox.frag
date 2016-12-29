

#version 150

in vec4 tex_coord;
uniform samplerCube cube_texture;

out vec4 fragColor;

				
void main( void ) {
	fragColor = texture(cube_texture,normalize(vec3(tex_coord)));
}