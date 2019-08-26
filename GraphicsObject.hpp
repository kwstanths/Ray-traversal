#ifndef __GraphicsObject_hpp__
#define __GraphicsObject_hpp__

#include <vector>

#include <glm/glm.hpp>

#include "TriangleMesh.h"

#include "Ray.hpp"
#include "PointOctree.hpp"

/*
    A GraphicsObject encapsulates a mesh in the world. It stores a pointer to 
    the actual triangular mesh, and its position
*/
class GraphicsObject {
public:

    GraphicsObject(TriangleMesh * mesh);
    ~GraphicsObject();

    void SetPosition(glm::vec3 position);

    glm::mat4 GetModel();

    int RayCast(Ray3D ray_world_space);

    TriangleMesh * mesh_ = nullptr;
private:
    /* Holds the position of the model */
    glm::mat4 translate_;

    /* Scaling of models is not implemented */
    glm::mat4 scale_;
};


#endif