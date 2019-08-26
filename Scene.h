#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Camera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "Text.h"
#include "GraphicsObject.hpp"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene {

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	void update(int deltaTime);
	void render();
    void renderPolygonMesh(TriangleMesh * mesh, glm::vec4 color = glm::vec4(0.9f, 0.9f, 0.95f, 1.0f));
    void renderRayCastingLine(glm::vec3 start, glm::vec3 end);

  Camera &getCamera();
  
  void switchPolygonMode();

  void DisplayFps(size_t fps);

private:
	void initShaders();

private:
    Camera camera;
    GraphicsObject * floor_;
    GraphicsObject * object_;
    GraphicsObject * object_lod_1;
    GraphicsObject * object_lod_2;
    GraphicsObject * object_lod_3;
    GraphicsObject * object_lod_4;
    GraphicsObject * object_lod_5;

	ShaderProgram basicProgram;
	float currentTime;
    int frame_time_;
	
	bool bPolygonFill;
	
	Text text;

    size_t fps_;

};


#endif // _SCENE_INCLUDE

