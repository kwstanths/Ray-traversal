#ifndef _CAMERA_INCLUDE
#define _CAMERA_INCLUDE


#include <glm/glm.hpp>


// Camera contains the properies of the camera the scene is using
// It is responsible for computing the associated GL matrices


class Camera
{

public:
    Camera();
    ~Camera();

    void init(float initDistance, float initAngleX = 0.0f, float initAngleY = 0.0f);

    void resizeCameraViewport(int width, int height);
    void zoomCamera(float distDelta);

    void moveCamera(float front, float left);
    void rotateCamera(float xRotation, float yRotation);

    glm::mat4 &getProjectionMatrix();
    glm::mat4 &getViewMatrix();
    glm::vec3 getPosition();
    glm::vec3 getDirection();

private:
    void computeViewMatrix();

private:
    bool first_mouse_call_;
    bool warp_call_;
    int width_, height_;
    float last_cursor_position_x_, last_cursor_position_y_;
    float mouse_sensitivity_;
    float pitch_, yaw_;

    glm::mat4 projection, view;  // OpenGL matrices


    glm::vec3 position_;
    glm::vec3 direction_;
    glm::vec3 up_vector_;

};


#endif // _CAMERA_INCLUDE

