#include "camera.h"


Camera::Camera()
{

}


Camera::~Camera()
{


}


bool Camera::Initialize1(int w, int h, glm::vec3 pos, glm::vec3 targetPos)
{
    dt = 0.f;
    lastFrame = 0.f;
    yaw = -90.0f;
    pitch = 0.f;
    fov = 40.0f;
	orbitYaw = 0.f;
    camPos = pos;
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    width = w;
    height = h;

    view = glm::lookAt(camPos, //Eye position
        targetPos, // Focus Point
        camUp); // Positive Y is up

    projection = glm::perspective(glm::radians(fov), //the FoV typically 90 degrees is good which is what this is set to
        float(w) / float(h), //Aspect Ratio, so Circles stay Circular
        0.01f, //Distance to the near plane, normally a small value like this
        100.0f); //Distance to the far plane, 
    return true;
}

bool Camera::Initialize2(int w, int h, glm::vec3 pos)
{
    dt = 0.f;
    lastFrame = 0.f;
    yaw = 90.0f;
    pitch = 0.f;
    fov = 40.0f;
    orbitYaw = 0.0f;
    camPos = pos;
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camPos, //Eye Position
        camPos + camFront, //Focus point
        camUp); //Positive Y is up

    projection = glm::perspective(glm::radians(fov), //the FoV typically 90 degrees is good which is what this is set to
        float(w) / float(h), //Aspect Ratio, so Circles stay Circular
        0.01f, //Distance to the near plane, normally a small value like this
        100.0f); //Distance to the far plane, 
    return true;
}

glm::mat4 Camera::GetProjection()
{
    return projection;
}


glm::mat4 Camera::GetView()
{
    return view;
}

void Camera::SetPosition(glm::vec3 newPos) {
    camPos = newPos;
}

void Camera::SetFOV(float newFov) {
    fov = newFov;
}

void Camera::SetYaw(float newYaw) {
    yaw = newYaw;
}

/*
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
*/

/*

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
*/
void Camera::Update1(glm::vec3 targetPos, glm::vec3 offset, float angleStep) {
    // Creates an orbit rotation for third-person view
    orbitYaw += glm::radians(angleStep);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), orbitYaw, glm::vec3(0, 1, 0));
    glm::vec3 rotatedOffset = glm::vec3(rotation * glm::vec4(offset, 1.0f));
    camPos = targetPos + rotatedOffset;
    view = glm::lookAt(camPos, targetPos, camUp);
}

void Camera::Update2(float xoffset, float yoffset, glm::mat4 targetModel, glm::vec3 camOffset) {
    yaw += xoffset;
    pitch += yoffset;

    // Limit pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Get direction based on x and y offsets
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);

    // Get rotation matrix from target model matrix
    glm::mat3 rotation = glm::mat3(targetModel);

    // Update camera position and up vector to match target object's
    camPos = glm::vec3(targetModel[3]) + (rotation * camOffset);
    glm::mat3 rotationMatrix = glm::mat3(targetModel);
    camUp = glm::normalize(glm::vec3(targetModel[1]));


    view = glm::lookAt(camPos, camPos + camFront + glm::normalize(glm::vec3(targetModel[2])), camUp);
}

void Camera::ProcessMouseScroll(float yoffset) {
    fov -= yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;

    projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
}


void Camera::FollowShip(glm::vec3 pos, glm::vec3 dir, float speed) {

    // Offset the camera behind and slightly above the ship
    float dist = glm::clamp(speed * 1.5f, 10.f, 40.f);
    glm::vec3 offset = glm::normalize(dir) * dist + glm::vec3(0.0f, 3.0f, 0.0f);

    camPos = pos + offset;                 // new camera position
    camFront = glm::normalize(pos - camPos); // look at the ship
    view = glm::lookAt(camPos, pos, camUp);  // recalculate view matrix

    // Dynamically adjust FOV with speed (optional)
    fov = glm::clamp(45.f + speed * 0.3f, 45.f, 80.f);
}


void Camera::LookAt(glm::vec3 target) {
    camFront = glm::normalize(target - camPos);
    view = glm::lookAt(camPos, target, camUp);
}

void Camera::SetUp(glm::vec3 up) {
    camUp = up;
}