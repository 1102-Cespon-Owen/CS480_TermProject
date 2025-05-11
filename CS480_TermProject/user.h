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
	
		glm::mat4 orientation = glm::mat4(1.0f); // declared in header


		float shipSpeed = 0.f;
		float maxShipSpeed = 1.0f;
		float shipAcceleration;


};
#endif /* SCENE_H  */

