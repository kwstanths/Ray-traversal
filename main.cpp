#include <ctime>
#include <iostream>

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include "Application.h"


//Remove console (only works in Visual Studio)
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


#define TIME_PER_FRAME 1000.f / 60.f // Approx. 60 fps

#define WINDOW_WIDTH    1024
#define WINDOW_HEIGHT   768 

static int prevTime;
static Application app; // This object represents our whole app


// If a key is pressed this callback is called

static void keyboardDownCallback(unsigned char key, int x, int y)
{
	Application::instance().keyPressed(key);
}

// If a key is released this callback is called

static void keyboardUpCallback(unsigned char key, int x, int y)
{
	Application::instance().keyReleased(key);
}

// If a special key is pressed this callback is called

static void specialDownCallback(int key, int x, int y)
{
	Application::instance().specialKeyPressed(key);
}

// If a special key is released this callback is called

static void specialUpCallback(int key, int x, int y)
{
	Application::instance().specialKeyReleased(key);
}

// Same for changes in mouse cursor position

static void motionCallback(int x, int y)
{
	Application::instance().mouseMove(x, y);
}

// Same for mouse button presses or releases

static void mouseCallback(int button, int state, int x, int y)
{
  int buttonId;
  
  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    buttonId = 0;
    break;
  case GLUT_RIGHT_BUTTON:
    buttonId = 1;
    break;
  case GLUT_MIDDLE_BUTTON:
    buttonId = 2;
    break;
  default:
      buttonId = 3;
      break;
  }

	if(state == GLUT_DOWN)
		Application::instance().mousePress(buttonId);
	else if(state == GLUT_UP)
		Application::instance().mouseRelease(buttonId);
}

static void mousePassiveCallback(int x, int y) {
    Application::instance().mouseMove(x, y);

    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

// Resizing the window calls this function

static void resizeCallback(int width, int height)
{
  Application::instance().resize(width, height);
}

static void drawCallback() {
    Application::instance().render();
    glutSwapBuffers();
}

static void idleCallback()
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - prevTime;
	
	if(deltaTime > TIME_PER_FRAME)
	{
		// Every time we enter here is equivalent to a game loop execution
		if(!Application::instance().update(deltaTime))
			exit(0);
        
        int deltaTime = currentTime - prevTime;
        Application::instance().DisplayFPS(deltaTime);
		prevTime = currentTime;
		
        glutPostRedisplay();
	}
}


int main(int argc, char **argv)
{
	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#ifdef __linux__
    glutInitContextVersion(3, 3);
#endif
	glutInitWindowPosition(30, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow(argv[0]);
	glutReshapeFunc(resizeCallback);
	glutDisplayFunc(drawCallback);
	glutIdleFunc(idleCallback);
	glutKeyboardFunc(keyboardDownCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialDownCallback);
	glutSpecialUpFunc(specialUpCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(mousePassiveCallback);
    glutSetCursor(GLUT_CURSOR_NONE);
	// GLEW will take care of OpenGL extension functions
	glewExperimental = GL_TRUE;
	glewInit();
	
	// Application instance initialization
	Application::instance().init();
    /*if (argc > 1)
        Application::instance().loadMesh(argv[1]);
    else
        Application::instance().loadMesh("models_srgge/bunny.ply");
	*/
    prevTime = glutGet(GLUT_ELAPSED_TIME);
	// GLUT gains control of the application
	glutMainLoop();

	return 0;
}



