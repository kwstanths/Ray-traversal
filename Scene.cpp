#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <sstream>
#include <string>
#include <exception>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Scene.h"
#include "PLYReader.h"
#include "TriangleBoxOverlapping.hpp"


Scene::Scene()
{
}

Scene::~Scene()
{
    /* TODO Delete map */
}

void Scene::init() {
	initShaders();

    PLYReader reader;

    /* Read a mesh */
    std::cout << "Reading the model..." << std::endl;
    TriangleMesh * mesh = new TriangleMesh();
    reader.readMesh("bunny.ply", *mesh);
    /* Preprocess the mesh before sending to OpenGL */
    std::cout << "Preprocessing the model..." << std::endl;
    mesh->Preprocess();
    mesh->sendToOpenGL(basicProgram);
    /* Draw a mesh at that position */
    object_ = new GraphicsObject(mesh);
    object_->SetPosition(glm::vec3(0, 0, 0));

    /* Gather info for ray tracing, use 5 million rays */
    //mesh->TestRaysPerSecond(5000000);

    /* 
        Create a simplified version of that mesh, preprocess, send to opengl, 
        and create an object at that position
    */
    std::cout << "Clustering: 7 ..." << std::endl;
    TriangleMesh * mesh_lod_1 = mesh->VertexClustering(7);
    //TriangleMesh * mesh_lod_1 = mesh->VertexClustering_GRID(128);
    std::cout << "Preprocessing: 7 ..." << std::endl;
    mesh_lod_1->Preprocess();
    mesh_lod_1->sendToOpenGL(basicProgram);
    object_lod_1 = new GraphicsObject(mesh_lod_1);
    object_lod_1->SetPosition(glm::vec3(1, 0, 0));
    std::cout << std::endl;

    std::cout << "Clustering: 6 ..." << std::endl;
    TriangleMesh * mesh_lod_2 = mesh->VertexClustering(6);
    //TriangleMesh * mesh_lod_2 = mesh->VertexClustering_GRID(64);
    std::cout << "Preprocessing: 6 ..." << std::endl;
    mesh_lod_2->Preprocess();
    mesh_lod_2->sendToOpenGL(basicProgram);
    object_lod_2 = new GraphicsObject(mesh_lod_2);
    object_lod_2->SetPosition(glm::vec3(2, 0, 0));
    std::cout << std::endl;

    std::cout << "Clustering: 5 ..." << std::endl;
    TriangleMesh * mesh_lod_3 = mesh->VertexClustering(5);
    //TriangleMesh * mesh_lod_3 = mesh->VertexClustering_GRID(32);
    std::cout << "Preprocessing: 5 ..." << std::endl;
    mesh_lod_3->Preprocess();
    mesh_lod_3->sendToOpenGL(basicProgram);
    object_lod_3 = new GraphicsObject(mesh_lod_3);
    object_lod_3->SetPosition(glm::vec3(3, 0, 0));
    std::cout << std::endl;

    std::cout << "Clustering: 4 ..." << std::endl;
    TriangleMesh * mesh_lod_4 = mesh->VertexClustering(4);
    //TriangleMesh * mesh_lod_4 = mesh->VertexClustering_GRID(16);
    std::cout << "Preprocessing: 4 ..." << std::endl;
    mesh_lod_4->Preprocess();
    mesh_lod_4->sendToOpenGL(basicProgram);
    object_lod_4 = new GraphicsObject(mesh_lod_4);
    object_lod_4->SetPosition(glm::vec3(4, 0, 0));
    std::cout << std::endl;

    std::cout << "Clustering: 3 ..." << std::endl;
    TriangleMesh * mesh_lod_5 = mesh->VertexClustering(3);
    //TriangleMesh * mesh_lod_5 = mesh->VertexClustering_GRID(8);
    std::cout << "Preprocessing: 3 ..." << std::endl;
    mesh_lod_5->Preprocess();
    mesh_lod_5->sendToOpenGL(basicProgram);
    object_lod_5 = new GraphicsObject(mesh_lod_5);
    object_lod_5->SetPosition(glm::vec3(5, 0, 0));
    std::cout << std::endl;
    
	currentTime = 0.0f;
	
	camera.init(2.0f);
	
	bPolygonFill = true;
	
	// Select which font you want to use
	if(!text.init("fonts/OpenSans-Regular.ttf"))
	//if(!text.init("fonts/OpenSans-Bold.ttf"))
	//if(!text.init("fonts/DroidSerif.ttf"))
		cout << "Could not load font!!!" << endl;
}

bool Scene::loadMesh(const char *filename)
{
	//PLYReader reader;

	//object_->mesh_->free();
 //   bool bSuccess = reader.readMesh(filename, *(object_->mesh_));
	//if(bSuccess)
	//  object_->mesh_->sendToOpenGL(basicProgram);
	//

    std::cout << "Scene::loadMesh(): Code removed" << std::endl;

	return false;
}

void Scene::update(int deltaTime)
{
    frame_time_ = deltaTime;
	currentTime += deltaTime;
}

void Scene::render()
{

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR);
    
    /* Get the camera position and direction at world coordinates */
    glm::vec3 camera_position = camera.getPosition();
    glm::vec3 camera_direction = camera.getDirection();
    Ray3D camera_ray(Point3D({ camera_position.x, camera_position.y,camera_position.z }), Point3D({ camera_direction.x, camera_direction.y, camera_direction.z }));

    /* Cast rays on the two objects */
    object_->RayCast(camera_ray);
    object_lod_1->RayCast(camera_ray);
    object_lod_2->RayCast(camera_ray);
    object_lod_3->RayCast(camera_ray);
    object_lod_4->RayCast(camera_ray);
    object_lod_5->RayCast(camera_ray);

    /* Draw the objects */
    basicProgram.use();
    basicProgram.setUniformMatrix4f("projection", camera.getProjectionMatrix());
    basicProgram.setUniformMatrix4f("view", camera.getViewMatrix());
    
    glm::mat3 normalMatrix;
    glm::mat4 model;
    
    model = object_->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_->mesh_);
 
    model = object_lod_1->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_lod_1->mesh_);

    model = object_lod_2->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_lod_2->mesh_);

    model = object_lod_3->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_lod_3->mesh_);

    model = object_lod_4->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_lod_4->mesh_);

    model = object_lod_5->GetModel();
    basicProgram.setUniformMatrix4f("model", model);
    normalMatrix = glm::inverseTranspose(model * camera.getViewMatrix());
    basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
    renderPolygonMesh(object_lod_5->mesh_);

    text.render(std::to_string(fps_), glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
    /* 
        Draw something in the middle of the screen to see the ray. Adjust it because this is 
        a "cheap" way of doing that 
    */
    text.render("O", glm::vec2(1024 / 2 -4, 768 / 2 + 3), 10, glm::vec4(1, 0, 0, 1));
}

void Scene::renderPolygonMesh(TriangleMesh * mesh, glm::vec4 color) {
    mesh->render(basicProgram);
}

void Scene::renderRayCastingLine(glm::vec3 start, glm::vec3 end) {
    GLfloat vertices[] = {
        -0.5, -0.5, -0.5, 
        end.x, end.y, end.z,
        /*start.x, start.y, start.z,
        end.x, end.y, end.z*/
    };
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort elements[] = { 0, 1, 2, 3 };
    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 model = glm::mat4(1.0f);
    basicProgram.setUniformMatrix4f("model", model);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    GLint posLocation = basicProgram.bindVertexAttribute("position", 3, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(posLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glDrawElements(GL_LINE_LOOP, 2, GL_UNSIGNED_SHORT, 0);
    //glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    //glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDisableVertexAttribArray(posLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &ibo_elements);
}

Camera &Scene::getCamera()
{
  return camera;
}

void Scene::switchPolygonMode()
{
  bPolygonFill = !bPolygonFill;
}

void Scene::DisplayFps(size_t fps)
{
    fps_ = fps;
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/basic.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/basic.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}
