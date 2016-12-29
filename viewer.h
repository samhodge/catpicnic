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
#include "wander.h"
#include "sh_matrix.h"
//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class ViewCamera 
{
public:
    glm::mat4 viewMtx;
    glm::mat4 viewProj;    
    glm::vec3 initEye;
    glm::vec3 initAt;
    glm::vec3 initUp;
	float initFov;
	float initAspect;
	float initNear;
	float initFar;    
	float zoom;
	float aspect;
    ViewCamera( glm::vec3 eye, glm::vec3 at, glm::vec3 up, float fov, float aspect, float near, float far );
    virtual ~ViewCamera() = 0;

    const glm::mat4 getViewMtx() const;
    const glm::mat4 getProjMtx() const;
    const float getZoom() const;
    //    void orthogonaliseViewMtx();
    void reset();
    void updateAspect(float aspect);
};

class ObjectViewer : public ViewCamera
{
public:
    ObjectViewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up , float fov, float aspect, float near, float far );

};
#define NUM_BUFFERS 5

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define NUM_VERTS 3			// Total number of vertices to load/render
class Viewer
{
protected:
  int m_width;
  int m_height;
  bool m_bFullscreen;
  int m_mouseX;
  int m_mouseY;
  float m_tiltAngle;
  float m_twistAngle;
  float m_distance;
  bool m_bLeftMouseButtonDown;
  bool m_bRightMouseButtonDown;
  unsigned int m_lastTick;
  bool m_bPaused;
  //static const std::string model_name;
  CalCoreModel* m_calCoreModel;
  CalModel* m_calModel;
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
  ViewCamera *Camera;
  ObjectViewer *m_objCam;
  SteeringCat *m_catSteer;
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
// constructors/destructor
public:
	Viewer();
	virtual ~Viewer();
	
// member functions
public:
  std::string getCaption();
  bool getFullscreen();
  int getHeight();
  int getWidth();
  bool onCreate(int argc, char *argv[]);
  void onIdle(const unsigned int &id);
  bool onInit(const unsigned int &id);
  void onKey(unsigned char key, int x, int y);
  void onMouseButtonDown(int button, int x, int y);
  void onMouseButtonUp(int button, int x, int y);
  void onMouseMove(int x, int y);
  void onRender(const unsigned int &id);
  void onShutdown();
  void setDimension(int width, int height);

protected: 
  GLuint loadTexture(const std::string& strFilename);
  bool parseModelConfiguration(const std::string& strFilename);
  void renderCursor();
  void renderModel(const unsigned int &id);
  void drawGnomon(const unsigned int &id);
  int initMaterialLight(const unsigned int &id);
  int setShaderData(const unsigned int &id);
  void simple_render(const unsigned int &id);
  ObjectViewer* getInitialCamera();
private:
  GLuint tex_cube;
};

extern Viewer theViewer;

#endif

//----------------------------------------------------------------------------//
