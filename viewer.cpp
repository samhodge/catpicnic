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

#include "viewer.h"
#include "tick.h"
#include "tga.h"
#include "wander.h"

//----------------------------------------------------------------------------//
// The one and only Viewer instance                                           //
//----------------------------------------------------------------------------//

Viewer theViewer;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

#include "glm/gtc/matrix_transform.hpp"


#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 

ViewCamera::ViewCamera( glm::vec3 eye, glm::vec3 at, glm::vec3 up , float fov, float aspect, float near, float far)
{
    initEye = eye;
    initAt = at;
    initUp = up;
	initFov = fov;
	initAspect = aspect;
	initNear = near;
	initFar = far;
    reset();
}

ViewCamera::~ViewCamera(){}

/**
 Assumes the current matrix has been calculated (usually in 
 update() or reset())
*/
const glm::mat4 ViewCamera::getViewMtx() const
{
    return viewMtx;
}

/**
 Assumes the current matrix has been calculated (usually in 
 update() or reset())
*/
const glm::mat4 ViewCamera::getProjMtx() const
{
    return viewProj;
}

/**
 get the zoom
*/
const float ViewCamera::getZoom() const
{
    return zoom;
}


/**
 Resets the view matrix to the value the camera was 
 initialised with
*/
void ViewCamera::reset()
{
	//std::cout << "Init Camera" << std::endl;
    viewMtx = glm::lookAt(initEye, initAt, initUp);
    viewProj = glm::perspective(
	initFov,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
	initAspect, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
	initNear,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
	initFar       // Far clipping plane. Keep as little as possible.
	);
	zoom = initFov;
	std::cout << "Eye: " << initEye[0] << "," << initEye[1] << "," << initEye[2] << std::endl;
	
	std::cout << "At: " << initAt[0] << "," << initAt[1] << "," << initAt[2] << std::endl;
	
	std::cout << "Up: " << initUp[0] << "," << initUp[1] << "," << initUp[2] << std::endl;	
	
	std::cout << "Fov: " << initFov << std::endl;	
	
	
	std::cout << "Aspect: " << initAspect << std::endl;	
	
	
	std::cout << "Near: " << initNear << std::endl;	
	
	std::cout << "Far: " << initFar << std::endl;	
	zoom = initFov;
	aspect = initAspect;
}

ObjectViewer::ObjectViewer(glm::vec3 eye, glm::vec3 at, glm::vec3 up, float fov, float aspect, float near, float far )
: ViewCamera(eye, at, up, fov, aspect, near, far )
{}
/*
void ObjectViewer::update( InputState input ) 
{
    if ( input.lMousePressed )
    {
        // The first 3 rows of the view matrix are the camera x, y, z axes
        // in world coordinate space. (see lecture 6)
        glm::vec3 eyeX( viewMtx[0][0], viewMtx[1][0], viewMtx[2][0] );
        glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
        glm::vec3 eyeZ( viewMtx[0][2], viewMtx[1][2], viewMtx[2][2] );

        float yRot = input.deltaX;
        float xRot = input.deltaY;

        // Rotate about the eye's y axis.
        if ( yRot != 0 )
        {
            float sinY = sin(DEG2RAD(yRot));
            float cosY = cos(DEG2RAD(yRot));

            glm::vec3 tmpX = eyeX;
            eyeX = cosY*tmpX + sinY*eyeZ;
            eyeZ = -sinY*tmpX + cosY*eyeZ;
        }
        // Rotate about the eye's x axis.
        if ( xRot != 0 )
        {
            float sinX = sin(DEG2RAD(xRot));
            float cosX = cos(DEG2RAD(xRot));

            glm::vec3 tmpY = eyeY;
            eyeY = cosX*tmpY - sinX*eyeZ;
            eyeZ = sinX*tmpY + cosX*eyeZ;
        }

        // Update the view matrix with new eye axes.
        viewMtx[0][0] = eyeX[0];
        viewMtx[1][0] = eyeX[1];
        viewMtx[2][0] = eyeX[2];
        
        viewMtx[0][1] = eyeY[0];
        viewMtx[1][1] = eyeY[1];
        viewMtx[2][1] = eyeY[2];
        
        viewMtx[0][2] = eyeZ[0];
        viewMtx[1][2] = eyeZ[1];
        viewMtx[2][2] = eyeZ[2];
    }
    if (input.rMousePressed){
    	float zoomDiff = input.deltaY;
    	zoom += zoomDiff*0.1;
		viewProj = glm::perspective(
		zoom,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		aspect, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		initNear,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		initFar       // Far clipping plane. Keep as little as possible.
		);    	
    }
}
*/
void ViewCamera::updateAspect( float new_aspect ) 
{
	aspect = new_aspect;
		viewProj = glm::perspective(
		glm::radians(zoom),         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		aspect, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		initNear,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		initFar       // Far clipping plane. Keep as little as possible.
		);	
}

ObjectViewer* Viewer::getInitialCamera(){
    glm::vec4 eye(0.0f, 0.0f, 1.0f*m_distance,1.0f);
    glm::vec3 at(0.0f,0.0f,0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    glm::mat4 eyeRotationX;
    eyeRotationX = glm::rotate(eyeRotationX,m_tiltAngle,glm::vec3(1.0f,0.0f,0.0f));
    
    glm::mat4 eyeRotationY;
	eyeRotationY = glm::rotate(eyeRotationY,m_twistAngle,glm::vec3(0.0f,1.0f,0.0f));
     
    eye = eyeRotationY*eyeRotationX*eye; 
    float aspect = (float)m_width/(float)m_height;
    //float camera_h_fov=35.0;

    ObjectViewer *ObjCam = new ObjectViewer( glm::vec3(eye), at, up, camera_h_fov, aspect, near_clip,far_clip);
	//ObjCam->updateAspect(aspect);
	
	eyeRotationY = ObjCam->getViewMtx();
    std::cout << eyeRotationY[0][0] << ",";
	std::cout << eyeRotationY[0][1] << ",";
	std::cout << eyeRotationY[0][2] << ",";
	std::cout << eyeRotationY[0][3] << std::endl;
	std::cout << eyeRotationY[1][0] << ",";
	std::cout << eyeRotationY[1][1] << ",";
	std::cout << eyeRotationY[1][2] << ",";
	std::cout << eyeRotationY[1][3] << std::endl;
	std::cout << eyeRotationY[2][0] << ",";
	std::cout << eyeRotationY[2][1] << ",";
	std::cout << eyeRotationY[2][2] << ",";
	std::cout << eyeRotationY[2][3] << std::endl;
	
	std::cout << eyeRotationY[3][0] << ",";
	std::cout << eyeRotationY[3][1] << ",";
	std::cout << eyeRotationY[3][2] << ",";
	std::cout << eyeRotationY[3][3] << std::endl;
	
	eyeRotationY = ObjCam->getProjMtx();
	/*
    std::cout << eyeRotationY[0][0] << ",";
	std::cout << eyeRotationY[0][1] << ",";
	std::cout << eyeRotationY[0][2] << ",";
	std::cout << eyeRotationY[0][3] << std::endl;
	std::cout << eyeRotationY[1][0] << ",";
	std::cout << eyeRotationY[1][1] << ",";
	std::cout << eyeRotationY[1][2] << ",";
	std::cout << eyeRotationY[1][3] << std::endl;
	std::cout << eyeRotationY[2][0] << ",";
	std::cout << eyeRotationY[2][1] << ",";
	std::cout << eyeRotationY[2][2] << ",";
	std::cout << eyeRotationY[2][3] << std::endl;
	
	std::cout << eyeRotationY[3][0] << ",";
	std::cout << eyeRotationY[3][1] << ",";
	std::cout << eyeRotationY[3][2] << ",";
	std::cout << eyeRotationY[3][3] << std::endl;	
	*/
	return ObjCam;
};




Viewer::Viewer()
{
  m_width = 640;
  m_height = 480;
  m_bFullscreen = false;
  m_mouseX = 0;
  m_mouseY = 0;
  m_tiltAngle = -45.0f;
  m_twistAngle = -45.0f;
  m_distance = 200.0f;
  m_bLeftMouseButtonDown = false;
  m_bRightMouseButtonDown = false;
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
  near_clip = 0.01f;
  far_clip = 10000.0f;
  camera_h_fov=35.0;
  m_objCam=getInitialCamera();
  Camera=m_objCam;
  m_catSteer=new SteeringCat(glm::vec3(0.0,0.0,0.0),glm::vec3(0.01,0,0.0),2.0f);
  /*
  m_skyBox=new Skybox( 
  "SanFrancisco2/posx.jpg",
  "SanFrancisco2/negx.jpg",
  "SanFrancisco2/posy.jpg",
  "SanFrancisco2/negy.jpg",
  "SanFrancisco2/posz.jpg",
  "SanFrancisco2/negz.jpg",
  10.0f
  );
  */
  
  

  //m_calCoreModel=new CalCoreModel(std::string("default"));
  //m_calModel=new CalModel(m_calCoreModel);

}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Viewer::~Viewer()
{
}

//----------------------------------------------------------------------------//
// Get the viewer caption                                                     //
//----------------------------------------------------------------------------//

std::string Viewer::getCaption()
{
  return "- Cal3D MiniViewer -";
}

//----------------------------------------------------------------------------//
// Get the fullscreen flag                                                    //
//----------------------------------------------------------------------------//

bool Viewer::getFullscreen()
{
  return m_bFullscreen;
}

//----------------------------------------------------------------------------//
// Get the window height                                                      //
//----------------------------------------------------------------------------//

int Viewer::getHeight()
{
  return m_height;
}

//----------------------------------------------------------------------------//
// Get the window width                                                       //
//----------------------------------------------------------------------------//

int Viewer::getWidth()
{
  return m_width;
}

//----------------------------------------------------------------------------//
// Load and create a texture from a given file                                //
//----------------------------------------------------------------------------//

GLuint Viewer::loadTexture(const std::string& strFilename)
{
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

bool Viewer::onCreate(int argc, char *argv[])
{
  // show some information
  std::cout << "o----------------------------------------------------------------o" << std::endl;
  std::cout << "|                    Cal3D MiniViewer 0.9                        |" << std::endl;
  std::cout << "|     (C) 2001, 2002, 2003 Bruno 'Beosil' Heidelberger           |" << std::endl;
  std::cout << "o----------------------------------------------------------------o" << std::endl;
  std::cout << "| This program is free software; you can redistribute it and/or  |" << std::endl;
  std::cout << "| modify it under the terms of the GNU General Public License as |" << std::endl;
  std::cout << "| published by the Free Software Foundation; either version 2 of |" << std::endl;
  std::cout << "| the License, or (at your option) any later version.            |" << std::endl;
  std::cout << "o----------------------------------------------------------------o" << std::endl;
  std::cout << std::endl;

  bool bModelConfiguration;
  bModelConfiguration = false;

  // parse the command line arguments
  int arg;
  for(arg = 1; arg < argc; arg++)
  {
    // check for fullscreen flag
    if(strcmp(argv[arg], "--fullscreen") == 0) m_bFullscreen = true;
    // check for window flag
    else if(strcmp(argv[arg], "--window") == 0) m_bFullscreen = false;
    // check for dimension flag
    else if((strcmp(argv[arg], "--dimension") == 0) && (argc - arg > 2))
    {
      m_width = atoi(argv[++arg]);
      m_height = atoi(argv[++arg]);
      if((m_width <= 0) || (m_height <= 0))
      {
        std::cerr << "Invalid dimension!" << std::endl;
        return false;
      }
    }
    // check for help flag
    else if(strcmp(argv[arg], "--help") == 0)
    {
      std::cerr << "Usage: " << argv[0] << " [--fullscreen] [--window] [--dimension width height] [--help] model-configuration-file" << std::endl;
      return false;
    }
    // must be the model configuration file then
    else
    {
      // parse the model configuration file
      if(!parseModelConfiguration(argv[arg])) return false;

      // set model configuration flag
      bModelConfiguration = true;
    }
  }

  // check if we have successfully loaded a model configuration
  if(!bModelConfiguration)
  {
    std::cerr << "No model configuration file given." << std::endl;
    return false;
  }

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc., so this is the only
  // thing we can do here.
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel->createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
  }

   m_calModel=new CalModel(m_calCoreModel);
  // create the model instance from the loaded core model
  if(m_calModel == NULL)
  {
    CalError::printLastError();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------//
// Handle an idle event                                                       //
//----------------------------------------------------------------------------//

void Viewer::onIdle(const unsigned int &id)
{
  // get the current tick value
  unsigned int tick;
  tick = Tick::getTick();

  // calculate the amount of elapsed seconds
  float elapsedSeconds;
  elapsedSeconds = (float)(tick - m_lastTick) / 1000.0f;

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
    m_catSteer->update();
  }

  // current tick will be last tick next round
  m_lastTick = tick;


  // update the screen
  glutPostRedisplay();
}

//----------------------------------------------------------------------------//
// Initialize the demo                                                        //
//----------------------------------------------------------------------------//

bool Viewer::onInit(const unsigned int &id)
{

	initMaterialLight(id);
	setShaderData(id);
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
      GLuint textureId;
      textureId = loadTexture(strFilename);
      tex_cube = textureId;
      
      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)(void*)&textureId);
      
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
  std::cout << "Quit the viewer by pressing 'q' or ESC" << std::endl;
  std::cout << std::endl;

  m_lastTick = Tick::getTick();

  return true;
}

//----------------------------------------------------------------------------//
// Handle a key event                                                         //
//----------------------------------------------------------------------------//

void Viewer::onKey(unsigned char key, int x, int y)
{
  switch(key)
  {
    // test for quit event
    case 27:
    case 'q':
    case 'Q':
      exit(0);
      break;
    case '-':
      m_lodLevel -= 0.002f;
      if(m_lodLevel < 0.0f) m_lodLevel = 0.0f;
      break;
    case '+':
      m_lodLevel += 0.002f;
      if(m_lodLevel > 1.0f) m_lodLevel = 1.0f;
      break;
    // test for pause event
    case ' ':
      m_bPaused = !m_bPaused;
      break;
    // test for the lod keys
    default:
      if((key >= '0') && (key <= '9'))
      {
        m_lodLevel = (key == '0') ? 1.0f : (key - '0') * 0.1f;
      }
      break;
  }

  // set the (possible) new lod level
  m_calModel->setLodLevel(m_lodLevel);
}

//----------------------------------------------------------------------------//
// Handle a mouse button down event                                           //
//----------------------------------------------------------------------------//

void Viewer::onMouseButtonDown(int button, int x, int y)
{
  // update mouse button states
  if(button == GLUT_LEFT_BUTTON)
  {
    m_bLeftMouseButtonDown = true;
  }

  if(button == GLUT_RIGHT_BUTTON)
  {
    m_bRightMouseButtonDown = true;
  }

  // update internal mouse position
  m_mouseX = x;
  m_mouseY = y;
}

//----------------------------------------------------------------------------//
// Handle a mouse button up event                                             //
//----------------------------------------------------------------------------//

void Viewer::onMouseButtonUp(int button, int x, int y)
{
  // update mouse button states
  if(button == GLUT_LEFT_BUTTON)
  {
    m_bLeftMouseButtonDown = false;
  }

  if(button == GLUT_RIGHT_BUTTON)
  {
    m_bRightMouseButtonDown = false;
  }

  // update internal mouse position
  m_mouseX = x;
  m_mouseY = y;
}

//----------------------------------------------------------------------------//
// Handle a mouse move event                                                  //
//----------------------------------------------------------------------------//

void Viewer::onMouseMove(int x, int y)
{
	/*
  // update twist/tilt angles
  if(m_bLeftMouseButtonDown)
  {
    // calculate new angles
    m_twistAngle += (float)(x - m_mouseX);
    m_tiltAngle -= (float)(y - m_mouseY);
  }

  // update distance
  if(m_bRightMouseButtonDown)
  {
    // calculate new distance
    m_distance -= (float)(y - m_mouseY) / 3.0f;
    if(m_distance < 0.0f) m_distance = 0.0f;
  }
  */
  
    if ( m_bLeftMouseButtonDown )
    {
    
  		glm::mat4 viewMtx = Camera->viewMtx;
        // The first 3 rows of the view matrix are the camera x, y, z axes
        // in world coordinate space. (see lecture 6)
        glm::vec3 eyeX( viewMtx[0][0], viewMtx[1][0], viewMtx[2][0] );
        glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
        glm::vec3 eyeZ( viewMtx[0][2], viewMtx[1][2], viewMtx[2][2] );

        float yRot = (x - m_mouseX);
        float xRot = -1.0f * (y - m_mouseY);

        // Rotate about the eye's y axis.
        if ( yRot != 0 )
        {
            float sinY = sin(DEG2RAD(yRot));
            float cosY = cos(DEG2RAD(yRot));

            glm::vec3 tmpX = eyeX;
            eyeX = cosY*tmpX + sinY*eyeZ;
            eyeZ = -sinY*tmpX + cosY*eyeZ;
        }
        // Rotate about the eye's x axis.
        if ( xRot != 0 )
        {
            float sinX = sin(DEG2RAD(xRot));
            float cosX = cos(DEG2RAD(xRot));

            glm::vec3 tmpY = eyeY;
            eyeY = cosX*tmpY - sinX*eyeZ;
            eyeZ = sinX*tmpY + cosX*eyeZ;
        }

        // Update the view matrix with new eye axes.
        viewMtx[0][0] = eyeX[0];
        viewMtx[1][0] = eyeX[1];
        viewMtx[2][0] = eyeX[2];
        
        viewMtx[0][1] = eyeY[0];
        viewMtx[1][1] = eyeY[1];
        viewMtx[2][1] = eyeY[2];
        
        viewMtx[0][2] = eyeZ[0];
        viewMtx[1][2] = eyeZ[1];
        viewMtx[2][2] = eyeZ[2];
        //glm::mat4 viewMtx = Camera->viewMtx;
        Camera->viewMtx = viewMtx;
    }
    if (m_bRightMouseButtonDown){
    	float zoomDiff = y - m_mouseY;
    	Camera->zoom += zoomDiff*0.1;
		Camera->viewProj = glm::perspective(
		Camera->zoom,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		Camera->aspect, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		Camera->initNear,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		Camera->initFar       // Far clipping plane. Keep as little as possible.
		);    	
    }  

  // update internal mouse position
  m_mouseX = x;
  m_mouseY = y;
}

//----------------------------------------------------------------------------//
// Render the current scene                                                   //
//----------------------------------------------------------------------------//

void Viewer::onRender(const unsigned int &id)
{
   //initMaterialLight(id);
  // render the model
  //renderModel( id);
 //drawGnomon(id);
  // render the cursor
  //renderCursor();
  	//setShaderData(id);
  simple_render(id);
  // swap the front- and back-buffer
  glutSwapBuffers(); 
    glFlush();
}

//----------------------------------------------------------------------------//
// Shut the viewer down                                                       //
//----------------------------------------------------------------------------//

void Viewer::onShutdown()
{
  // destroy model instance
  delete m_calModel;

  // destroy core model instance
  delete m_calCoreModel;

}

//----------------------------------------------------------------------------//
// Render the cursor                                                          //
//----------------------------------------------------------------------------//

void Viewer::renderCursor()
{

}

//----------------------------------------------------------------------------//
// Parse the configuration file and load the whole model                      //
//----------------------------------------------------------------------------//

bool Viewer::parseModelConfiguration(const std::string& strFilename)
{
  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)

  {
    std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
    return false;
  }

  // create a core model instance
  m_calCoreModel=new CalCoreModel(std::string("default"));
    
  if(m_calCoreModel == NULL)
  {
    CalError::printLastError();
    return false;
  }

  // parse all lines from the model configuration file
  int line;
  for(line = 1; ; line++)
  {
    // read the next model configuration line
    std::string strBuffer;
    std::getline(file, strBuffer);

    // stop if we reached the end of file
    if(file.eof()) break;

    // check if an error happend while reading from the file
    if(!file)
    {
      std::cerr << "Error while reading from the model configuration file '" << strFilename << "'." << std::endl;
      return false;
    }

    // find the first non-whitespace character
    std::string::size_type pos;
    pos = strBuffer.find_first_not_of(" \t");

    // check for empty lines
    if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

    // check for comment lines
    if(strBuffer[pos] == '#') continue;

    // get the key
    std::string strKey;
    strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
    pos += strKey.size();

    // get the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos);
    if((pos == std::string::npos) || (strBuffer[pos] != '='))
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }

    // find the first non-whitespace character after the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos + 1);

    // get the data
    std::string strData;
    strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

    // handle the model creation
    if(strKey == "scale")
    {
      // set rendering scale factor
      m_scale = atof(strData.c_str());
    }
    else if(strKey == "skeleton")
    {
      // load core skeleton
      std::cout << "Loading skeleton '" << strData << "'..." << std::endl;
      if(!m_calCoreModel->loadCoreSkeleton(strData))
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "animation")
    {
      // load core animation
      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreAnimation(strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "mesh")
    {
      // load core mesh
      std::cout << "Loading mesh '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMesh(strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "material")
    {
      // load core material
      std::cout << "Loading material '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMaterial(strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else
    {
      // everything else triggers an error message, but is ignored
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
    }
  }

  // explicitely close the file
  file.close();

  return true;
}

int Viewer::initMaterialLight(const unsigned int &id){


	int lightposHandle = glGetUniformLocation(id, "point_light_pos");
	if (lightposHandle == -1) {
        fprintf(stderr, "Error: missing light pos\n");
		exit(1);
    }
    
	// Update the light position
	float lightPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
    glUniform4fv(lightposHandle, 1, lightPos); 

    // Uniform lighting variables
    int lightambientHandle = glGetUniformLocation(id, "point_light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(id, "point_light_diffuse");
	int lightspecularHandle = glGetUniformLocation(id, "point_light_specular");
	if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables b\n");
		return 1;
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
	float lightDirection[4] = { 0.0f, -1.0f, 0.0f, 0.0f };	
    glUniform4fv(directionlightposHandle, 1, lightDirection); 

    // Uniform lighting variables
    int dirlightambientHandle = glGetUniformLocation(id, "directional_light_ambient");
	int dirlightdiffuseHandle = glGetUniformLocation(id, "directional_light_diffuse");
	int dirlightspecularHandle = glGetUniformLocation(id, "directional_light_specular");
	if ( dirlightambientHandle == -1 ||
         dirlightdiffuseHandle == -1 ||
         dirlightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables b\n");
		return 1;
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
		exit(1);
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
	
	
	return 0;
}




//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//



void Viewer::renderModel(const unsigned int &id)
{


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



	glm::mat4 projection = Camera->getProjMtx();

	
    if ( setUniformMatrix( id,
                           "P_matrix",
                           glm::value_ptr(projection) ) ) {
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
		glm::mat4 viewMatrix;
		viewMatrix = Camera->getViewMtx();

		// Combine Model , View and Projection together, in reverse order and send to the GPU
		//glm::mat4 model = glm::rotate(glm::mat4(),-90.0f,glm::vec3(1,0,0));
		//glm::mat4 model = m_catSteer->getTransform()*glm::rotate(glm::mat4(),-00.0f,glm::vec3(0,1,0))*glm::rotate(glm::mat4(),glm::radians(-90.0f),glm::vec3(1,0,0));
		
    glm::mat4 model =   glm::rotate(glm::mat4(),-00.0f,glm::vec3(0,1,0))*glm::rotate(glm::mat4(),glm::radians(-90.0f),glm::vec3(1,0,0));
    
		glm::mat3 N = glm::mat3(viewMatrix*model); // model is not currently rotated so just use view
		
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
	// Combine Model , View and Projection together, in reverse order and send to the GPU
	glm::mat4 M = glm::scale(glm::mat4(),glm::vec3(m_distance*0.5)); //identity
	//glm::mat4 V = glm::mat4(); //Camera->getViewMtx();
	glm::mat4 V = Camera->getViewMtx();
	//glm::mat4 V = glm::mat4();
    
    
       if ( setUniformMatrix( id,
                           "V_matrix",
                           glm::value_ptr(viewMatrix) ) ) {
        return;
    }
    glm::mat4 projection = Camera->getProjMtx();
	//glm::mat4 projection = glm::mat4();

	
    if ( setUniformMatrix( id,
                           "P_matrix",
                           glm::value_ptr(projection) ) ) {
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

//----------------------------------------------------------------------------//
// Set the dimension                                                          //
//----------------------------------------------------------------------------//

void Viewer::setDimension(int width, int height)
{
  // store new width and height values
  m_width = width;
  m_height = height;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
	float new_aspect = ((float)width / (float)height);
	Camera->updateAspect(new_aspect) ;
}

//----------------------------------------------------------------------------//

void Viewer::drawGnomon(const unsigned int &programID){


 // clear all the buffers
  //glClearColor(0.0f, 0.8f, 0.8f, 0.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(programID);
	float narrow_amount = 0.02f;
	glBindVertexArray(gnomonVao);
//glm::mat4 viewMatrix = Camera->getViewMtx();

    glm::mat4 identity=glm::mat4();
	// Combine Model , View and Projection together, in reverse order and send to the GPU
	glm::mat4 M = glm::scale(glm::mat4(),glm::vec3(m_distance*0.5)); //identity
	//glm::mat4 V = glm::mat4(); //Camera->getViewMtx();
	glm::mat4 V = Camera->getViewMtx();
	//glm::mat4 V = glm::mat4();
	
    glm::mat3 N = glm::mat3(V*identity);
    
    // Send the updated MVP matrix to the shader
    if ( setUniformMatrix( programID,
                           "M_matrix",
                           glm::value_ptr(M) ) ) {
        return;
    }
	
    if ( setUniformMatrix3( programID,
                           "N_matrix",
                           glm::value_ptr(N) ) ) {
        return;
    }
    
       if ( setUniformMatrix( programID,
                           "V_matrix",
                           glm::value_ptr(V) ) ) {
        return;
    }
    glm::mat4 projection = Camera->getProjMtx();
	//glm::mat4 projection = glm::mat4();

	
    if ( setUniformMatrix( programID,
                           "P_matrix",
                           glm::value_ptr(projection) ) ) {
        return;
    }
    /*
	glm::mat4 eyeRotationY = projection;
    std::cout << eyeRotationY[0][0] << ",";
	std::cout << eyeRotationY[0][1] << ",";
	std::cout << eyeRotationY[0][2] << ",";
	std::cout << eyeRotationY[0][3] << std::endl;
	std::cout << eyeRotationY[1][0] << ",";
	std::cout << eyeRotationY[1][1] << ",";
	std::cout << eyeRotationY[1][2] << ",";
	std::cout << eyeRotationY[1][3] << std::endl;
	std::cout << eyeRotationY[2][0] << ",";
	std::cout << eyeRotationY[2][1] << ",";
	std::cout << eyeRotationY[2][2] << ",";
	std::cout << eyeRotationY[2][3] << std::endl;
	
	std::cout << eyeRotationY[3][0] << ",";
	std::cout << eyeRotationY[3][1] << ",";
	std::cout << eyeRotationY[3][2] << ",";
	std::cout << eyeRotationY[3][3] << std::endl;
	*/
		
	
	float origin[3]={0.0,0.0,0.0};
	float pos_x[3]={1.0,0.0,0.0};
	float pos_y[3]={0.0,1.0,0.0};
	float pos_z[3]={0.0,0.0,1.0};
	
	std::vector<float> gnomon_verts;
	gnomon_verts.push_back(origin[0]);
	gnomon_verts.push_back(origin[1]);
	gnomon_verts.push_back(origin[2]);
	
	gnomon_verts.push_back(origin[0]);
	gnomon_verts.push_back(origin[1]);
	gnomon_verts.push_back(origin[2]);
	
	gnomon_verts.push_back(origin[0]);
	gnomon_verts.push_back(origin[1]);
	gnomon_verts.push_back(origin[2]);

	gnomon_verts.push_back(pos_x[0]);
	gnomon_verts.push_back(pos_x[1]);
	gnomon_verts.push_back(pos_x[2]);
	
	gnomon_verts.push_back(pos_y[0]);
	gnomon_verts.push_back(pos_y[1]);
	gnomon_verts.push_back(pos_y[2]);
	
	gnomon_verts.push_back(pos_z[0]);
	gnomon_verts.push_back(pos_z[1]);
	gnomon_verts.push_back(pos_z[2]);
	
	gnomon_verts.push_back(origin[0]);
	gnomon_verts.push_back(origin[1]+narrow_amount);
	gnomon_verts.push_back(origin[2]);
	
	gnomon_verts.push_back(origin[0]);
	gnomon_verts.push_back(origin[1]);
	gnomon_verts.push_back(origin[2]+narrow_amount);
	
	gnomon_verts.push_back(origin[0]+narrow_amount);
	gnomon_verts.push_back(origin[1]);
	gnomon_verts.push_back(origin[2]);	
	
	std::vector<float> gnomon_colors;
	gnomon_colors.push_back(pos_x[0]);
	gnomon_colors.push_back(pos_x[1]);
	gnomon_colors.push_back(pos_x[2]);
	
	gnomon_colors.push_back(pos_y[0]);
	gnomon_colors.push_back(pos_y[1]);
	gnomon_colors.push_back(pos_y[2]);
	
	gnomon_colors.push_back(pos_z[0]);
	gnomon_colors.push_back(pos_z[1]);
	gnomon_colors.push_back(pos_z[2]);
	
	gnomon_colors.push_back(pos_x[0]);
	gnomon_colors.push_back(pos_x[1]);
	gnomon_colors.push_back(pos_x[2]);
	
	gnomon_colors.push_back(pos_y[0]);
	gnomon_colors.push_back(pos_y[1]);
	gnomon_colors.push_back(pos_y[2]);
	
	gnomon_colors.push_back(pos_z[0]);
	gnomon_colors.push_back(pos_z[1]);
	gnomon_colors.push_back(pos_z[2]);

	gnomon_colors.push_back(pos_x[0]);
	gnomon_colors.push_back(pos_x[1]);
	gnomon_colors.push_back(pos_x[2]);
	
	gnomon_colors.push_back(pos_y[0]);
	gnomon_colors.push_back(pos_y[1]);
	gnomon_colors.push_back(pos_y[2]);
	
	gnomon_colors.push_back(pos_z[0]);
	gnomon_colors.push_back(pos_z[1]);
	gnomon_colors.push_back(pos_z[2]);	
	
	std::vector<float> gnomon_normals;
	gnomon_normals.push_back(pos_z[0]);
	gnomon_normals.push_back(pos_z[1]);
	gnomon_normals.push_back(pos_z[2]);
	
	gnomon_normals.push_back(pos_x[0]);
	gnomon_normals.push_back(pos_x[1]);
	gnomon_normals.push_back(pos_x[2]);
	
	gnomon_normals.push_back(pos_y[0]);
	gnomon_normals.push_back(pos_y[1]);
	gnomon_normals.push_back(pos_y[2]);
	
	gnomon_normals.push_back(pos_z[0]);
	gnomon_normals.push_back(pos_z[1]);
	gnomon_normals.push_back(pos_z[2]);
	
	gnomon_normals.push_back(pos_x[0]);
	gnomon_normals.push_back(pos_x[1]);
	gnomon_normals.push_back(pos_x[2]);
	
	gnomon_normals.push_back(pos_y[0]);
	gnomon_normals.push_back(pos_y[1]);
	gnomon_normals.push_back(pos_y[2]);
	
	gnomon_normals.push_back(pos_z[0]);
	gnomon_normals.push_back(pos_z[1]);
	gnomon_normals.push_back(pos_z[2]);
	
	gnomon_normals.push_back(pos_x[0]);
	gnomon_normals.push_back(pos_x[1]);
	gnomon_normals.push_back(pos_x[2]);
	
	gnomon_normals.push_back(pos_y[0]);
	gnomon_normals.push_back(pos_y[1]);
	gnomon_normals.push_back(pos_y[2]);
	
	std::vector<GLuint> gnomon_inds;
	gnomon_inds.push_back(0);
	gnomon_inds.push_back(3);		
	gnomon_inds.push_back(6);

	gnomon_inds.push_back(1);
	gnomon_inds.push_back(4);		
	gnomon_inds.push_back(7);
	
	gnomon_inds.push_back(2);
	gnomon_inds.push_back(5);		
	gnomon_inds.push_back(8);

	// A single data buffer store for generic, per-vertex attributes
	//unsigned int buffer[NUM_BUFFERS];
	//glGenBuffers(NUM_BUFFERS, buffer);

	// Allocate GPU memory for our vertices and copy them over
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*gnomon_verts.size(), &gnomon_verts[0], GL_STATIC_DRAW);
    
	// Do the same for our vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*gnomon_colors.size(), &gnomon_colors[0], GL_STATIC_DRAW);
	
	// Do the same for our vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*gnomon_normals.size(), &gnomon_normals[0], GL_STATIC_DRAW);
	
	// Now we tell OpenGL how to interpret the data we just gave it
	// Tell OpenGL what shader variable it corresponds to
    // Tell OpenGL how it's formatted (floating point, 3 values per vertex)
	int vertLoc = glGetAttribLocation(programID, "a_vertex");
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[0]);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Do the same for the vertex colours
	int colourLoc = glGetAttribLocation(programID, "a_color");
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[1]);
	glEnableVertexAttribArray(colourLoc);
	glVertexAttribPointer(colourLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
    // Do the same for the vertex colours
	int normalLoc = glGetAttribLocation(programID, "a_normal");
	glBindBuffer(GL_ARRAY_BUFFER, bufferGnomon[2]);
	glEnableVertexAttribArray(normalLoc);
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	

	//Setup indices                    
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, arrayGnomonVboArray );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, gnomon_inds.size()*sizeof(GLuint), &gnomon_inds[0], GL_STATIC_DRAW );
	

	// Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(programID, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(programID, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(programID, "mtl_specular");
	int mtlshininessHandle = glGetUniformLocation(programID, "shininess");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1||
         mtlshininessHandle == -1 )
          {
        fprintf(stderr, "Error: can't find material uniform variables a\n");
		exit(1);
    }
    
	float mtlambient[3] = { 1.0f, 1.0f, 1.0f };	// ambient material
    float mtldiffuse[3] = { 1.0f, 1.0f, 1.0f };	// diffuse material
    float mtlspecular[3] = { 0.0f, 0.0f, 0.0f };	// specular material
    float mtlshininess = 32.0f;	// specular material

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);
	glUniform1f(mtlshininessHandle,mtlshininess)	;	            
		            
    
    // Send command to GPU to draw the data in the current VAO as triangles
	glBindVertexArray(gnomonVao);
    glDrawElements( GL_TRIANGLES,  gnomon_inds.size(), GL_UNSIGNED_INT, NULL );
    glBindVertexArray(0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    gnomon_verts.clear();
    gnomon_colors.clear();
    int curTime = Tick::getTick();
    //std::cout << curTime << std::endl;
    
  	 //   std::cout << "Program ID init: " << programID << std::endl;

}



/**
 * Sets the shader uniforms and vertex data
 * This happens ONCE only, before any frames are rendered
 * @param id, Shader program object to use
 * @returns 0 for success, error otherwise
 */
int Viewer::setShaderData(const unsigned int &id) 
{
	/*
	 * What we want to draw
	 * Each set of 3 vertices (9 floats) defines one triangle
	 * You can define more triangles to draw here
	 */
	float vertices[ NUM_VERTS*VALS_PER_VERT ] = {
			-0.5f, -0.5f, -0.0f,	// Bottom left
			0.5f, -0.5f, -0.0f,		// Bottom right
			0.0f, 0.5f, -0.0f		// Top
	};
    
    // Colours for each vertex; red, green, blue and alpha
	// This data is indexed the same order as the vertex data, but reads 4 values
	// Alpha will not be used directly in this example program
	float colours[ NUM_VERTS*VALS_PER_COLOUR ] = {
            0.8f, 0.7f, 0.5f, 1.0f,
            0.3f, 0.7f, 0.1f, 1.0f,
            0.8f, 0.2f, 0.5f, 1.0f,
	};
	
	GLuint gnomonVao; 
	// Generate storage on the GPU for our triangle and make it current.
	// A VAO is a set of data buffers on the GPU
    glBindVertexArray(gnomonVao);

	// Generate new buffers in our VAO
	// A single data buffer store for generic, per-vertex attributes
	
	// Allocate GPU memory for our vertices and copy them over
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NUM_VERTS*VALS_PER_VERT, vertices, GL_STATIC_DRAW);
    
	// Do the same for our vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NUM_VERTS*VALS_PER_COLOUR, colours, GL_STATIC_DRAW);

	// Now we tell OpenGL how to interpret the data we just gave it
	// Tell OpenGL what shader variable it corresponds to
    // Tell OpenGL how it's formatted (floating point, 3 values per vertex)
	int vertLoc = glGetAttribLocation(id, "a_vertex");
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Do the same for the vertex colours
	int colourLoc = glGetAttribLocation(id, "a_colour");
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(colourLoc);
	glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);
    
    glm::mat4 identity;
	// Combine Model , View and Projection together, in reverse order and send to the GPU
	glm::mat4 M = identity; //identity
	glm::mat4 V = identity;
	//glm::mat4 V = glm::mat4();
	
    glm::mat3 N = glm::mat3(identity);
    /*
    // Send the updated MVP matrix to the shader
    if ( setUniformMatrix( id,
                           "M_matrix",
                           glm::value_ptr(M) ) ) {
        return 1;
    }

    if ( setUniformMatrix3( id,
                           "N_matrix",
                           glm::value_ptr(N) ) ) {
        return 1;
    }
       if ( setUniformMatrix3( id,
                           "V_matrix",
                           glm::value_ptr(V) ) ) {
        return 1;
    }
    glm::mat4 projection = identity;
	//glm::mat4 projection = glm::mat4();

	
    if ( setUniformMatrix( id,
                           "P_matrix",
                           glm::value_ptr(projection) ) ) {
        return 1;
    } 
    */   
	// An argument of zero un-binds all VAO's and stops us
    // from accidentally changing the VAO state
	glBindVertexArray(0);

	// The same is true for buffers, so we un-bind it too
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	std::cout << "started" << std::endl;
	return 0;	// return success
}


/**
 * Renders a frame of the state and shaders we have set up to the window
 * Executed each time a frame is to be drawn.
 */
void Viewer::simple_render(const unsigned int &id) 
{
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	// Clear the previous pixels we have drawn to the colour buffer (display buffer)
	// Called each frame so we don't draw over the top of everything previous
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(id);

	// Make the VAO with our vertex data buffer current
	glBindVertexArray(vertexVaoShapeArray);

    // Send command to GPU to draw the data in the current VAO as triangles
    //glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
      // set the global OpenGL states
  glEnable(GL_DEPTH_TEST);
    drawGnomon(id);
    
    renderModel( id);
    
	glBindVertexArray(0);	// Un-bind the VAO

	glutSwapBuffers();	// Swap the back buffer with the front buffer, showing what has been rendered

	glFlush();	// Guarantees previous commands have been completed before continuing
	//	std::cout << "repeat" << std::endl;
}
