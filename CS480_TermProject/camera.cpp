#include "camera.h"


Camera::Camera()
{

}


Camera::~Camera()
{


}


bool Camera::Initialize(int w, int h)
{
    dt = 0.f;
    lastFrame = 0.f;
    yaw = -90.0f;
    pitch = 0.f;
    camPos = glm::vec3(0.0f, 10.0f, 16.0f);
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    width = w;
    height = h;

    view = glm::lookAt(camPos, //Eye position
        camPos + camFront, // Focus Point
        camUp); // Positive Y is up

    projection = glm::perspective(glm::radians(40.f), //the FoV typically 90 degrees is good which is what this is set to
        float(w) / float(h), //Aspect Ratio, so Circles stay Circular
        0.01f, //Distance to the near plane, normally a small value like this
        100.0f); //Distance to the far plane, 
    return true;
}


glm::mat4 Camera::GetProjection()
{
    return glm::perspective(glm::radians(fov), (float)width / (float)height, 0.01f, 100.0f); 
}


glm::mat4 Camera::GetView()
{
    return view;
}

void Camera::MoveRight() {
    camSpeed = 2.5f * dt;
    camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    view = glm::lookAt(camPos, camPos + camFront, camUp);
}

void Camera::MoveLeft() {
    camSpeed = 2.5f * dt;
    camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    view = glm::lookAt(camPos, camPos + camFront, camUp);
}

void Camera::MoveForward() {
	camSpeed = 2.5f * dt;
	camPos += camSpeed * camFront;
	view = glm::lookAt(camPos, camPos + camFront, camUp);
}

void Camera::MoveBackward() {
    camSpeed = 2.5f * dt;
    camPos -= camSpeed * camFront;
    view = glm::lookAt(camPos, camPos + camFront, camUp);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);

    view = glm::lookAt(camPos, camPos + camFront, camUp);
}

void Camera::Update() {
    float currentFrame = glfwGetTime();
    dt = currentFrame - lastFrame;
    lastFrame = currentFrame;
}


void Camera::ProcessMouseScroll(float yoffset) {
    fov -= yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

