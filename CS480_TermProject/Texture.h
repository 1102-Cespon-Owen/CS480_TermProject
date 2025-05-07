#pragma once
#include "graphics_headers.h"
#include <SOIL2/SOIL2.h>

class Texture
{

public:
	Texture();
	Texture(const char* fileName, const char* filename2);
	bool loadTexture(const char* texFile, int textureType);
	GLuint getTextureID() { return m_TextureID; }
	GLuint getNormalTextureID() { return m_NormalTextureID; }

private:
	GLuint m_TextureID;
	GLuint m_NormalTextureID;

	bool initializeTexture();

};

