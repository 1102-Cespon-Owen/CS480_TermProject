#include "ring.h"

Ring::Ring(float innerRadius, float outerRadius, int segments, const char* textureFile) {
    hasTex = false;
    modelMatrix = glm::mat4(1.0f);
    generateRingGeometry(innerRadius, outerRadius, segments);
    setupBuffers();

    if (textureFile) {
        m_texture = new Texture(textureFile, NULL);
        hasTex = (m_texture != nullptr);
    }
}

Ring::~Ring() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    delete m_texture;
}

void Ring::generateRingGeometry(float innerRadius, float outerRadius, int segments) {
    float deltaAngle = 2.0f * glm::pi<float>() / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * deltaAngle;
        float xInner = innerRadius * cos(angle);
        float zInner = innerRadius * sin(angle);
        float xOuter = outerRadius * cos(angle);
        float zOuter = outerRadius * sin(angle);

        float u = (float)i / segments;

        // Inner vertex
        vertices.push_back(Vertex(glm::vec3(xInner, 0.0f, zInner), glm::vec3(0, 1, 0), glm::vec2(u, 0)));
        // Outer vertex
        vertices.push_back(Vertex(glm::vec3(xOuter, 0.0f, zOuter), glm::vec3(0, 1, 0), glm::vec2(u, 1)));
    }

    // Indices for triangle strip
    for (int i = 0; i < segments * 2; ++i) {
        indices.push_back(i);
        indices.push_back(i + 2);
    }
}

void Ring::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Ring::Render(GLint positionAttrib, GLint normalAttrib, GLint tcAttrib, GLint hasTextureLoc) {
    glUniform1i(hasTextureLoc, hasTex ? 1 : 0);

    if (hasTex && m_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(tcAttrib);
    glVertexAttribPointer(tcAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(normalAttrib);
    glDisableVertexAttribArray(tcAttrib);

    glBindVertexArray(0);

}

glm::mat4 Ring::GetModelMatrix() {
    return modelMatrix;
}

void Ring::SetModelMatrix(glm::mat4 model) {
    modelMatrix = model;
}
