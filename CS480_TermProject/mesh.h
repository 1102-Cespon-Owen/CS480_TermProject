#ifndef MESH_H
#define MESH_H

#include <vector>
#include "graphics_headers.h"
#include "Texture.h"

class Mesh
{
public:
    Mesh();
    Mesh(glm::vec3 pivot, const char* fname);
    Mesh(glm::vec3 pivot, const char* fname, const char* tname);

    ~Mesh();
    void Update(glm::mat4 model);
    void Render(GLint posAttrib, GLint colAttrib);
    void Render(GLint posAttribLoc, GLint normAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLint hasNormalMapLoc);

    glm::mat4 GetModel();

    bool InitBuffers();
    bool loadModelFromFile(const char* path);

    bool hasTex;
    GLuint getTextureID() { return m_texture->getTextureID(); }



private:
    std::vector<float> verVals;
    std::vector<float> normVals;
    std::vector<float> stVals;

    std::vector<float> triVerts;
    std::vector<float> texCoords;
    std::vector<float> normals;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    glm::vec3 pivotLocation;
    glm::vec3 colors;
    float angle;

    glm::mat4 model;
    GLuint VB, IB, vao;

    Texture* m_texture;
};

#endif