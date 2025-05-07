#include "object.h"

Object::Object()
{
	// Vertex Set Up
	setupVerticies();

	// Model Set Up
	angle = 0.0f;
	pivotLocation = glm::vec3(0.f, 0.f, 0.f);
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("Some buffers not initialized.\n");
	}

	hasTex = false; 
	
}

Object::Object(glm::vec3 pivot, const char* fname)
{
	// Vertex Set Up
	setupVerticies();
	
	// Model Set Up
	angle = 0.0f;
	pivotLocation = pivot;
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("Some buffers not initialized.\n");
	}

	//Load Texture from file
	m_texture = new Texture();
	if (m_texture->loadTexture(fname, 0)) {
		hasTex = true;
	}
	else {
		hasTex = false;
		printf("Texture load failed for %s\n", fname);
	}

}

Object::Object(glm::vec3 pivot, const char* fname, const char* normalfname) {
	// Vertex Set Up
	setupVerticies();

	// Model Set Up
	angle = 0.0f;
	pivotLocation = pivot;
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("Some buffers not initialized.\n");
	}

	//Load Texture from file
	m_texture = new Texture();
	if (m_texture->loadTexture(fname, 0)) {
		hasTex = true;
		std::cout << "Texture loaded for " << fname << " | ID: " << m_texture->getTextureID() << std::endl;
	}
	else {
		hasTex = false;
		printf("Texture load failed for %s\n", fname);
	}

	if (m_texture->loadTexture(normalfname, 1)) {
		hasNormalTex = true;
		std::cout << "Normal Texture loaded for " << normalfname << " | ID: " << m_texture->getNormalTextureID() << std::endl;
	}
	else {
		hasNormalTex = false;
		printf("Normal map load failed for %s\n", normalfname);
	}

}


Object::~Object()
{
	Vertices.clear();
	Indices.clear();
}

void Object::Update(glm::mat4 inmodel)
{
	model = inmodel;

}

glm::mat4 Object::GetModel()
{
	return model;
}

void Object::Render(GLint posAttribLoc, GLint colAttribLoc)
{

	glBindVertexArray(vao);

	// Enable vertex attibute arrays for each vertex attrib
	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(colAttribLoc);

	// Bind your VBO
	glBindBuffer(GL_ARRAY_BUFFER, VB);

	// Set vertex attribute pointers to the load correct data
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	// Bind your Element Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	//If has texture, set up texture unit(s)
	if (m_texture != NULL) {
		GLuint texID = m_texture->getTextureID();
		glUniform1i(hasTex, texID > 0);
		if (texID > 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texID);
		}

		GLuint normID = m_texture->getNormalTextureID();
		glUniform1i(hasNormalTex, normID > 0);
		if (normID > 0) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normID);
		}
	}


	// Render
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	// Disable vertex arrays
	glDisableVertexAttribArray(posAttribLoc);
	glDisableVertexAttribArray(colAttribLoc);
}


bool Object::InitBuffers() {

	// For OpenGL 3
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}

void Object::setupVerticies() {
	Vertices = {
  {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {1.,0.}},
  {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.,0.}},
  {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.,0.}},
  {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.,0.}},
  {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.,0.}},
  {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.,0.}},
  {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.,0.}},
  {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.,0.}}
	};

	Indices = {
	  2, 3, 4,
	  8, 7, 6,
	  1, 5, 6,
	  2, 6, 7,
	  7, 8, 4,
	  1, 4, 8,
	  1, 2, 4,
	  5, 8, 6,
	  2, 1, 6,
	  3, 2, 7,
	  3, 7, 4,
	  5, 1, 8
	};

	// The index works at a 0th index
	for (unsigned int i = 0; i < Indices.size(); i++)
	{
		Indices[i] = Indices[i] - 1;
	}

}
