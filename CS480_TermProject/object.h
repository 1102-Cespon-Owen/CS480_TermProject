#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "graphics_headers.h"
#include "texture.h"


class Object
{
  public:
    Object();
    Object(glm::vec3 pivot, const char* fname);
    Object(glm::vec3 pivot, const char* fname, const char* normalfname);

    ~Object();
    void Update(glm::mat4 model);
    void Render(GLint posAttrib, GLint colAttrib);

    glm::mat4 GetModel();

    bool InitBuffers();
    void setupVerticies();

  private:
    glm::vec3 pivotLocation;
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    GLuint vao;

    float angle;

    //Texture
	Texture* m_texture;

    // Uniform Location for texture use
    GLint hasTex;
    GLint hasNormalTex;


};

#endif /* OBJECT_H */
