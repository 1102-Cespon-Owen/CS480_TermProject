#include "object.h"
#include "Texture.h"

class Sphere :
    public Object
{
public:
    Sphere();


    void Render(GLint positionAttribLoc, GLint colorAttribLoc);
    void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex, GLint hasNormalMapLoc);

    glm::mat4 GetModel() { return model; }
    void Update(glm::mat4 matModel);

    Sphere(int prec);
    Sphere(int prec, const char* fname);
    Sphere(int prec, const char* fname, const char* fname2);

    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();

    GLuint getTextureID() { return m_texture->getTextureID(); }
    GLuint getNormalTextureID() { return m_texture->getNormalTextureID(); }

    void setMaterial(const Material& mat) { material = mat; }
    Material getMaterial() const { return material;  }
    
    //Check For Texture
    bool hasTex;
    bool hasNormalTex;

private:
    glm::vec3 pivotLocation;
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<glm::vec3> TexCoords; // For TB
	std::vector<glm::vec3> Normals; // For NB
    GLuint VB;
    GLuint IB;
    GLuint TB;
    GLuint NB;

    //For Texture
    Texture* m_texture;


    GLuint vao;

    float angle;

    void setupVertices();
    void setupBuffers();
    void setupModelMatrix(glm::vec3 pivotLoc, float angle, float scale);

    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    void init(int);
    float toRadians(float degrees);

    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors

    Material material; 

 

};