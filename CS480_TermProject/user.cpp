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

	angle = 0.0f;
	model = glm::translate(glm::mat4(1.0f), pivotLoc);
	model *= glm::scale(glm::vec3(0.001f, 0.001f, 0.001f));
	ship = new Mesh(pivotLoc, "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");
	ship->Update(model);
	shipSpeed = 0.0f;
	shipAcceleration = 0.05f;
	yaw = 0.0f;
	pitch = 0.0f; 

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

	angle = 0.0f;
	model = glm::translate(glm::mat4(1.0f), pivotLoc);
	model *= glm::scale(glm::vec3(0.001f, 0.001f, 0.001f));
	ship = new Mesh(pivotLoc, "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");
	ship->Update(model);
	shipSpeed = 0.0f;
	shipAcceleration = 0.05f;
	yaw = 0.0f;
	pitch = 0.0f;

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
	shipRoll = 0.0f; // Reset ship roll when switching modes
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
		// Get ship forward direction (local -Z transformed by model)
		glm::vec3 forward = glm::normalize(glm::vec3(model * glm::vec4(0, 0, 1, 0)));

		// Move the ship forward along its front vector
		glm::vec3 newPos = glm::vec3(model[3]) + forward * (shipSpeed); // removed * scale

		// Rebuild the model at new position with current rotation
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));
		rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0));
		rotation = glm::rotate(rotation, shipRoll, glm::vec3(0, 0, 1));

		model = glm::translate(glm::mat4(1.0f), newPos);
		model *= glm::scale(glm::vec3(0.001f));
		model *= rotation;

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
	shipRoll += 0.02f; // roll left
}

void User::RollRight() {
	shipRoll -= 0.02f; // roll right
}

void User::YawLeft() {
	yaw += 0.002f;
}

void User::YawRight() {
	yaw += -0.002f;
}


void User::ProcessMouseMovement(float xoffset, float yoffset) {
	if (mode == EXPLORATION) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw -= xoffset;
		pitch -= yoffset;

		// Apply yaw and pitch to model matrix
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));
		rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0));
		rotation = glm::rotate(rotation, shipRoll, glm::vec3(0, 0, 1)); // <--- add this line


		glm::vec3 shipPos = glm::vec3(model[3]);
		model = glm::translate(glm::mat4(1.0f), shipPos);
		model *= glm::scale(glm::vec3(0.001f));
		model *= rotation;

		ship->Update(model);

		// Get ship's "back" direction from model matrix
		glm::vec3 shipFront = glm::normalize(glm::vec3(model * glm::vec4(0, 0, 1, 0))); // ship looks down -Z
		glm::vec3 shipUp = glm::normalize(glm::vec3(model * glm::vec4(0, 1, 0, 0)));

		// Position camera behind the ship
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

	// Start from identity
	glm::mat4 rotation = glm::mat4(1.0f);

	// Apply roll, then pitch, then yaw in ship's local frame
	rotation = glm::rotate(rotation, shipRoll, glm::vec3(0, 0, 1)); // roll around local Z
	rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0));    // pitch around local X
	rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));      // yaw around local Y

	// Apply translation and scale
	model = glm::translate(glm::mat4(1.0f), shipPos);
	model *= glm::scale(glm::vec3(0.001f));
	model *= rotation;

	ship->Update(model);
	
}
