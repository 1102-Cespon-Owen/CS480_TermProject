#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include "graphics_headers.h"
#include "texture.h"
class Light
{
	public:
		Light();
		Light(glm::mat4 viewMatrix);

		glm::vec4 getGlobalAmbient() { return m_globalAmbient; }
		glm::vec4 getLightAmbient() { return m_lightAmbient; }
		glm::vec4 getLightDiffuse() { return m_lightDiffuse; }
		glm::vec3 getLightPosition() { return m_lightPosition; }
		glm::vec3 getLightPositionViewSpace() { return m_lightPositionViewSpace; }
		glm::vec4 getLightSpecular() { return m_lightSpecular; }

		void UpdateViewSpacePosition(glm::mat4 viewMatrix);


	private:
		glm::vec4 m_globalAmbient;
		glm::vec4 m_lightAmbient;
		glm::vec4 m_lightDiffuse;
		glm::vec3 m_lightPosition;
		glm::vec3 m_lightPositionViewSpace; 
		glm::vec4 m_lightSpecular;


};
#endif /* LIGHT_H */

