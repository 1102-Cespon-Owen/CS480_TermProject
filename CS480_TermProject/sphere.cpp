#include "sphere.h"

Sphere::Sphere()
{
    init(48);
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
	hasTex = false;
    hasNormalTex = false; 
}

Sphere::Sphere(int prec) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
    hasTex = false;
    hasNormalTex = false;
}

//ONLY TEXTURE
Sphere::Sphere(int prec, const char* fname) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

    m_texture = new Texture();
    if (m_texture->loadTexture(fname, 0)) {
        hasTex = true;
        std::cout << "Texture loaded for " << fname << " | ID: " << m_texture->getTextureID() << std::endl;
    }
    else {
        hasTex = false;
        printf("Texture load failed for %s\n", fname);
    }

    hasNormalTex = false; 
}

Sphere::Sphere(int prec, const char* fname, const char* normalfname) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

    // load texture from file
    m_texture = new Texture();
    if (m_texture->loadTexture(fname, 0)) {
        hasTex = true;
        std::cout << "Texture loaded for " << fname << " | ID: " << m_texture->getTextureID() << std::endl;

    }
    else {
        hasTex = false;
        printf("Texture load failed for %s\n", fname);
    }

    if (normalfname != nullptr) {
        if (m_texture->loadTexture(normalfname, 1)) {
            hasNormalTex = true;
            std::cout << "Normal Texture loaded for " << normalfname << " | ID: " << m_texture->getNormalTextureID() << std::endl;
        }
        else {
            hasNormalTex = false;
            printf("Normal map load failed for %s\n", normalfname);
        }
    }
    else {
        hasNormalTex = false;
        std::cout << "No normal map provided.\n";
    }

}


void Sphere::Render(GLint positionAttribLoc, GLint normalAttribLoc)
{
    glBindVertexArray(vao);

    // Enable Vertext Attributes
    glEnableVertexAttribArray(positionAttribLoc);
    glEnableVertexAttribArray(normalAttribLoc);

    // Bind your VBO buffer(s) and then setup vertex attribute pointers
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(positionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
 

    // Bind your index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render
    glDrawArrays(GL_TRIANGLES, 0, getNumIndices());

    // Disable Vertex Attribuates
    glDisableVertexAttribArray(positionAttribLoc);
    glDisableVertexAttribArray(normalAttribLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Sphere::Render(GLint posAttribLoc, GLint normAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLint hasNormalMapLoc)
{
    glBindVertexArray(vao);

    // Enable vertex attibute arrays for each vertex attrib
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(normAttribLoc);
    glEnableVertexAttribArray(tcAttribLoc);

    // Bind your VBO
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(normAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    // If has texture, set up texture unit(s): update here for texture rendering
    if (m_texture != NULL) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());

        if (m_texture->getNormalTextureID() != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texture->getNormalTextureID());
        }
    }

    // Bind your Element Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

    // Disable vertex arrays
    glDisableVertexAttribArray(posAttribLoc);
    glDisableVertexAttribArray(normAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Sphere::setupVertices() {
    std::vector<int> ind = getIndices();
    std::vector<glm::vec3> vert = getVertices();
    std::vector<glm::vec2> tex = getTexCoords();
    std::vector<glm::vec3> norm = getNormals();


    int numIndices = getNumIndices();
    for (int i = 0; i < numIndices; i++) {
        Vertices.push_back(Vertex(vertices[ind[i]], normals[ind[i]], texCoords[ind[i]]));
        Indices.push_back(i);
    }
}


void Sphere::setupBuffers() {
    // For OpenGL 3
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}



void Sphere::setupModelMatrix(glm::vec3 pivot, float angle, float scale) {
    pivotLocation = pivot;
    model = glm::translate(glm::mat4(1.0f), pivotLocation);
    model *= glm::rotate(glm::mat4(1.f), angle, glm::vec3(0., 1., 0));
    model *= glm::scale(glm::vec3(scale, scale, scale));
}

void Sphere::Update(glm::mat4 matModel) {

    model = matModel;

}

float Sphere::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void Sphere::init(int prec) {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }
   

    
    for (int i = 0; i <= prec; i++) {
        for (int j = 0; j <= prec; j++) {
            float y = (float)cos(toRadians(180.f - i * 180.f / prec));
            float x = -(float)cos(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            float z = (float)sin(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
            texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
            normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
        }
    }

    // calculate triangles indices
    for (int i = 0; i < prec; i++) {
        for (int j = 0; j < prec; j++) {
            indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }

}

// accessors
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
std::vector<int> Sphere::getIndices() { return indices; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec2> Sphere::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }