//----------------------------------------------------------------------------//
// main.cpp                                                                   //
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


#include "global.h"

#include "shader.h"
#include "viewer.h"


GLuint programID;
//----------------------------------------------------------------------------//
// GLUT callback functions                                                    //
//----------------------------------------------------------------------------//

void displayFunc()
{
  // render the scene
  theViewer.onRender( programID);
}

void exitFunc()
{
  // shut the viewer instance down
  theViewer.onShutdown();
}

void idleFunc()
{
  // redirect to the viewer instance
  theViewer.onIdle(programID);
}

void keyboardFunc(unsigned char key, int x, int y)
{
  // redirect the message to the viewer instance
  theViewer.onKey(key, x, theViewer.getHeight() - y - 1);
}

void motionFunc(int x, int y)
{
  // redirect the message to the viewer instance
  theViewer.onMouseMove(x, theViewer.getHeight() - y - 1);
}

void mouseFunc(int button, int state, int x, int y)
{
  // redirect the message to the viewer instance
  if(state == GLUT_DOWN)
  {
    theViewer.onMouseButtonDown(button, x, theViewer.getHeight() - y - 1);
  }
  else if(state == GLUT_UP)
  {
    theViewer.onMouseButtonUp(button, x, theViewer.getHeight() - y - 1);
  }
}

void reshapeFunc(int width, int height)
{
  // set the new width/height values
  theViewer.setDimension(width, height);
}

//----------------------------------------------------------------------------//
// Main entry point of the application                                        //
//----------------------------------------------------------------------------//
	// Set up the shaders we are to use. 0 indicates error.
	
int main(int argc, char *argv[])
{
  // initialize the GLUT system
  glutInit(&argc, argv);
  
	
  // create our view instance
  if(!theViewer.onCreate(argc, argv))
  {
    std::cerr << "Creation of the viewer failed." << std::endl;
    return -1;
  }
  

  // register our own exit callback
  atexit(exitFunc);
	
#ifdef __APPLE__    
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE | GLUT_MULTISAMPLE);
//	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif

  glutInitWindowSize(theViewer.getWidth(), theViewer.getHeight());
  glutCreateWindow(theViewer.getCaption().c_str());
  if(theViewer.getFullscreen()) glutFullScreen();
  //glutSetCursor(GLUT_CURSOR_NONE);  
		
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
		
	programID= LoadShaders("shaded.vert", "shaded.frag");
		
	
	if (programID == 0){
		std::cout << "Shaders did not load" << std::endl;
		return 1;
	}
  
  	glUseProgram(programID);	

  	    std::cout << "Program ID init: " << programID << std::endl;
  // register all GLUT callback functions
  glutIdleFunc(idleFunc);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(motionFunc);
  glutPassiveMotionFunc(motionFunc);
  glutReshapeFunc(reshapeFunc);
  glutDisplayFunc(displayFunc);
  glutKeyboardFunc(keyboardFunc);
  
    // initialize our viewer instance
  if(!theViewer.onInit(programID))
  {
    std::cerr << "Initialization of the viewer failed." << std::endl;
    return -1;
  }


  glutMainLoop();

  return 0;
}

//----------------------------------------------------------------------------//
