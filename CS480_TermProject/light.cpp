#include "light.h"

Light::Light() {
	// Default light position (world space)
	m_lightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	m_lightPositionViewSpace = m_lightPosition;

	// Lighting properties
	m_globalAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightAmbient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	m_lightDiffuse = glm::vec4(1.2f, 1.2f, 1.2f, 1.0f);
	m_lightSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

Light::Light(glm::mat4 viewMatrix) {
	// Same as default, then update view-space position
	m_lightPosition = glm::vec3(0.0f, 0.0f, 0.0f); // was 0, 0, 10
	m_globalAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightAmbient = glm::vec4(0.4, 0.4f, 0.4f, 1.0f);
	m_lightDiffuse = glm::vec4(1.2f, 1.2f, 1.2f, 1.0f);
	m_lightSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	UpdateViewSpacePosition(viewMatrix);
	
}
void Light::UpdateViewSpacePosition(glm::mat4 viewMatrix) {
	glm::vec4 posVS = viewMatrix * glm::vec4(m_lightPosition, 1.0f);
	m_lightPositionViewSpace = glm::vec3(posVS);
}
