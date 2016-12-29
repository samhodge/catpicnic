/*********************************************************
 Version 1.2
 
 Code provided by Gustavo Carneiro, Anthony Dick
 for the course 
 COMP SCI 3014/7090 Computer Graphics
 School of Computer Science
 University of Adelaide
 
 Permission is granted for anyone to copy, use, modify, or distribute this
 program and accompanying programs and documents for any purpose, provided
 this copyright notice is retained and prominently displayed, along with
 a note saying that the original programs are available from the aforementioned 
 course web page. 
 
 The programs and documents are distributed without any warranty, express or
 implied.  As the programs were written for research purposes only, they have
 not been tested to the degree that would be advisable in any important
 application.  All use of these programs is entirely at the user's own risk.
 *********************************************************/

/**
 * Skymap
 */

#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <unistd.h>

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif




#include "shader.h"
#include "skybox.h"
#include "InputState.h"
#include "Camera.h"
#include "Ocean.h"
#include "cat.h"
#include "wander.h"
#include "Island.h"
#include "Audio.h"

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_COLOUR 4
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`

// A cube
unsigned int cubeVaoHandle;

// Our shader program
GLuint programID;
unsigned int cubeMapTextureHandle;

// An accumulation of the input for our camera rotation
float xRot = 0.0f;
float yRot = 0.0f;

int winX = 640;
int winY = 480;



//
// Switch between orthographic and perspective camera.
// Also ensure the cube stays the same shape if window resized
//


/**
 * Sets the shader uniforms and generic attributes
 * @param program, Shader program object to use
 */

 Skybox* m_skybox;
 InputState Input;
 Camera* m_camera;
 Ocean* m_ocean;
 Cat* m_cat;
 SteeringCat *m_catSteer;
 Island *m_Island;
 AudioLoop *m_Audio;

 float strafe_speed;
 float travel_speed;
     float groundPlane = 1.0f;
int setup() {
    m_camera = new Camera();
    travel_speed=0.0f;
    strafe_speed=0.0f;
    m_camera->PositionCamera(-10.0f, 5.0f, 20.0f, 0.42f, 0.105f);
    //m_camera->SetSpeed(0.0f);
    m_camera->SetPerspective(glm::radians(53.13f), (float) winX / winY, 0.1f, 10000.0f );
    
    m_skybox = new Skybox( 

    "san_francisco/posx.jpg",
    "san_francisco/posy.jpg",
    "san_francisco/posz.jpg",
    "san_francisco/negx.jpg",
    "san_francisco/negy.jpg",
    "san_francisco/negz.jpg",
  10.0f
  );
  
    
        m_ocean = new Ocean( 

    "san_francisco/posx.jpg",
    "san_francisco/posy.jpg",
    "san_francisco/posz.jpg",
    "san_francisco/negx.jpg",
    "san_francisco/negy.jpg",
    "san_francisco/negz.jpg",
    0.0f,
    0.0f,
    0.0f,
  1000.0f
  );
  
        /*
        public:
    Cat( const char* in_skeleton,
                const char* in_mesh,
                const char* in_walk_F_anim,
                const char* in_walk_L_anim,
                const char* in_walk_R_anim,
                const char* in_material,
                const char* in_tex_albedo,
                float in_cycle_duration,
                float in_scale);
                */
        glm::mat4 initialLocation = glm::mat4();
        m_cat = new Cat(
                "jnx.csf",
                "jnx.cmf",
                "walk.caf",
                "walk_L.caf",
                "walk_R.caf",
                "jnx.crf",
                "test.tga",
                1.33333333f,
                0.1f,
                initialLocation);

    glm::vec3 initialPosition=glm::vec3(0.0,0.0,0.0);
    glm::vec3 initialVelocity=glm::vec3(0.5,0.0,0.0);
    float initialMass =5.0;
     m_catSteer = new SteeringCat (initialPosition, 
                initialVelocity, 
                initialMass);   
     /*
Island::Island (const char* heightMap,
    float scalex,
    float scaley,
    float scalez,
    float posx,
    float posy,
    float posz
    */
    const char* hieightMap = "islandHieght.tga";

     m_Island = new Island (
            "islandHieght.tga",
            40.0,
            1.0,
            1.0,
            0.0,
            groundPlane,
            0.0
        );
     m_Audio = new AudioLoop("Intro.mp3");
     m_Audio->playSoundOnce();
    return 0;	// return success
}


const int TIMERMSECS = 400;
float animation_time = 0;
const float  animation_step = 0.03f;
void animate(int id){
        float time_elapsed = TIMERMSECS/1000.0;
        float current_step = animation_step* time_elapsed;
        animation_time += current_step;
        glutPostRedisplay();
        
        travel_speed*=0.8f;
        strafe_speed*=0.8f;        
}

#define ANG_SCALE 28.0f; 
/**
 * Renders a frame of the state and shaders we have set up to the window
 */
void render() {

    m_camera->MoveCamera(travel_speed);
    m_camera->Strafe(strafe_speed);
    m_camera->SetViewByMouse(Input.deltaX, Input.deltaY);
    
    // GL state
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutTimerFunc(17,animate,0); 
    m_catSteer->update();
    m_skybox->projectionMatrix = m_camera->GetProjectionMatrix();
    m_skybox->viewMatrix = m_camera->GetRotationMatrix();
    m_skybox->render();
    m_ocean->projectionMatrix =  m_camera->GetProjectionMatrix();
    m_ocean->viewMatrix = m_camera->GetViewMatrix();
    m_ocean->cameraPosition = m_camera->GetPosition();
    m_Island->projectionMatrix =  m_camera->GetProjectionMatrix();
    m_Island->viewMatrix = m_camera->GetViewMatrix();
    m_Island->cameraPosition = m_camera->GetPosition();
    m_Island->render();
    m_cat->viewMatrix = m_camera->GetViewMatrix();
    m_cat->projectionMatrix = m_camera->GetProjectionMatrix();
    m_cat->location = glm::translate(glm::mat4(),glm::vec3(0.0,groundPlane,0.0))*m_catSteer->getTransform();
    m_cat->direction =  m_catSteer->getAngularVelocity()*ANG_SCALE;
    //std::cout << m_cat->direction << std::endl;
    m_cat->onIdle();
    m_cat->renderModel();
#ifdef DEBUG    
    std::cout << "Position Camera:" << m_ocean->cameraPosition.x << "," <<  m_ocean->cameraPosition.y << "," << m_ocean->cameraPosition.z << "," << std::endl;    
#endif
    m_ocean->render();
    glFlush();
    glutSwapBuffers();
#ifdef DEBUG    
    std::cout << "Yaw:" << m_camera->GetYaw() << std::endl;

    std::cout << "Pitch:" << m_camera->GetPitch() << std::endl;
    std::cout << "Position: " << 
        m_camera->GetPosition()[0] << "," <<
        m_camera->GetPosition()[1] << "," <<
        m_camera->GetPosition()[2] << "," << std::endl; 
#endif
}
float STRAFE_INC=0.15;

float FORWARD_INC=0.15;
void SpecialFunc( int key, int x, int y )
{
   	switch(key) 
	{
        case GLUT_KEY_LEFT:
            yRot -= 2.0;
            strafe_speed += -STRAFE_INC;
            break;
        case GLUT_KEY_RIGHT:
            yRot += 2.0;
            strafe_speed += STRAFE_INC;
            break;
        case GLUT_KEY_UP:
            xRot += 2.0;
            travel_speed += FORWARD_INC;
            break;
        case GLUT_KEY_DOWN:
            xRot -= 2.0;
            travel_speed += -FORWARD_INC;
            break;

    }
    glutPostRedisplay();
}


/**
 * Called while a keyboard key press is detected
 * This GLUT functions is not OpenGL specific, but allows interactivity to our programs
 * @param key, the keyboard key that made the event
 * @param x, not used
 * @param y, not used
 */
void keyboardDown(unsigned char key, int x, int y) {

	// We simply check the key argument against characters we care about, in this case A and D
    switch(key) 
    {
        case 27: // escape key pressed
            exit(0);
            break;

        case 'd': // Toggle depth test
            if ( glIsEnabled( GL_DEPTH_TEST ) ) {
                glDisable( GL_DEPTH_TEST );
            }
            else {
                glEnable( GL_DEPTH_TEST );
            }
            glutPostRedisplay();
            break;
            glutPostRedisplay();
            break;

    }

}

void reshapeFunc( int x, int y ) 
{
    winX = x;
    winY = y;
    glViewport(0, 0, (GLsizei)x, (GLsizei)y); // Set our viewport to the size of our window  
    m_camera->SetPerspective(glm::radians(53.13f), (float) winX / winY, 0.05f, 100.0f );
    
}

void MouseFunc(int button, int state, int x, int y)
{
    // Update a global set of variables 
    // to avoid cluttering up the callbacks
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                Input.lMousePressed = true;
                Input.prevX = x;
                Input.prevY = y;
            }
            else
                Input.lMousePressed = false;
                Input.deltaX = 0.0f;
                Input.deltaY = 0.0f;
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN){
                Input.rMousePressed = true;
                Input.prevX = x;
                Input.prevY = y;    
            }               
            else 
                Input.rMousePressed = false;
                Input.deltaX = 0.0f;
                Input.deltaY = 0.0f;            
            break;

        default:
            break;
    }
}

void MotionFunc(int x, int y)
{
    int xDiff = x - Input.prevX;
    int yDiff = y - Input.prevY;
    Input.deltaX += xDiff;
    Input.deltaY += yDiff;
    Input.prevX = x;
    Input.prevY = y;
}


/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLUT window and function call backs
 * Takes no arguments
 */
int main(int argc, char **argv) {

	std::cout << "Controls: arrow keys rotate cube\n";

	// Set up GLUT window
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(winX, winY);

#ifdef __APPLE__    
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE | GLUT_MULTISAMPLE);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif
	glutCreateWindow("Cat Picnic");

    glClearColor(0.2f,0.2f,0.2f,1.0f);
	/*
    // Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
    
	programID = LoadShaders("sky.vert", "sky.frag");
	if (programID == 0)
		return 1;
	glUseProgram(programID);
    */
	if (setup() !=0)
		return 1;

	// Here we set a new function callback which is the GLUT handling of keyboard input
	glutKeyboardFunc(keyboardDown);
    glutSpecialFunc(SpecialFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
	glutDisplayFunc(render);
    glutReshapeFunc(reshapeFunc);
	glutMainLoop();

	return 0;
}
