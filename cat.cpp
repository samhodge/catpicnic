//----------------------------------------------------------------------------//
// viewer.cpp                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "cat.h"
#include "tick.h"
#include "tga.h"



//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//




#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 

Cat::Cat(const char* in_skeleton,
                const char* in_mesh,
                const char* in_walk_F_anim,
                const char* in_walk_L_anim,
                const char* in_walk_R_anim,
                const char* in_material,
                const char* in_tex_albedo,
                float in_cycle_duration,
                float in_scale,
                glm::mat4 in_location)
{
  m_lastTick = 0;
  m_bPaused = false;
  m_scale = 1.0f;
  m_blendTime = 0.1111111111111 * 1.0f;
  m_lodLevel = 1.0f;
  m_vertexCount = 0;
  m_faceCount = 0;
  //const std::string model_name="default";
  m_calCoreModel=NULL;
  m_calModel=NULL;
  onEmission = 0.0f;
  onPoint = 1.0f;
  onDirectional = 0.0f;
  wireframeToggle = 0.0f;
  skeleton=std::string(in_skeleton);
  mesh=std::string(in_mesh);
  walk_anim_F=std::string(in_walk_F_anim);
  walk_anim_L=std::string(in_walk_L_anim);
  walk_anim_R=std::string(in_walk_R_anim);
  material=std::string(in_material);
  tex_map=std::string(in_tex_albedo);
  scale=in_scale;
  cycle_duration=in_cycle_duration;
  location=in_location;
  Cat::onCreate();
  change = 0.0;
  int i;
  for(i=0;i<NUM_AVERAGES;i++)
  {
    direction_average[i]=0.0;
  }

}




//----------------------------------------------------------------------------//
// Load and create a texture from a given file                                //
//----------------------------------------------------------------------------//

GLuint Cat::loadTexture(const std::string& strFilename)
{
  glUseProgram(id);
  GLuint textureId=0;
  if(stricmp(strrchr(strFilename.c_str(),'.'),".raw")==0)
  {

     // open the texture file
     std::ifstream file;
     file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
     if(!file)
     {
       std::cerr << "Texture file '" << strFilename << "' not found." << std::endl;
       return 0;
     }

     // load the dimension of the texture
     int width;
     file.read((char *)&width, 4);
     int height;
     file.read((char *)&height, 4);
     int depth;
     file.read((char *)&depth, 4);

     // allocate a temporary buffer to load the texture to
     unsigned char *pBuffer;
     pBuffer = new unsigned char[2 * width * height * depth];
     if(pBuffer == 0)
     {
       std::cerr << "Memory allocation for texture '" << strFilename << "' failed." << std::endl;
       return 0;
     }

     // load the texture
     file.read((char *)pBuffer, width * height * depth);

     // explicitely close the file
     file.close();

     // flip texture around y-axis (-> opengl-style)
     int y;
     for(y = 0; y < height; y++)
     {
       memcpy(&pBuffer[(height + y) * width * depth], &pBuffer[(height - y - 1) * width * depth], width * depth);
     }
     
     // generate texture
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     glGenTextures(1, &textureId);
     glBindTexture(GL_TEXTURE_2D, textureId);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     
	 glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[width * height * depth]);
     // free the allocated memory
	glGenerateMipmap(GL_TEXTURE_2D); 
     delete [] pBuffer;  
  }
  else if(stricmp(strrchr(strFilename.c_str(),'.'),".tga")==0)  
  {
	  CTga *Tga;
	  Tga = new CTga();
	  if(Tga->ReadFile(strFilename.c_str())==0)
	  {
		  Tga->Release();
		  return 0;
	  }

	  if(Tga->Bpp()!=32)
	  {
		  Tga->Release();
		  return 0;
	  }

     //Flip texture
     int width = Tga->GetSizeX();
     int height = Tga->GetSizeY();
     int depth = Tga->Bpp() / 8;

     char* texData = new char[width * height * depth];

     for (int y = 0; y < height; ++y)
     {
       memcpy(&texData[y * width * depth], 
              &((char*)Tga->GetPointer())[(height - y - 1) * width * depth], width * depth);
     }
    

     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     glGenTextures(1, &textureId);

     glBindTexture(GL_TEXTURE_2D, textureId);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA , GL_UNSIGNED_BYTE, texData );
	glGenerateMipmap(GL_TEXTURE_2D);
	  Tga->Release();

     delete [] texData;

  }


  return textureId;
}

//----------------------------------------------------------------------------//
// Create the viewer                                                          //
//----------------------------------------------------------------------------//
bool Cat::createShaders(const char* vert, const char* frag)
{
      // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }
  
  id= LoadShaders( vert , frag );
    
  
  if (id == 0){
    std::cout << "Shaders did not load for cat program" << std::endl;
    return false;
  }
  return true;
}
bool Cat::onCreate()
{
  //TODO put stuff in place of parsing file here

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc., so this is the only
  // thing we can do here.

//std::cout << "Stuff -1" << std::endl;
    if( Cat::onInitModel() == false)
  {

    CalError::printLastError();
    return false;
  }
  //std::cout << "Stuff 0" << std::endl;
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel->createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
  }
  //std::cout << "Stuff 1" << std::endl;

   m_calModel=new CalModel(m_calCoreModel);
  // create the model instance from the loaded core model
  if(m_calModel == NULL)
  {
    CalError::printLastError();
    return false;
  }

  if(Cat::createShaders("shaded.vert","shaded.frag") == false)
  {
  std::cout << "Shaders did not load" << std::endl;
    return false;
  }
  if(Cat::onInitShading() == false)
  {
    std::cout << "Error setting up shading on cat" << std::endl;
    exit(1);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------//
// Handle an idle event                                                       //
//----------------------------------------------------------------------------//

void Cat::onIdle()
{
  // get the current tick value
  unsigned int tick;
  tick = Tick::getTick();

  // calculate the amount of elapsed seconds
  float elapsedSeconds;
  elapsedSeconds = (float)(tick - m_lastTick) / 1000.0f;

  /*
  // update the model if not paused
  if(!m_bPaused)
  {
    // check if the time has come to blend to the next animation
    if(m_calCoreModel->getCoreAnimationCount() > 1)
    {
      m_leftAnimationTime -= elapsedSeconds;
      if(m_leftAnimationTime <= m_blendTime)
      {
        // get the next animation
        m_currentAnimationId = (m_currentAnimationId + 1) % m_calCoreModel->getCoreAnimationCount();

        // fade in the new animation
        m_calModel->getMixer()->executeAction(m_currentAnimationId, m_leftAnimationTime, m_blendTime);

        // adjust the animation time left until next animation flip
        m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime;
      }
    }

    m_calModel->update(elapsedSeconds);
    //m_catSteer->update();
  }
  */
  
  float left_blend = 0.0;
  float forward_blend = 0.0;
  float right_blend = 0.0;
  float average = direction;
  int i;
    for(i=1;i<NUM_AVERAGES;i++)
  {
    average += direction_average[i-1];
    direction_average[i-1]=direction_average[i];
  }
  direction_average[NUM_AVERAGES-1]=direction;

  average = average / (float)NUM_AVERAGES;
  change = average;

  if (change < 0.0 )
  {
    right_blend = 0.0;
    left_blend = -1.0 * change;
    forward_blend = 1.0 - left_blend;
  }else{
    left_blend = 0.0;
    right_blend = change;
    forward_blend = 1.0 - right_blend;
  }
  if (left_blend > 1.0){ left_blend = 1.0; }
  if (left_blend < 0.0){ left_blend = 0.0; }
  if (right_blend > 1.0){ right_blend = 1.0; }
  if (right_blend < 0.0){ right_blend = 0.0; }
  if (forward_blend > 1.0){ forward_blend = 1.0;}
  if (forward_blend < 0.0){ forward_blend = 0.0; }
  //std::cout << "L: " << left_blend << " R: " << right_blend << " F: " << forward_blend << std::endl;

#ifdef DEBUG  
  std::cout << "L: " << left_blend << " R: " << right_blend << " F: " << forward_blend << std::endl;
#endif
    m_calModel->getMixer()->blendCycle(id_anim_F, forward_blend, 0.0);
    m_calModel->getMixer()->blendCycle(id_anim_L, left_blend, 0.0);
    m_calModel->getMixer()->blendCycle(id_anim_R, right_blend, 0.0);
  m_calModel->update(elapsedSeconds);

  // current tick will be last tick next round
  m_lastTick = tick;


  // update the screen
  glutPostRedisplay();
}

//----------------------------------------------------------------------------//
// Initialize the demo                                                        //
//----------------------------------------------------------------------------//

bool Cat::onInitShading()
{

	Cat::initMaterialLight();
	 // load all textures and store the opengl texture id in the corresponding map in the material
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
    {
      // get the filename of the texture
      std::string strFilename;
      strFilename = pCoreMaterial->getMapFilename(mapId);

      // load the texture from the file
      tex_cube = loadTexture(strFilename);
      
      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)(void*)&tex_cube);
      
    }
  }

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++)
  {
    m_calModel->attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel->setMaterialSet(0);

  // set initial animation state
  if(m_calCoreModel->getCoreAnimationCount() > 0)
  {
    m_currentAnimationId = 0;
    m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime;
    if(m_calCoreModel->getCoreAnimationCount() > 1)
    {
      m_calModel->getMixer()->executeAction(m_currentAnimationId, 0.0f, m_blendTime);
    }
    else
    {
      m_calModel->getMixer()->blendCycle(m_currentAnimationId, 1.0f, 0.0f);
    }
  }
  else
  {
    m_currentAnimationId = -1;
    m_leftAnimationTime = -1.0f;
  }

  // we're done
  std::cout << "Initialization done." << std::endl;
  std::cout << std::endl;

  m_lastTick = Tick::getTick();

  return true;
}






//----------------------------------------------------------------------------//
// Shut the viewer down                                                       //
//----------------------------------------------------------------------------//

Cat::~Cat()
{
  // destroy model instance
  delete m_calModel;

  // destroy core model instance
  delete m_calCoreModel;

}


//----------------------------------------------------------------------------//
// Parse the configuration file and load the whole model                      //
//----------------------------------------------------------------------------//

bool Cat::onInitModel()
{


  // create a core model instance
  m_calCoreModel=new CalCoreModel(std::string("default"));
    
  if(m_calCoreModel == NULL)
  {
    CalError::printLastError();
    return false;
  }

  std::cout << "Loading skeleton '" << skeleton << "'..." << std::endl;
  if(!m_calCoreModel->loadCoreSkeleton(skeleton))
      {
        CalError::printLastError();
        return false;
      }

  std::cout << "Loading animation walk forward '" << walk_anim_F << "'..." << std::endl;

  id_anim_F = m_calCoreModel->loadCoreAnimation(walk_anim_F);
  if(id_anim_F == -1)
  {
    CalError::printLastError();
    return false;
  }
  
  std::cout << "Loading animation walk left '" << walk_anim_L << "'..." << std::endl;

  id_anim_L = m_calCoreModel->loadCoreAnimation(walk_anim_L);
  if(id_anim_L == -1)
  {
    CalError::printLastError();
    return false;
  }

    std::cout << "Loading animation walk right '" << walk_anim_R << "'..." << std::endl;

  id_anim_R = m_calCoreModel->loadCoreAnimation(walk_anim_R);
  if(id_anim_R == -1)
  {
    CalError::printLastError();
    return false;
  }
   

        // load core mesh
      std::cout << "Loading mesh '" << mesh << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMesh(mesh) == -1)
      {
        CalError::printLastError();
        return false;
      }



  std::cout << "Loading material '" << material << "'..." << std::endl;
  if(m_calCoreModel->loadCoreMaterial(material) == -1)
  {
    CalError::printLastError();
    return false;
  }



  return true;
}

bool Cat::initMaterialLight(){
  glUseProgram(id);

	GLuint lightposHandle = glGetUniformLocation(id, "point_light_pos");
	if (lightposHandle == -1) {
        fprintf(stderr, "Error: missing light pos\n");
		//exit(1);
    }
    
	// Update the light position
	float lightPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
    glUniform4fv(lightposHandle, 1, lightPos); 

    // Uniform lighting variables
    GLuint lightambientHandle = glGetUniformLocation(id, "point_light_ambient");
	GLuint lightdiffuseHandle = glGetUniformLocation(id, "point_light_diffuse");
	GLuint lightspecularHandle = glGetUniformLocation(id, "point_light_specular");
	if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables for point light\n");
		//return false;
    }
    
	float lightambient[3] = { 1.0f, 1.0f, 1.0f };	// ambient light components
    float lightdiffuse[3] = { 1.0f, 1.0f, 1.0f };	// diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular light components

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);    
	
	int directionlightposHandle = glGetUniformLocation(id, "directional_light_direction");
	if (directionlightposHandle == -1) {
        fprintf(stderr, "Error: missing light dir\n");
		exit(1);
    }
    
	// Update the light position
	float lightDirection[3] = { 0.0f, -1.0f, 0.0f };	
    glUniform3fv(directionlightposHandle, 1, lightDirection); 

    // Uniform lighting variables
    int dirlightambientHandle = glGetUniformLocation(id, "directional_light_ambient");
	int dirlightdiffuseHandle = glGetUniformLocation(id, "directional_light_diffuse");
	int dirlightspecularHandle = glGetUniformLocation(id, "directional_light_specular");
	if ( dirlightambientHandle == -1 ||
         dirlightdiffuseHandle == -1 ||
         dirlightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables for directional light\n");
		//return false;
    }
    
	float dirlightambient[3] = { 1.0f, 1.0f, 1.0f };	// ambient light components
    float dirlightdiffuse[3] = { 0.7f, 0.7f, 1.0f };	// diffuse light components
    float dirlightspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular light components

	glUniform3fv(dirlightambientHandle, 1, dirlightambient);
	glUniform3fv(dirlightdiffuseHandle, 1, dirlightdiffuse);
	glUniform3fv(dirlightspecularHandle, 1, dirlightspecular);
		
    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(id, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(id, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(id, "mtl_specular");
	int mtlshininessHandle = glGetUniformLocation(id, "shininess");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1 ||
         mtlshininessHandle == -1 ) {
        fprintf(stderr, "Error: can't find material uniform variables\n");
		//exit(1);
    }
    
	float mtlambient[3] = { 0.1f, 0.1f, 0.1f };	// ambient material
    float mtldiffuse[3] = { 1.0f, 1.0f, 1.0f };	// diffuse material
    float mtlspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular material
    float mtlshininess = 32.0f;

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);
	glUniform1f(mtlshininessHandle,mtlshininess);
		
	int onDirectionalHandle = glGetUniformLocation(id, "on_directional");
	int onPointHandle = glGetUniformLocation(id, "on_point");   
	int onEmissionHandle = glGetUniformLocation(id, "on_emission"); 
        int onWireHandle = glGetUniformLocation(id, "on_wireframe");
	

	glUniform1f(onDirectionalHandle,onDirectional);
	glUniform1f(onPointHandle,onPoint);
	glUniform1f(onEmissionHandle,onEmission);
        glUniform1f(onWireHandle,wireframeToggle);
	glGenVertexArrays(1, &vertexVaoShapeArray);		
	glBindVertexArray(vertexVaoShapeArray);
	
	glGenBuffers(NUM_BUFFERS, buffer);  
	glGenBuffers( 1, &arrayVboArray );
	glGenBuffers( 1, &arrayGnomonVboArray);
	glGenBuffers(NUM_BUFFERS, bufferGnomon); 			
	glGenVertexArrays(1, &gnomonVao);
	
	
	return true;
}




//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//



void Cat::renderModel()
{
  glUseProgram(id);

   // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(id, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(id, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(id, "mtl_specular");
	int mtlshininessHandle = glGetUniformLocation(id, "shininess");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1 ||
         mtlshininessHandle == -1 ) {
        fprintf(stderr, "Error: can't find material uniform variables d\n");
		exit(1);
    }			
	
	CalMesh* m_calMesh;
	m_calMesh = m_calModel->getMesh(0);
	CalCoreMesh* m_coreMesh;
	
	m_coreMesh = m_calMesh->getCoreMesh();
	
	CalCoreSubmesh* m_coreSubMesh;
	m_coreSubMesh = m_coreMesh->getCoreSubmesh(0);
	
	m_coreSubMesh->enableTangents(0,true);
    
    // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel->getRenderer();


  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;



	//glm::mat4 projection = Camera->getProjMtx();

	
    if ( setUniformMatrix( id,
                           "P_matrix",
                           glm::value_ptr(projectionMatrix) ) ) {
        return;
    }


  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  
  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
		//Camera->update( Input ); 
		//glm::mat4 viewMatrix;
		//viewMatrix = Camera->getViewMtx();

		// Combine Model , View and Projection together, in reverse order and send to the GPU
		//glm::mat4 model = glm::rotate(glm::mat4(),-90.0f,glm::vec3(1,0,0));
		//glm::mat4 model = m_catSteer->getTransform()*glm::rotate(glm::mat4(),-00.0f,glm::vec3(0,1,0))*glm::rotate(glm::mat4(),glm::radians(-90.0f),glm::vec3(1,0,0));
		
      // Combine Model , View and Projection together, in reverse order and send to the GPU
    glm::mat4 M = glm::scale(glm::mat4(),glm::vec3(scale,scale,scale)); 

    glm::mat4 model =  location * glm::rotate(glm::mat4(),-0.0f,glm::vec3(0.0,1.0,0.0))*glm::rotate(glm::mat4(),glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0))*M;
    
		glm::mat3 N = glm::mat3(viewMatrix*model); // model is not currently rotated so just use view
		
      //glm::mat4 viewMatrix = projection;
    /*
    std::cout << viewMatrix[0][0] << ",";
  std::cout << viewMatrix[0][1] << ",";
  std::cout << viewMatrix[0][2] << ",";
  std::cout << viewMatrix[0][3] << std::endl;
  std::cout << viewMatrix[1][0] << ",";
  std::cout << viewMatrix[1][1] << ",";
  std::cout << viewMatrix[1][2] << ",";
  std::cout << viewMatrix[1][3] << std::endl;
  std::cout << viewMatrix[2][0] << ",";
  std::cout << viewMatrix[2][1] << ",";
  std::cout << viewMatrix[2][2] << ",";
  std::cout << viewMatrix[2][3] << std::endl;
  
  std::cout << viewMatrix[3][0] << ",";
  std::cout << viewMatrix[3][1] << ",";
  std::cout << viewMatrix[3][2] << ",";
  std::cout << viewMatrix[3][3] << std::endl;
*/
/*
      std::cout << projectionMatrix[0][0] << ",";
  std::cout << projectionMatrix[0][1] << ",";
  std::cout << projectionMatrix[0][2] << ",";
  std::cout << projectionMatrix[0][3] << std::endl;
  std::cout << projectionMatrix[1][0] << ",";
  std::cout << projectionMatrix[1][1] << ",";
  std::cout << projectionMatrix[1][2] << ",";
  std::cout << projectionMatrix[1][3] << std::endl;
  std::cout << projectionMatrix[2][0] << ",";
  std::cout << projectionMatrix[2][1] << ",";
  std::cout << projectionMatrix[2][2] << ",";
  std::cout << projectionMatrix[2][3] << std::endl;
  
  std::cout << projectionMatrix[3][0] << ",";
  std::cout << projectionMatrix[3][1] << ",";
  std::cout << projectionMatrix[3][2] << ",";
  std::cout << projectionMatrix[3][3] << std::endl;
  */

  /*
      std::cout << model[0][0] << ",";
  std::cout << model[0][1] << ",";
  std::cout << model[0][2] << ",";
  std::cout << model[0][3] << std::endl;
  std::cout << model[1][0] << ",";
  std::cout << model[1][1] << ",";
  std::cout << model[1][2] << ",";
  std::cout << model[1][3] << std::endl;
  std::cout << model[2][0] << ",";
  std::cout << model[2][1] << ",";
  std::cout << model[2][2] << ",";
  std::cout << model[2][3] << std::endl;
  
  std::cout << model[3][0] << ",";
  std::cout << model[3][1] << ",";
  std::cout << model[3][2] << ",";
  std::cout << model[3][3] << std::endl;
  */
  
		// Send the updated MVP matrix to the shader
		if ( setUniformMatrix( id,
						   "M_matrix",
						   glm::value_ptr(model) ) ) {
		return;
		}
		if ( setUniformMatrix3( id,
						   "N_matrix",
						   glm::value_ptr(N) ) ) {
		return;
		}
		
		    glm::mat4 identity=glm::mat4();
//identity

    
       if ( setUniformMatrix( id,
                           "V_matrix",
                           glm::value_ptr(viewMatrix) ) ) {
        return;
    }

    int directionlightposHandle = glGetUniformLocation(id, "directional_light_direction");
  if (directionlightposHandle == -1) {
        fprintf(stderr, "Error: missing light dir\n");
    exit(1);
    }
    
  // Update the light position
  glm::vec3 lightDirection = glm::vec3(-0.4f, -1.0f, 0.2f); 
  lightDirection = N * lightDirection; 
    glUniform3fv(directionlightposHandle, 1, glm::value_ptr(lightDirection)); 
	
    if ( setUniformMatrix( id,
                           "P_matrix",
                           glm::value_ptr(projectionMatrix) ) ) {
        return;
    }


        unsigned char meshColor[4];
        GLfloat materialColor[4];

        // set the material ambient color
        pCalRenderer->getAmbientColor(&meshColor[0]);
        //materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        //glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);

			// ambient material
			float mtlambient[3] = {
			meshColor[0]/ 255.0f,
			meshColor[1]/ 255.0f,
			meshColor[2]/ 255.0f};
			
        // set the material diffuse color
        pCalRenderer->getDiffuseColor(&meshColor[0]);
        //materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);

			// ambient material
			float mtldiffuse[3] = {
			meshColor[0]/ 255.0f,
			meshColor[1]/ 255.0f,
			meshColor[2]/ 255.0f};

        // set the material specular color
        pCalRenderer->getSpecularColor(&meshColor[0]);
        //materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        //glMaterialfv(GL_FRONT, GL_SPECULAR, materialColor);

		
			// specular material
			float mtlspecular[3] = {
			meshColor[0]/ 255.0f,
			meshColor[1]/ 255.0f,
			meshColor[2]/ 255.0f};
			float mtlshininess= 22.0f;	
	
	
			glUniform3fv(mtlambientHandle, 1, mtlambient);
			glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
			glUniform3fv(mtlspecularHandle, 1, mtlspecular);
			glUniform1f(mtlshininessHandle,mtlshininess);
       
        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);
        
        std::vector<float> color;
        for ( int i = 0; i < vertexCount + 1; i++){
        	color.push_back(1.0f);
        	color.push_back(1.0f);
        	color.push_back(1.0f);
        }
        

        // get the transformed normals of the submesh
        static float meshNormals[30000][3];
        int normalCount;
        normalCount = pCalRenderer->getNormals(&meshNormals[0][0]);

        // get the texture coordinates of the submesh
        static float meshTextureCoordinates[30000][2];
        int textureCoordinateCount;
        textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

        // get the faces of the submesh
        static CalIndex meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);
	  // set the vertex and normal buffers
        //glVertexPointer(3, GL_FLOAT, 0, &meshVertices[0][0]);
        //glNormalPointer(GL_FLOAT, 0, &meshNormals[0][0]);
        
                  //get the tangent space data of the submesh
          static float meshTangents[30000][4];
          int tangentCount = pCalRenderer->getTangentSpaces(0, &meshTangents[0][0]);
	    		
			glBindVertexArray(vertexVaoShapeArray);


			// Allocate GPU memory for our vertices and copy them over
			glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertexCount*3, &meshVertices[0][0], GL_STATIC_DRAW);
	
      //std::cout << vertexCount << std::endl;
			// Do the same for our vertex colours
			glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertexCount*3, &color[0], GL_STATIC_DRAW);

			// Do the same for our heights colours
			glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*textureCoordinateCount*2,  &meshTextureCoordinates[0][0], GL_STATIC_DRAW);

			// Do the same for our heights colours
			glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normalCount*3, &meshNormals[0][0], GL_STATIC_DRAW);
			
						// Do the same for our heights colours
			glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*tangentCount*4, &meshTangents[0][0], GL_STATIC_DRAW);
		
			// Now we tell OpenGL how to interpret the data we just gave it
			// Tell OpenGL what shader variable it corresponds to
			// Tell OpenGL how it's formatted (floating point, 3 values per vertex)
			int vertLoc = glGetAttribLocation(id, "a_vertex");
			glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
			glEnableVertexAttribArray(vertLoc);
			glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
			// Do the same for the vertex colours
			int colourLoc = glGetAttribLocation(id, "a_color");
			glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
			glEnableVertexAttribArray(colourLoc);
			glVertexAttribPointer(colourLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
			// Do the same for the height floats
			int tex_loc = glGetAttribLocation(id, "a_tex_coord");
			glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
			glEnableVertexAttribArray(tex_loc);
			glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

			// Do the same for the height floats
			int normal_loc = glGetAttribLocation(id, "a_normal");
			glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
			glEnableVertexAttribArray(normal_loc);
			glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        	// Do the same for the height floats
			int tangent_loc = glGetAttribLocation(id, "a_tangent");
			glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
			glEnableVertexAttribArray(tangent_loc);
			glVertexAttribPointer(tangent_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
			
        // set the texture coordinate buffer and state if necessary
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
        {
            glEnable(GL_TEXTURE_2D);

            glActiveTexture(GL_TEXTURE0);
  
            GLuint uniform_mytexture = glGetUniformLocation(id, "texMap");
            if (uniform_mytexture == -1 ){
              std::cout << "texMap : not found in shader for cat" << std::endl;
            }
            
            glUniform1i(id, uniform_mytexture);
            glBindTexture(GL_TEXTURE_2D, tex_cube);
          //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          //glEnable(GL_COLOR_MATERIAL);

          // set the texture id we stored in the map user data
          //glBindTexture(GL_TEXTURE_2D, *(static_cast<GLuint*> (pCalRenderer->getMapUserData(0))));
          // set the texture coordinate buffer
          //glTexCoordPointer(2, GL_FLOAT, 0, &meshTextureCoordinates[0][0]);
          //glColor3f(1.0f, 1.0f, 1.0f);
        }
    
        
        			//Setup indices 
                  
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, arrayVboArray );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CalIndex)*faceCount*3, &meshFaces[0][0], GL_STATIC_DRAW );
			glBindVertexArray(vertexVaoShapeArray);
        if(sizeof(CalIndex)==2){
			glDrawElements( GL_TRIANGLES,  faceCount*3, GL_UNSIGNED_SHORT, NULL);
		  }else{
			glDrawElements( GL_TRIANGLES,  faceCount*3, GL_UNSIGNED_INT, NULL);
			}
	    // disable the texture coordinate state if necessary
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
        {
          glDisable(GL_COLOR_MATERIAL);
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);
          glDisable(GL_TEXTURE_2D);
        }
        glBindVertexArray(0);	// Un-bind the VAO
      }
    }
    	
	    
	//std::cout << "rendering model" << std::endl;
  }
  // reset wireframe mode if necessary

  // end the rendering
  pCalRenderer->endRendering();
  return;
}

