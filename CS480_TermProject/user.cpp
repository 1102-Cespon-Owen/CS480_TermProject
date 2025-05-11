#include "user.h"
User::User() {
	
	dt = 0.f;
	lastFrame = 0.f;
	pivotLoc = glm::vec3(0.f, 0.f, 20.f);
	cameraOffsetExp = glm::vec3(0.f, 2.f, -5.f);
	cameraOffsetObs = glm::vec3(0.f, 0.0f, 100.f);
	m_cameraExp = new Camera();
	m_cameraObs = new Camera();
	m_cameraExp->Initialize1(1600, 1200, pivotLoc + cameraOffsetExp, pivotLoc);
	m_cameraObs->Initialize2(1600, 1200, pivotLoc + cameraOffsetObs);


	model = glm::translate(glm::mat4(1.0f), pivotLoc);
	model *= glm::scale(glm::vec3(0.001f, 0.001f, 0.001f));
	ship = new Mesh(pivotLoc, "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");
	ship->Update(model);
	shipSpeed = 0.0f;
	shipAcceleration = 0.05f;

	mode = EXPLORATION;
	
}


User::User(glm::vec3 pivot) {
	dt = 0.f;
	lastFrame = 0.f;
	pivotLoc = glm::vec3(0.f, 0.f, 20.f) + pivot;
	cameraOffsetExp = glm::vec3(0.f, 2.f, -5.f);
	cameraOffsetObs = glm::vec3(0.f, 0.0f, 100.f);
	m_cameraExp = new Camera();
	m_cameraObs = new Camera();
	glm::vec3 camExpPos = pivotLoc + cameraOffsetExp;
	glm::vec3 camObsPos = pivotLoc + cameraOffsetObs;
	m_cameraExp->Initialize1(1600, 1200, camExpPos, pivotLoc);
	m_cameraObs->Initialize2(1600, 1200, camObsPos);


	model = glm::translate(glm::mat4(1.0f), pivotLoc);
	model *= glm::scale(glm::vec3(0.001f, 0.001f, 0.001f));
	ship = new Mesh(pivotLoc, "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");
	ship->Update(model);
	shipSpeed = 0.0f;
	shipAcceleration = 0.05f;


	mode = EXPLORATION;
}


User::~User() {
	delete m_cameraExp;
	delete m_cameraObs;
	delete ship;
}

Camera* User::getExpCamera() {
	return m_cameraExp;
}

Camera* User::getObsCamera() {
	return m_cameraObs;
}

Mesh* User::getMesh() {
	return ship;
}

CameraMode User::getCameraMode() {
	return mode;
}

glm::mat4 User::getModel() {
	return model;
}

void User::ToggleCameraMode() {
	if (mode == EXPLORATION) {
		mode = OBSERVATION;
	}
	else if (mode == OBSERVATION) {
		mode = EXPLORATION;
		resetObsCamera();
	}
}


void User::MoveForward() {
	if (mode == EXPLORATION) {
		glm::vec3 forward = glm::normalize(glm::vec3(orientation * glm::vec4(0, 0, 1, 0)));
		glm::vec3 newPos = glm::vec3(model[3]) + forward * shipSpeed;

		model = glm::translate(glm::mat4(1.0f), newPos);
		model *= glm::scale(glm::vec3(0.001f));
		model *= orientation;

		ship->Update(model);
	}
	
}


void User::Accelerate() {

	if (mode == EXPLORATION) {
		shipSpeed += shipAcceleration * dt;
		shipSpeed = ((maxShipSpeed) < (shipSpeed) ? (maxShipSpeed) : (shipSpeed));
	}
}

void User::Brake() {
	if (mode == EXPLORATION) {
		shipSpeed -= shipAcceleration * dt;
		shipSpeed = ((0.0f) > (shipSpeed) ? (0.0f) : (shipSpeed));
	}

}

void User::Stop() {
	if (mode == EXPLORATION) {
		shipSpeed = 0.0f;
	}
}


void User::RollLeft() {
	glm::vec3 forward = glm::normalize(glm::vec3(orientation * glm::vec4(0, 0, 1, 0)));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), 0.02f, forward);
	orientation = rot * orientation;
}

void User::RollRight() {
	glm::vec3 forward = glm::normalize(glm::vec3(orientation * glm::vec4(0, 0, 1, 0)));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), -0.02f, forward);
	orientation = rot * orientation;
}

void User::YawLeft() {
	glm::vec3 up = glm::normalize(glm::vec3(orientation * glm::vec4(0, 1, 0, 0)));
	glm::mat4 yawRot = glm::rotate(glm::mat4(1.0f), 0.002f, up); // local up
	orientation = yawRot * orientation;

}

void User::YawRight() {
	glm::vec3 up = glm::normalize(glm::vec3(orientation * glm::vec4(0, 1, 0, 0)));
	glm::mat4 yawRot = glm::rotate(glm::mat4(1.0f), -0.002f, up);
	orientation = yawRot * orientation;
}


void User::ProcessMouseMovement(float xoffset, float yoffset) {
	if (mode == EXPLORATION) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		// Get local axes from current orientation
		glm::vec3 right = glm::normalize(glm::vec3(orientation * glm::vec4(1, 0, 0, 0))); // local X
		glm::vec3 up = glm::normalize(glm::vec3(orientation * glm::vec4(0, 1, 0, 0)));     // local Y

		// Apply yaw (around local up)
		glm::mat4 yawRot = glm::rotate(glm::mat4(1.0f), -xoffset, up);
		orientation = yawRot * orientation;

		// Apply pitch (around local right)
		glm::mat4 pitchRot = glm::rotate(glm::mat4(1.0f), -yoffset, right);
		orientation = pitchRot * orientation;

		applyOrientation(); // update model

		// Update camera
		glm::vec3 shipPos = glm::vec3(model[3]);
		glm::vec3 shipFront = glm::normalize(glm::vec3(model * glm::vec4(0, 0, 1, 0)));
		glm::vec3 shipUp = glm::normalize(glm::vec3(model * glm::vec4(0, 1, 0, 0)));
		glm::vec3 camPos = shipPos - shipFront * 5.0f + shipUp * 2.0f;

		m_cameraExp->SetPosition(camPos);
		m_cameraExp->LookAt(shipPos);
	}
	else if (mode == OBSERVATION) {
		m_cameraObs->Update2(xoffset, yoffset, model, cameraOffsetObs);
	}
}



void User::ProcessMouseScroll(float yoffset) {
	if (mode == OBSERVATION) {
		m_cameraObs->ProcessMouseScroll(yoffset);
	}
}


void User::resetObsCamera() {
	glm::vec3 cameraObsResPos = model[3];
	cameraObsResPos += cameraOffsetObs;
	m_cameraObs->SetPosition(cameraObsResPos);
	m_cameraObs->SetYaw(40.0f);
	m_cameraObs->SetFOV(90.0f);
}

void User::UpdateDT() {
	float currentFrame = glfwGetTime();
	dt = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void User::updateModel() {
	applyOrientation();

	ship->Update(model);

	if (mode == EXPLORATION) {
		glm::vec3 shipPos = glm::vec3(model[3]);
		glm::vec3 shipFront = glm::normalize(glm::vec3(model * glm::vec4(0, 0, 1, 0)));
		glm::vec3 shipUp = glm::normalize(glm::vec3(model * glm::vec4(0, 1, 0, 0)));

		float speedRatio = glm::clamp(shipSpeed / maxShipSpeed, 0.0f, 1.0f);
		float intensity = pow(speedRatio, 4.5);  // even sharper curve

		float distBehind = glm::mix(0.5f, 5.0f, intensity);   // much closer
		float heightAbove = glm::mix(0.1f, 2.0f, intensity);  // subtle change

		glm::vec3 camPos = shipPos - shipFront * distBehind + shipUp * heightAbove;
		glm::vec3 lookAt = shipPos + shipFront * 0.5f - shipUp * 0.1f;


		m_cameraExp->SetPosition(camPos);
		m_cameraExp->SetUp(shipUp);
		m_cameraExp->LookAt(lookAt);
	}
	

}


void User::applyOrientation() {
	glm::vec3 shipPos = glm::vec3(model[3]);
	model = glm::translate(glm::mat4(1.0f), shipPos);
	model *= glm::scale(glm::vec3(0.001f));
	model *= orientation;
}

