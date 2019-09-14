#include "skybox.h"




#include <iostream>

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_COLOUR 4
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`


/*
Setup the shaders, buffers, load the textures, get everything self contained for the renderCall
*/
Skybox::Skybox(  
    const char* posx,
    const char* posy,
    const char* posz,
    const char* negx,
    const char* negy,
    const char* negz,
	float initScale)
{
	std::cout << "bingo" << std::endl;		
	
		// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}
	
	//progID= LoadShaders("", "");

	progID= LoadShaders("skybox.vert", "skybox.frag");

	if (progID == 0){
		std::cout << "Shaders did not load for skybox" << std::endl;
		return;
	}

	create_cube_map (
  
     posx,
     posy,
     posz,
     negx,
     negy,
     negz,
		  &tex_cube);

    // Cube has 8 vertices at its corners
	float cubeVertices[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
		 -1.0f, -1.0f, 1.0f ,
          1.0f, -1.0f, 1.0f ,
          1.0f,  1.0f, 1.0f ,
         -1.0f,  1.0f, 1.0f ,
         -1.0f, -1.0f, -1.0f ,
          1.0f, -1.0f, -1.0f ,
          1.0f,  1.0f, -1.0f ,
         -1.0f,  1.0f, -1.0f  
	};

	// Colours for each vertex; red, green, blue and alpha
	float cubeNormals[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
        -1.0f, -1.0f, 1.0f ,
         1.0f, -1.0f, 1.0f ,
         1.0f,  1.0f, 1.0f ,
        -1.0f,  1.0f, 1.0f ,
        -1.0f, -1.0f, -1.0f ,
         1.0f, -1.0f, -1.0f ,
         1.0f,  1.0f, -1.0f ,
        -1.0f,  1.0f, -1.0f
	};
    
    // Texture coords for each vertex. 2 per vertex.
    float tex_coord[CUBE_NUM_VERTICES * VALS_PER_TEX] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    // Each square face is made up of two triangles
    unsigned int indices[CUBE_NUM_TRIS * 3] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };
    

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int vertLoc = glGetAttribLocation(progID, "a_vertex");
    int normLoc = glGetAttribLocation(progID, "a_normal" );
	int texLoc = glGetAttribLocation(progID, "a_tex_coord" );

	// Buffers to store position, colour and index data
	unsigned int buffer[4];
	glGenBuffers(4, buffer);

	// Set vertex position
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Normal attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);
    

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	// We don't attach this to a shader label, instead it controls how rendering is performed
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(indices), indices, GL_STATIC_DRAW);   
	
    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	viewMatrix=glm::mat4();
	projectionMatrix=glm::mat4();
	boxScale=initScale;
	}

void Skybox::render()
{
	glUseProgram(progID);
	
  glm::vec3 cameraTranslate = glm::vec3(viewMatrix[3]);
  
	glm::mat4 modelMatrix = glm::mat4();//glm::scale( glm::mat4(),glm::vec3(boxScale,boxScale,boxScale));
  //modelMatrix = glm::translate(modelMatrix,
  //    glm::vec3(-1.0f * cameraPosition[0],-1.0f * cameraPosition[1],-1.0f * cameraPosition[2]));
	if ( setUniformMatrix( progID,
                           "M_matrix",
                           glm::value_ptr(modelMatrix) ) ) {
        std::cout << "Couldnt find M_matrix in skybox shader" << std::endl;
        return;
    }

    


	glm::mat4 cameraRotation = glm::translate(viewMatrix,
		glm::vec3(1.0f*cameraTranslate[0],-1.0f*cameraTranslate[1],-1.0f*cameraTranslate[2]));
	if ( setUniformMatrix( progID,
                           "V_matrix",
                           glm::value_ptr(viewMatrix) ) ) {
        std::cout << "Couldnt find V_matrix in skybox shader" << std::endl;
        return;
    }
    if ( setUniformMatrix( progID,
                           "P_matrix",
                           glm::value_ptr(projectionMatrix) ) ) {
        std::cout << "Couldnt find P_matrix in skybox shader" << std::endl;
        return;
    }
    
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
	GLuint uniform_mytexture = glGetUniformLocation(progID, "cube_texture");
	glDepthMask (GL_FALSE);
	glUseProgram (progID);
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS * VALS_PER_VERT, GL_UNSIGNED_INT, 0);	// New call
	//glBindVertexArray (vao);
	//glDrawArrays (GL_TRIANGLES, 0, 36);
	glDepthMask (GL_TRUE);
		glBindVertexArray(0);
	//glutSwapBuffers();
	//glFlush();
	//std::cout << "whizz" << std::endl;
}
Skybox::~Skybox()
{
}
