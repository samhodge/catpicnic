#include "Island.h"
#include <iostream>

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_TANGENT 3
#define VALS_PER_COLOUR 4
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`




//Setup the shaders, buffers, load the textures, get everything self contained for the renderCall
Island::Island (const char* heightMap,
    float scalex,
    float scaley,
    float scalez,
    float posx,
    float posy,
    float posz)
{
		// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}
	
	//progID= LoadShaders("", "");

	progID= LoadShaders("island.vert", "island.frag");

	if (progID == 0){
		std::cout << "Shaders did not load for island" << std::endl;
		return;
	}



    // Cube has 8 vertices at its corners
  float cubeVertices[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
         -1.0f, -2.0f, 1.0f ,
          1.0f, -2.0f, 1.0f ,
          1.0f,  0.0f, 1.0f ,
         -1.0f,  0.0f, 1.0f ,
         -1.0f, -2.0f, -1.0f ,
          1.0f, -2.0f, -1.0f ,
          1.0f,  0.0f, -1.0f ,
         -1.0f,  0.0f, -1.0f  
  };

	// Colours for each vertex; red, green, blue and alpha
	float cubeNormals[ CUBE_NUM_VERTICES*VALS_PER_VERT ] = {
        -0.0f, -1.0f, 0.0f ,
         0.0f, -1.0f, 0.0f ,
         0.0f,  1.0f, 0.0f ,
        -0.0f,  1.0f, 0.0f ,
        -0.0f, 1.0f, -0.0f ,
         0.0f, 1.0f, -0.0f ,
         0.0f,  1.0f, -0.0f ,
        -0.0f,  1.0f, -0.0f
	};
    
    // Texture coords for each vertex. 2 per vertex.
    float tex_coord[CUBE_NUM_VERTICES * VALS_PER_TEX] = {
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    float tangents[CUBE_NUM_VERTICES * VALS_PER_TANGENT] = {
      //incorrect but only concerned with the top face which is OK
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0
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

  int tangentLoc = glGetAttribLocation(progID, "a_tangent" );

	// Buffers to store position, colour and index data
	unsigned int buffer[5];
	glGenBuffers(5, buffer);

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
   
  // Texture attributes
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tangents), tangents, GL_STATIC_DRAW);
  glEnableVertexAttribArray(tangentLoc);
  glVertexAttribPointer(tangentLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);    

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	// We don't attach this to a shader label, instead it controls how rendering is performed
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(indices), indices, GL_STATIC_DRAW);   
	
    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	viewMatrix=glm::mat4();
	projectionMatrix=glm::mat4();
	boxScale=scalex;
  yPlane=posy;
  timer=0.1;
	}

void Island::render()
{
	glUseProgram(progID);
	
  glm::vec3 cameraTranslate = glm::vec3(viewMatrix[3]);
  
	glm::mat4 modelMatrix = glm::scale( glm::mat4(),glm::vec3(boxScale,boxScale,boxScale));
  modelMatrix = glm::translate(glm::mat4(),glm::vec3(0.0,yPlane,0.0))*modelMatrix;

  GLint loc = glGetUniformLocation(progID, "intime");
  if (loc != -1)
  {
    //std::cout << "update" << std::endl;
   glUniform1f(loc, timer);
  }
  
#ifdef DEBUG  
      std::cout << "Position Camera:" << cameraPosition.x << "," <<  cameraPosition.y << "," << cameraPosition.z << "," << std::endl;    
#endif
  
    GLint loc2 = glGetUniformLocation(progID, "cam_pos");
  if (loc2 != -1)
  {
    //std::cout << "update" << std::endl;
   glUniform3f(loc2, cameraPosition.x,cameraPosition.y,cameraPosition.z);
  }
  timer += 0.01;
  //modelMatrix = glm::translate(modelMatrix,
  //    glm::vec3(-1.0f * cameraPosition[0],-1.0f * cameraPosition[1],-1.0f * cameraPosition[2]));
	if ( setUniformMatrix( progID,
                           "M_matrix",
                           glm::value_ptr(modelMatrix) ) ) {
        std::cout << "Couldnt find M_matrix in skybox shader" << std::endl;
        return;
    }

    /*
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  if ( setUniformMatrix3( progID,
                           "m_3x3_inv_transp",
                           glm::value_ptr(m_3x3_inv_transp) ) ) {
        std::cout << "Couldnt find m_3x3_inv_transp in ocean shader" << std::endl;
        return;
    }
    */

	glm::mat4 cameraRotation = glm::translate(viewMatrix,
		glm::vec3(1.0f*cameraTranslate[0],-1.0f*cameraTranslate[1],-1.0f*cameraTranslate[2]));
	if ( setUniformMatrix( progID,
                           "V_matrix",
                           glm::value_ptr(viewMatrix) ) ) {
        std::cout << "Couldnt find V_matrix in ocean shader" << std::endl;
        return;
    }
    if ( setUniformMatrix( progID,
                           "P_matrix",
                           glm::value_ptr(projectionMatrix) ) ) {
        std::cout << "Couldnt find P_matrix in ocean shader" << std::endl;
        return;
    }
    
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
	//GLuint uniform_mytexture = glGetUniformLocation(progID, "cube_texture");
  //glProgramUniform1i(progID, uniform_mytexture , 0);
	glDepthMask (GL_TRUE);
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
Island::~Island()
{
}
