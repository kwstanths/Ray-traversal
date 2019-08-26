#include "Camera.h"

#include <iostream>

#include <GL/glut.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


#define PI 3.14159f


Camera::Camera()
{
    
}

Camera::~Camera()
{
}


void Camera::init(float initDistance, float initAngleX, float initAngleY)
{
    first_mouse_call_ = true;
    warp_call_ = false;

    mouse_sensitivity_ = 0.05f;
    yaw_ = -89.0f;
    pitch_ = 0.0f;

    position_ = glm::vec3(0, 0.1, 3);
    //direction_ = glm::vec3(0, 0, 1);
    up_vector_ = glm::vec3(0, 1, 0);

    computeViewMatrix();
}

void Camera::resizeCameraViewport(int width, int height)
{
    width_ = width;
    height_ = height;
    projection = glm::perspective(45.f / 180.f * PI, float(width) / float(height), 0.01f, 100.0f);
}

void Camera::moveCamera(float front, float left) {
    /* Calculate the amount of front movement */
    position_ = position_ + front * direction_;

    /* Calculate the left vector */
    glm::vec3 left_vector = glm::cross(direction_, up_vector_);

    /* Calculate the amount of left movement */
    position_ = position_ + left_vector * left;

    computeViewMatrix();
}

void Camera::rotateCamera(float xRotation, float yRotation)
{
    if (first_mouse_call_) {
        last_cursor_position_x_ = xRotation;
        last_cursor_position_y_ = yRotation;
        first_mouse_call_ = false;
    }

    if (warp_call_) {
        warp_call_ = false;

        last_cursor_position_x_ = static_cast<float>(width_) / 2;
        last_cursor_position_y_ = static_cast<float>(height_) / 2;

        return;
    }

    float xoffset = xRotation - last_cursor_position_x_;
    float yoffset = last_cursor_position_y_ - yRotation;
    last_cursor_position_x_ = xRotation;
    last_cursor_position_y_ = yRotation;

    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    direction_.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    direction_.y = sin(glm::radians(pitch_));
    direction_.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
    direction_ = glm::normalize(direction_);

    computeViewMatrix();
    warp_call_ = true;
}

void Camera::zoomCamera(float distDelta)
{
    computeViewMatrix();
}

void Camera::computeViewMatrix()
{
    view = glm::lookAt(position_, position_ + direction_, up_vector_);
}

glm::mat4 &Camera::getProjectionMatrix()
{
    return projection;
}

glm::mat4 &Camera::getViewMatrix()
{
    return view;
}

glm::vec3 Camera::getPosition() {
    return position_;
}

glm::vec3 Camera::getDirection() {
    return direction_;
}


