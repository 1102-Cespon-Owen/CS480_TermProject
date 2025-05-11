#include "graphics.h"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif


	//Create USER
	m_user = new User();



	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	m_light = new Light(m_user->getExpCamera()->GetView());
	if (m_light == NULL) {
		printf("Light failed to initialize\n");
		return false;
	}

	//Sky Box (SPACE)
	m_space = new Sphere(128, "Cubemaps/Galaxy2.jpg", NULL);
	m_spaceMat = {
		glm::vec4(0.1, 0.1, 0.1, 1.0), // ambient
		glm::vec4(0.1, 0.1, 0.1, 1.0), // diffuse
		glm::vec4(0.1, 0.1, 0.1, 1.0), // specular
		16.0f
	};


	//Starship
	m_ship = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");

	// The Sun
	m_sun = new Sphere(64, "Planetary_Textures/2k_sun.jpg", NULL);
	m_sunMat = {
	glm::vec4(0.6, 0.5, 0.1, 1.0), // ambient
	glm::vec4(1.0, 0.9, 0.3, 1.0), // diffuse
	glm::vec4(1.0, 1.0, 0.8, 1.0), // specular
	16.0f
	};


	// The Earth
	m_earth = new Sphere(48, "Planetary_Textures/2k_earth_daymap.jpg", "Planetary_Textures/2k_earth_daymap-n.jpg");
	m_earthMat = {
	glm::vec4(0.3, 0.3, 0.3, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	32.0f
	};

	// The moon
	//m_moon = new Sphere(48, "Planetary_Textures/2k_moon.jpg");



	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void Graphics::HierarchicalUpdate2(double dt) {
	glm::mat4 tmat, rmat, smat, localTransform;
	
	// === SKY SPHERE ===
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));  // Sun at origin
	localTransform = modelStack.top();
	localTransform *= glm::scale(glm::vec3(70.f));
	if (m_space) m_space->Update(localTransform);
	modelStack.pop();  // return to empty


	// === SUN ===
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));  // Sun at origin
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), (float)(0.5 * dt), glm::vec3(0.f, 1.f, 0.f));
	localTransform *= glm::scale(glm::vec3(1.25f));
	if (m_sun) m_sun->Update(localTransform);
	
	
	// === EARTH ===
	std::vector<float> earthSpeed = { .5f, .5f, .5f };
	std::vector<float> earthDist = { 6.f, 0.f, 6.f };
	glm::vec3 earthRotVector = { 0.f, 1.f, 0.f };
	std::vector<float> earthRotSpeed = { .2f };
	std::vector<float> earthScale = { 0.75f, 0.75f, 0.75f };

	ComputeTransforms(dt, earthSpeed, earthDist, earthRotSpeed, earthRotVector, earthScale, tmat, rmat, smat);
	localTransform = modelStack.top() * tmat;
	modelStack.push(localTransform);
	localTransform *= rmat;
	localTransform *= smat;
	if (m_earth) m_earth->Update(localTransform);


	 /*
	// === MOON ===
	std::vector<float> moonSpeed = { 3.f, 3.f, 3.f };
	std::vector<float> moonDist = { 1.25f, 1.25f, 0.75f };
	glm::vec3 moonRotVector = { 0.f, 1.f, 1.f };
	std::vector<float> moonRotSpeed = { 3.f };
	std::vector<float> moonScale = { 0.15f, 0.15f, 0.15f };

	ComputeTransforms(dt, moonSpeed, moonDist, moonRotSpeed, moonRotVector, moonScale, tmat, rmat, smat);
	localTransform = modelStack.top() * tmat;
	modelStack.push(localTransform);
	localTransform *= rmat;
	localTransform *= smat;
	if (m_moon) m_moon->Update(localTransform);

	modelStack.pop();  // back to Earth
	modelStack.pop();  // back to Sun
	*/
	modelStack.pop();  // return to sun
	
	modelStack.pop(); // empty stack

	m_user->UpdateDT();
	m_user->MoveForward();
}


void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist, 
	std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 
			sin(speed[1] * dt) * dist[1], 
			sin(speed[2] * dt) * dist[2]));
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}


void Graphics::Render()
{
	// Clear the screen
	glClearColor(0.5, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable shader
	m_shader->Enable();

	// === Select current camera based on mode ===
	Camera* cam = (m_user->getCameraMode() == EXPLORATION)
		? m_user->getExpCamera()
		: m_user->getObsCamera();

	// === Force view matrix update using LookAt and correct up vector ===
	if (m_user->getCameraMode() == EXPLORATION)
	{
		glm::vec3 shipPos = glm::vec3(m_user->getModel()[3]);
		cam->LookAt(shipPos);  // Rebuilds view using camUp set earlier in updateModel()
	}

	// === Upload projection and view matrices to shader ===
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(cam->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(cam->GetView()));

	// === Set lighting camera position ===
	glUniform3fv(m_shader->GetUniformLocation("viewPos"), 1, glm::value_ptr(cam->getPos()));
	glUniform3fv(m_shader->GetUniformLocation("lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
	glUniform3fv(m_shader->GetUniformLocation("lightPos"), 1, glm::value_ptr(glm::vec3(0.0f)));

	// === Set light uniforms ===
	glUniform4fv(m_shader->GetUniformLocation("GlobalAmbient"), 1, glm::value_ptr(m_light->getGlobalAmbient()));
	glUniform4fv(m_shader->GetUniformLocation("light.ambient"), 1, glm::value_ptr(m_light->getLightAmbient()));
	glUniform4fv(m_shader->GetUniformLocation("light.diffuse"), 1, glm::value_ptr(m_light->getLightDiffuse()));
	glUniform4fv(m_shader->GetUniformLocation("light.spec"), 1, glm::value_ptr(m_light->getLightSpecular()));
	glUniform3fv(m_shader->GetUniformLocation("light.position"), 1, glm::value_ptr(m_light->getLightPositionViewSpace()));

	// === Render ship ===
	if (m_user->getMesh())
	{
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_user->getMesh()->GetModel()));

		if (m_user->getMesh()->hasTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_user->getMesh()->getTextureID());
			glUniform1i(m_shader->GetUniformLocation("sp"), 0);
			glUniform1i(m_hasTexture, true);
		}
		else
		{
			glUniform1i(m_hasTexture, false);
		}

		glUniform1i(m_hasNormalMap, false); // Assuming ship has no normal map
		m_user->getMesh()->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}

	// === Render sky (space sphere) ===
	if (m_space)
	{
		glm::mat4 model = m_space->GetModel();
		glm::mat4 view = cam->GetView();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		if (m_space->hasTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_space->getTextureID());
			glUniform1i(m_shader->GetUniformLocation("sp"), 0);
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 1);
		}
		else
		{
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 0);
		}

		glUniform1i(m_shader->GetUniformLocation("hasNormalMap"), 0); // Space doesn't use normal map
		SetMaterial(m_spaceMat);
		m_space->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}

	// === Render Sun ===
	if (m_sun)
	{
		glm::mat4 model = m_sun->GetModel();
		glm::mat4 view = cam->GetView();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		if (m_sun->hasTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			glUniform1i(m_shader->GetUniformLocation("sp"), 0);
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 1);
		}
		else
		{
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 0);
		}

		glUniform1i(m_shader->GetUniformLocation("hasNormalMap"), 0); // no normal map
		SetMaterial(m_sunMat);
		m_sun->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}

	// === Render Earth ===
	if (m_earth)
	{
		glm::mat4 model = m_earth->GetModel();
		glm::mat4 view = cam->GetView();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		if (m_earth->hasTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());
			glUniform1i(m_shader->GetUniformLocation("sp"), 0);
			glUniform1i(m_hasTexture, true);
		}
		else
		{
			glUniform1i(m_hasTexture, false);
		}

		if (m_earth->hasNormalTex)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_earth->getNormalTextureID());
			glUniform1i(m_shader->GetUniformLocation("normalMap"), 1);
			glUniform1i(m_hasNormalMap, true);
		}
		else
		{
			glUniform1i(m_hasNormalMap, false);
		}

		SetMaterial(m_earthMat);
		m_earth->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}

	// === Error checking ===
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL Error: " << ErrorString(error) << "\n";
	}
}


bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	// Locate the normal matrix in the shader
	m_normalMatrix = m_shader->GetUniformLocation("normalMatrix");
	if (m_normalMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_normalMatrix not found\n");
		anyProblem = false;
	}

	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == -1)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_normalAttrib = m_shader->GetAttribLocation("v_normal");
	if (m_normalAttrib == -1)
	{
		printf("v_normal attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_tc");
	if (m_tcAttrib == -1)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}

	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		anyProblem = false;
	}

	m_hasNormalMap = m_shader->GetUniformLocation("hasNormalMap");
	if (m_hasNormalMap == INVALID_UNIFORM_LOCATION) {
		printf("hasNormalMap uniform not found\n");
		anyProblem = false; 
	}


	/////////////////////////////////////////////////
	// Locate the light uniforms
	GLuint lightAloc = m_shader->GetUniformLocation("light.ambient");
	if (lightAloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightAmbient not found\n");
		anyProblem = false;
	}

	GLuint lightDloc = m_shader->GetUniformLocation("light.diffuse");
	if (lightDloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightDiffuse not found\n");
		anyProblem = false;
	}

	GLuint lightSloc = m_shader->GetUniformLocation("light.spec");
	if (lightSloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightSpecular not found\n");
		anyProblem = false;
	}

	GLuint lightPloc = m_shader->GetUniformLocation("light.position");
	if (lightPloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightPosition not found\n");
		anyProblem = false;
	}

	GLuint globalAmbLoc = m_shader->GetUniformLocation("GlobalAmbient");
	if (globalAmbLoc == INVALID_UNIFORM_LOCATION)
	{
		printf("GlobalAmbient not found\n");
		anyProblem = false;
	}

	


	return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}



void Graphics::SetMaterial(const Material& mat) {
	GLint mAmbLoc = m_shader->GetUniformLocation("material.ambient");
	GLint mDiffLoc = m_shader->GetUniformLocation("material.diffuse");
	GLint mSpecLoc = m_shader->GetUniformLocation("material.specular");
	GLint mShineLoc = m_shader->GetUniformLocation("material.shininess");

	glUniform4fv(mAmbLoc, 1, glm::value_ptr(mat.ambient));
	glUniform4fv(mDiffLoc, 1, glm::value_ptr(mat.diffuse));
	glUniform4fv(mSpecLoc, 1, glm::value_ptr(mat.specular));
	glUniform1f(mShineLoc, mat.shininess);
}
