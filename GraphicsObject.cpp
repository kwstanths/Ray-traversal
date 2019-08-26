#include "GraphicsObject.hpp"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


GraphicsObject::GraphicsObject(TriangleMesh * mesh) {
    mesh_ = mesh;
    scale_ = glm::mat4(1.0f);
    translate_ = glm::mat4(1.0f);
}

GraphicsObject::~GraphicsObject() {
    
}

void GraphicsObject::SetPosition(glm::vec3 position) {
    translate_ = glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 GraphicsObject::GetModel() {
    return translate_;
}

int GraphicsObject::RayCast(Ray3D ray_world_space) {

    /*
        The ray is at world coordinates. We have to transform it
        local object space coordinates. Remember: The octree is always 
        centered at the origin, object space.
    */

    glm::vec3 world_origin = glm::vec3(ray_world_space.Origin()[0], ray_world_space.Origin()[1], ray_world_space.Origin()[2]);
    glm::vec3 world_directon = glm::vec3(ray_world_space.Direction()[0], ray_world_space.Direction()[1], ray_world_space.Direction()[2]);

    /* Transform to object space */
    glm::vec3 object_space_position = glm::vec3(glm::inverse(translate_) * glm::vec4(world_origin, 1));
    glm::vec3 object_space_direction = glm::vec3(glm::inverse(translate_) * glm::vec4(world_directon, 0));

    Ray3D camera_ray_object_space(
        Point3D({ object_space_position.x, object_space_position.y, object_space_position.z }), 
        Point3D({ object_space_direction.x, object_space_direction.y, object_space_direction.z })
    );

    return mesh_->RayCast(camera_ray_object_space);
}
