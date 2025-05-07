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



	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

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

	m_light = new Light(m_camera->GetView());
	if (m_light == NULL) {
		printf("Light failed to initialize\n");
		return false;
	}

	//Sky Box (SPACE)
	//m_space = new Sphere(256, "Cubemaps/Galaxy2.jpg", NULL); 

	//Starship
	//m_ship = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets/SpaceShip-1.obj", "assets/SpaceShip-1.png");

	// The Sun
	m_sun = new Sphere(64, "Planetary_Textures/2k_sun.jpg", NULL);

	// The Earth
	m_earth = new Sphere(48, "Planetary_Textures/2k_earth_daymap.jpg", "Planetary_Textures/2k_earth_daymap-n.jpg");
	Material earthMat = {
	glm::vec4(0.2, 0.2, 0.2, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	20.0f
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
	/*
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));  // Sun at origin
	localTransform = modelStack.top();
	localTransform *= glm::scale(glm::vec3(50.f));
	if (m_space) m_space->Update(localTransform);
	modelStack.pop();  // return to empty
	*/


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


	//////////////////////////////// NEED TO ADJUST STARSHIP ORBIT AND ORIENTATION
	// === STARSHIP ===    
	std::vector<float> shipSpeed = { 0.5, 0.5, 0.5 };
	std::vector<float> shipDist = { 0, 2., 2. };
	glm::vec3 shipRotVector = { 1.,0.,0. };
	std::vector<float> shipRotSpeed = { 0.5, 0.5, 0.5 };
	std::vector<float> shipScale = { .01f, .01f, .01f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(shipSpeed[0] * dt) * shipDist[0], cos(shipSpeed[1] * dt) * shipDist[1], sin(shipSpeed[2] * dt) * shipDist[2]));
	modelStack.push(localTransform);
	localTransform *= glm::rotate(glm::mat4(1.f), shipRotSpeed[0] * (float)dt, shipRotVector);
	localTransform *= glm::scale(glm::vec3(shipScale[0], shipScale[1], shipScale[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(90.f), shipRotVector);


	if (m_ship != NULL)
		m_ship->Update(localTransform);
	*/
	modelStack.pop();  // return to sun
	
	modelStack.pop(); // empty stack
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
	//clear the screen
	glClearColor(0.5, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));


	///////////// NEW FOR LIGHT /////////////////////////////
	// Update light position in view space
	m_light->UpdateViewSpacePosition(m_camera->GetView());

	//getting location and sending to light object
	GLuint globalAmbLoc = glGetUniformLocation(m_shader->GetShaderProgram(), "GlobalAmbient");
	if (globalAmbLoc == INVALID_UNIFORM_LOCATION)
	{
		printf("GlobalAmbient not found\n");
	}
	else {
		glUniform4fv(m_shader->GetUniformLocation("GlobalAmbient"), 1, glm::value_ptr(m_light->getGlobalAmbient()));
	}


	GLuint lightAloc = glGetUniformLocation(m_shader->GetShaderProgram(), "light.ambient");
	if (lightAloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightAmbient not found\n");
	}
	else {
		glUniform4fv(m_shader->GetUniformLocation("light.ambient"), 1, glm::value_ptr(m_light->getLightAmbient()));
	}

	GLuint lightDloc = glGetUniformLocation(m_shader->GetShaderProgram(), "light.diffuse");
	if (lightDloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightDiffuse not found\n");
	}
	else {
		//glProgramUniform4fv(m_shader->GetShaderProgram(), lightDloc, 1, glm::value_ptr(m_light->getLightDiffuse()));
		glUniform4fv(m_shader->GetUniformLocation("light.diffuse"), 1, glm::value_ptr(m_light->getLightDiffuse()));

	}

	GLuint lightSloc = glGetUniformLocation(m_shader->GetShaderProgram(), "light.spec");
	if (lightSloc == INVALID_UNIFORM_LOCATION)
	{
		printf("lightSpecular not found\n");
	}
	else {
		//glProgramUniform4fv(m_shader->GetShaderProgram(), lightSloc, 1, glm::value_ptr(m_light->getLightSpecular()));
		glUniform4fv(m_shader->GetUniformLocation("light.spec"), 1, glm::value_ptr(m_light->getLightSpecular()));

	}

	GLuint lightPloc = glGetUniformLocation(m_shader->GetShaderProgram(), "light.position");
	//glProgramUniform3fv(m_shader->GetShaderProgram(), lightPloc, 1, glm::value_ptr(m_light->getLightPositionViewSpace()));
    glUniform3fv(m_shader->GetUniformLocation("light.position"), 1, glm::value_ptr(m_light->getLightPositionViewSpace()));


	//////////////////////////////////////////////


	//////// TEST MATERIAL ///////////////
	/*
	float matAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	float matDiffuse[] = { 1.0, 0.1, 0.1, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShininess = 20.0f;

	GLuint mAmbLoc = m_shader->GetUniformLocation("material.ambient");
	glUniform4fv(mAmbLoc, 1, matAmbient);

	GLuint mDiffLoc = m_shader->GetUniformLocation("material.diffuse");
	glUniform4fv(mDiffLoc, 1, matDiffuse);

	GLuint mSpecLoc = m_shader->GetUniformLocation("material.spec");
	glUniform4fv(mSpecLoc, 1, matSpec);

	GLuint mShineLoc = m_shader->GetUniformLocation("material.shininess");
	glUniform1f(mShineLoc, matShininess);
	*/
	///////////////////////////////////////////

	
	/*
	if (m_ship != NULL) {
		glUniform1i(m_hasTexture, false);


		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_ship->GetModel()));

		if (m_ship->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ship->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_ship->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}
	
	*/

	/*
	if (m_space != NULL) {

		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_space->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_space->GetModel()));

		if (m_space->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_space->getTextureID());

			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 0);  // texture unit 0
			glUniform1i(m_hasTexture, true);
		}
		else {
			glUniform1i(m_hasTexture, false);
		}
		glUniform1i(m_hasNormalMap, false);
		m_space->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}
	*/

	if (m_sun != NULL) {

		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_sun->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));

		if (m_sun->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			glUniform1i(m_shader->GetUniformLocation("sp"), 0);
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 1);
		}
		else {
			glUniform1i(m_shader->GetUniformLocation("hasTexture"), 0);
		}

		if (m_sun->hasNormalTex) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_sun->getNormalTextureID());
			glUniform1i(m_shader->GetUniformLocation("normalMap"), 1);
			glUniform1i(m_shader->GetUniformLocation("hasNormalMap"), 1);
		}
		else {
			glUniform1i(m_shader->GetUniformLocation("hasNormalMap"), 0);
		}


		m_sun->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}



	//Rendder the EARTH
	if (m_earth != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));
		glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_earth->GetModel())));
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(normMat));


		//Material for EARTH
		SetMaterial(m_earth->getMaterial());

		if (m_earth->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());

			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 0);  // texture unit 0
			glUniform1i(m_hasTexture, true);
		}
		else {
			glUniform1i(m_hasTexture, false);
		}

		if (m_earth->hasNormalTex) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_earth->getNormalTextureID());
			GLuint normalSampler = m_shader->GetUniformLocation("normalMap");
			glUniform1i(normalSampler, 1);
			glUniform1i(m_hasNormalMap, true);
		}
		else {
			glUniform1i(m_hasNormalMap, false);
		}

		m_earth->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture, m_hasNormalMap);
	}


	/*

	if (m_moon != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));

		if (m_moon->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());

			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 0);  // texture unit 0
			glUniform1i(m_hasTexture, true);
		}
		else {
			glUniform1i(m_hasTexture, false);
		}

		m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
	}
	*/
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
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
	GLint mSpecLoc = m_shader->GetUniformLocation("material.spec");
	GLint mShineLoc = m_shader->GetUniformLocation("material.shininess");

	glUniform4fv(mAmbLoc, 1, glm::value_ptr(mat.ambient));
	glUniform4fv(mDiffLoc, 1, glm::value_ptr(mat.diffuse));
	glUniform4fv(mSpecLoc, 1, glm::value_ptr(mat.specular));
	glUniform1f(mShineLoc, mat.shininess);
}
