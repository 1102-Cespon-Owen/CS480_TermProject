#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"
#include "light.h"

#define numVBOs 2;
#define numIBs 2;


class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void Render();

    Camera* getCamera() { return m_camera; }
    void SetMaterial(const Material& mat);


  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
	GLint m_normalMatrix;
    GLint m_positionAttrib;
    GLint m_normalAttrib;
    GLint m_tcAttrib;
    GLint m_hasTexture;
    GLint m_hasNormalMap;


    // PROLLY NO NEED
    /*
	GLint m_lightAmbient;
	GLint m_lightDiffuse;
	GLint m_lightSpecular;
	GLint m_lightPosition;
	GLint m_globalAmbient;
	GLint m_lightPositionViewSpace;
    */


    //LIGHTING OBJECT
    Light* m_light;

    //SKY SPHERE(SPACE)
    Sphere* m_space; 
	Material m_spaceMat;

    // SOLAR BODIES

    //SUN
    Sphere* m_sun;
	Material m_sunMat;

    //MERCURY

	//VENUS

	//EARTH SYSTEM
    Sphere* m_earth;
	Material m_earthMat;
    Sphere* m_moon;

	//MARS

	//JUPITER

	//SATURN

	//URANUS

	//NEPTUNE

	

    //STARSHIP
    Mesh* m_ship;



};

#endif /* GRAPHICS_H */
