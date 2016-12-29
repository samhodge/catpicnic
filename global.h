//----------------------------------------------------------------------------//
// global.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef GLOBAL_H
#define GLOBAL_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <vector>

// GLEW loads OpenGL extensions. Required for all OpenGL programs.
// GLEW loads OpenGL extensions. Required for all OpenGL programs.
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cal3d/cal3d.h>


#endif

//----------------------------------------------------------------------------//

