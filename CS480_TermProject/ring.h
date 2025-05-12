#ifndef RING_H
#define RING_H

#include "graphics_headers.h"
#include "Texture.h"
class Ring
{
    public:
        Ring(float innerRadius, float outerRadius, int segments, const char* textureFile = nullptr);
        ~Ring();

        void Render(GLint positionAttrib, GLint normalAttrib, GLint tcAttrib, GLint hasTexture);
        glm::mat4 GetModelMatrix();
        void SetModelMatrix(glm::mat4 model);
        bool getHasTex() { return hasTex; }

    private:
        void generateRingGeometry(float innerRadius, float outerRadius, int segments);
        void setupBuffers();

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint vao, vbo, ibo;

        Texture* m_texture;
        bool hasTex;
        glm::mat4 modelMatrix;
};
#endif /* RIING_H */

