#ifndef USER_H
#define USER_H

#include "camera.h"
#include "mesh.h"
#include "graphics_headers.h"

enum CameraMode {
	EXPLORATION,
	OBSERVATION
};

class User
{
	public:
		User();
		User(glm::vec3 pivot);
		~User();

		void UpdateDT();
		void ToggleCameraMode();

		void ProcessMouseMovement(float xoffset, float yoffset);
		void ProcessMouseScroll(float yoffset);

		Camera* getExpCamera();
		Camera* getObsCamera();
		Mesh* getMesh();
		CameraMode getCameraMode();

		glm::mat4 getModel();

		void updateModel();
		void MoveForward();
		void Accelerate();
		void Brake();
		void RollLeft();
		void RollRight();
		void Stop();
		void YawLeft();
		void YawRight();

		void applyOrientation();


		//void moveExpLeft();
		//void moveExpRight();
		void resetObsCamera();


	private:
		glm::mat4 model;
		glm::vec3 pivotLoc;
		glm::vec3 cameraOffsetExp;
		glm::vec3 cameraOffsetObs;

		Camera* m_cameraExp;
		Camera* m_cameraObs;
		Mesh* ship;

		float sensitivity = 0.003f;

		CameraMode mode;

		float dt;
		float lastFrame;
		float yaw;
		float pitch;
		float angle;
		float shipAcceleration;

		//maybe use
		glm::mat4 obsPlanetModel;

		float obsYaw, obsPitch;
		glm::vec3 obsPos;
		glm::vec3 obsFront;
		glm::vec3 obsUp;

		//glm::vec3 shipPosition;
		float shipSpeed = 0.f;
		float maxShipSpeed = 1.0f;
		float shipYaw = -90.f;
		float shipPitch = 0.f;
		float shipRoll = 0.f;

};
#endif /* SCENE_H  */

