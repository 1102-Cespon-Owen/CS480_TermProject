
#pragma once
#include <vector>
#include "graphics_headers.h"
#include "Mesh.h"
#include "Shader.h"

class AsteroidBelt {
public:
    AsteroidBelt(Mesh* baseMesh, int count, float innerRadius, float outerRadius);
    void render(GLint posAttribLoc, GLint normAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLint hasNormalMapLoc);

private:
    Mesh* mesh;
    std::vector<glm::mat4> modelMatrices;
    unsigned int instanceVBO;
    int instanceCount;

    void generateMatrices(float innerRadius, float outerRadius);
};
