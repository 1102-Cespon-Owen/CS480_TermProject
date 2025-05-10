#ifndef CAMERA_H
#define CAMERA_H


#include "graphics_headers.h"


class Camera
{
public:
    Camera();
    ~Camera();
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();
    glm::vec3 getPos() { return camPos; }

    void MoveRight();
    void MoveLeft();
    void ProcessMouseScroll(float yoffset);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void Update();
    void MoveForward();
	void MoveBackward();



private:
    glm::mat4 projection;
    glm::mat4 view;

    // Varaibles for camera movement
    glm::vec3 camPos;
    glm::vec3 camFront;
    glm::vec3 camUp;
    float camSpeed;
    float dt;
    float lastFrame;
    float yaw;
    float pitch;
    float fov = 45.0f;

    float width, height; 

};


#endif /* CAMERA_H */