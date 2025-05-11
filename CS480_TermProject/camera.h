#ifndef CAMERA_H
#define CAMERA_H


#include "graphics_headers.h"


class Camera
{
public:
    Camera();
    ~Camera();
    bool Initialize1(int w, int h, glm::vec3 pos, glm::vec3 targetPos); // Initialize camera based on a target position
    bool Initialize2(int w, int h, glm::vec3 pos); // Initialize camera based on just a given position
    glm::mat4 GetProjection();
    glm::mat4 GetView();
    glm::vec3 getPos() { return camPos; }
    
    void SetPosition(glm::vec3 newPos);
    void SetFOV(float newFOV);
    void SetYaw(float newYaw);
    void SetUp(glm::vec3 up);

 
    void LookAt(glm::vec3 target);

    //void MoveRight();
    //void MoveLeft();
    void ProcessMouseScroll(float yoffset);

    //void ProcessMouseMovement(float xoffset, float yoffset);

    //void Update();
    //void MoveForward();
	//void MoveBackward();

    // NEW
    void FollowShip(glm::vec3 pos, glm::vec3 dir, float speed);
    //void ToggleFollowMode();
    void Update1(glm::vec3 targetPos, glm::vec3 offset, float angleStep); // Update camera position and angle with angle step
    void Update2(float xoffset, float yoffset, glm::mat4 targetModel, glm::vec3 camOffset); // Update camera position and angle with offsets



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
    float orbitYaw;

    float width, height; 

    //NEW
    //bool followShip = false;
    //glm::vec3 shipPos, shipDir;
    //float shipSpeed = 0.0f;



};


#endif /* CAMERA_H */