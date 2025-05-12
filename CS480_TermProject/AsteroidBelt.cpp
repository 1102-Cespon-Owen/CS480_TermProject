#include "AsteroidBelt.h"
#include <cstdlib>

AsteroidBelt::AsteroidBelt(Mesh* baseMesh, int count, float inner, float outer)
    : mesh(baseMesh), instanceCount(count)
{
    generateMatrices(inner, outer);

    mesh->bind();  // Make sure VAO is bound

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);



    // Set instanced matrix attribute pointers (mat4 is 4 vec4s)
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(3 + i, 1);  // Advance per instance
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

 
}

void AsteroidBelt::generateMatrices(float inner, float outer) {
    modelMatrices.resize(instanceCount);
    for (int i = 0; i < instanceCount; i++) {
        float angle = ((float)i / instanceCount) * 360.0f;
        float radius = inner + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (outer - inner)));

        float x = cos(glm::radians(angle)) * radius;
        float z = sin(glm::radians(angle)) * radius;
        float y = ((rand() % 100) - 50) * 0.02f;  // slight vertical jitter

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

        float scale = 0.05f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / 0.05f));
        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = rand() % 360;
        model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }
}

void AsteroidBelt::render(GLint posAttribLoc, GLint normAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLint hasNormalMapLoc)
{
    glBindVertexArray(mesh->getVAO()); // Mesh VAO already contains all attributes

    // Enable per-vertex attribute arrays (do NOT redefine them)
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(normAttribLoc);
    glEnableVertexAttribArray(tcAttribLoc);

    // Bind textures, if available
    if (mesh->hasTexture()) {
        glUniform1i(hasTextureLoc, true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->getTextureID());
    }
    else {
        glUniform1i(hasTextureLoc, false);
    }

    if (mesh->hasNormalMap()) {
        glUniform1i(hasNormalMapLoc, true);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh->getNormalTextureID());
    }
    else {
        glUniform1i(hasNormalMapLoc, false);
    }

    // Draw instanced elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndexBuffer());
    glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0, instanceCount);

    // Cleanup
    glDisableVertexAttribArray(posAttribLoc);
    glDisableVertexAttribArray(normAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
