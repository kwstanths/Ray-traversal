#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#include "Application.h"

void Application::init()
{
	bPlay = true;
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	scene.init();
	
	for(unsigned int i=0; i<256; i++)
	{
	  keys[i] = false;
	  specialKeys[i] = false;
	}
	mouseButtons[0] = false;
	mouseButtons[1] = false;
	lastMousePos = glm::ivec2(-1, -1);
}

bool Application::loadMesh(const char *filename)
{
  return scene.loadMesh(filename);
}

bool Application::update(int deltaTime)
{
	scene.update(deltaTime);

    if (keys[119]) scene.getCamera().moveCamera(0.016f * 3, 0.0f); /* w */
    if (keys[115]) scene.getCamera().moveCamera(0.016f * (-3), 0.0f); /* s */
    if (keys[97]) scene.getCamera().moveCamera(0, 0.016f * (-3)); /* a */
    if (keys[100]) scene.getCamera().moveCamera(0, 0.016f * 3); /* d */

    if (keys[112]) {
        glm::vec3 pos = scene.getCamera().getPosition();
        std::cout << "(" << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    }
	return bPlay;
}

void Application::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Application::resize(int width, int height)
{
  glViewport(0, 0, width, height);
  scene.getCamera().resizeCameraViewport(width, height);
}

void Application::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
    
}

void Application::keyReleased(int key)
{
	keys[key] = false;
}

void Application::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Application::specialKeyReleased(int key)
{
	specialKeys[key] = false;
	if(key == GLUT_KEY_F1)
	  scene.switchPolygonMode();
}

void Application::mouseMove(int x, int y)
{
    scene.getCamera().rotateCamera(static_cast<float>(x), static_cast<float>(y));
}

void Application::mousePress(int button)
{
  mouseButtons[button] = true;
}

void Application::mouseRelease(int button)
{
  mouseButtons[button] = false;
  if(!mouseButtons[0] && !mouseButtons[1])
    lastMousePos = glm::ivec2(-1, -1);
}

bool Application::getKey(int key) const
{
	return keys[key];
}

bool Application::getSpecialKey(int key) const
{
	return specialKeys[key];
}

void Application::DisplayFPS(int frame_time_ms)
{
    /* That's so nasty */
    static int start;
    static unsigned int frames;

    start += frame_time_ms;
    frames++;
    if (start >= 1000) {

        scene.DisplayFps(frames);

        start = 0;
        frames = 0;
    }
}



