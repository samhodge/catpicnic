//----------------------------------------------------------------------------//
// viewer.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef VIEWER_H
#define VIEWER_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "global.h"
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "sh_matrix.h"
#include <cstring>
#include "shader.h"
//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CalCoreModel;
class CalModel;

#define NUM_BUFFERS 5

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define NUM_VERTS 3			// Total number of vertices to load/render
#define NUM_AVERAGES 20
class Cat
{
protected:

  unsigned int m_lastTick;
  bool m_bPaused;
  //static const std::string model_name;
  CalCoreModel * m_calCoreModel;
  CalModel * m_calModel;
  //CalCoreModel m_calCoreModel;
  //CalModel m_calModel;
  float m_scale;
  int m_currentAnimationId;
  float m_leftAnimationTime;
  float m_blendTime;
  float m_lodLevel;
  int m_vertexCount;
  int m_faceCount;
  float near_clip;
  float far_clip;
  float camera_h_fov;
  float onEmission;
  float onPoint;
  float onDirectional; 
  float wireframeToggle;
  GLuint vertexVaoShapeArray;
  GLuint buffer[NUM_BUFFERS];
  GLuint arrayVboArray;
  GLuint vertexVaoGnomonArray;
  GLuint gnomonVao;
  GLuint bufferGnomon[NUM_BUFFERS];
  GLuint arrayGnomonVboArray;
  GLuint id; // Vertex Shader ID
// constructors/destructor
public:
	Cat( const char* in_skeleton,
                const char* in_mesh,
                const char* in_walk_F_anim,
                const char* in_walk_L_anim,
                const char* in_walk_R_anim,
                const char* in_material,
                const char* in_tex_albedo,
                float in_cycle_duration,
                float in_scale,
                glm::mat4 in_location);
	
  virtual ~Cat();
// member functions
public:
  void renderModel();
  void onIdle();
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::mat4 modelMatrix;
  glm::mat4 location;
  float direction;


protected: 
  GLuint loadTexture(const std::string& strFilename);

  bool createShaders(const char* vert, const char* frag);
  bool onInitModel();
  bool onInitShading();
  bool onCreate();
  bool initModel();
  bool initProgram();
  bool initMaterialLight();
private:
  std::string skeleton;
  std::string mesh;
  std::string walk_anim_F;
  std::string walk_anim_L;
  std::string walk_anim_R;
  std::string tex_albedo;
  std::string material;
  float cycle_duration;
  int id_anim_F;
  int id_anim_L;
  int id_anim_R;
  std::string tex_map;
  float scale;
  GLuint tex_cube;
  float direction_average[NUM_AVERAGES];
  float change;
};


#endif

//----------------------------------------------------------------------------//
