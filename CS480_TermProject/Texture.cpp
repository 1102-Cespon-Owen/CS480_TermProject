#include "Texture.h"

Texture::Texture(const char* fileName, const char* normalFileName) {
    m_TextureID = 0;
	m_NormalTextureID = 0;

	if (!loadTexture(fileName, 0)) { //DIFFUSE
        printf("Texture Loading Failed for: %s\n", fileName);
    }
	if (normalFileName != NULL) { //NORMALS
		if (!loadTexture(normalFileName, 1)) {
			printf("Normal Texture Loading Failed for: %s\n", normalFileName);
		}
	}
    else {
        initializeTexture();
    }
}

Texture::Texture() {
    m_TextureID = 0;
	m_NormalTextureID = 0;
    printf("No Texture Data Provided.\n");
}

bool Texture::loadTexture(const char* texFile, int textureType) {

    if (texFile == nullptr) {
        printf("Texture path is null!\n");
        return false;
    }

    if (textureType == 0) {
        m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        if (!m_TextureID) {
            printf("Failed: Could not open diffuse texture file: %s\n", texFile);
            return false;
        }
        printf("Diffuse texture loaded: %s | ID: %u\n", texFile, m_TextureID);
    }

    if (textureType == 1) {
        m_NormalTextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        if (!m_NormalTextureID) {
            printf("Failed: Could not open normal texture file: %s\n", texFile);
            return false;
        }
        printf("Normal map loaded: %s | ID: %u\n", texFile, m_NormalTextureID);
    }

    return true;
}

bool Texture::initializeTexture() {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D); 

    return true;
}

