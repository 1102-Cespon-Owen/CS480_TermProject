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
#include "user.h"
#include "AsteroidBelt.h"
#include "ring.h"


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

    void RenderPlanets(Sphere* planet);

 
    void SetMaterial(const Material& mat);
    User* getUser() { return m_user; }



  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *cam;
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

    //LIGHTING OBJECT
    Light* m_light;

    //SKY SPHERE(SPACE)
    Sphere* m_space; 
	Material m_spaceMat;

    // SOLAR BODIES
    Material planetMaterial;
    //SUN
    Sphere* m_sun;
	Material m_sunMat;

    //MERCURY
    Sphere* m_mercury;

	//VENUS
    Sphere* m_venus;

	//EARTH SYSTEM
    Sphere* m_earth;

    Sphere* m_moon;

	//MARS
    Sphere* m_mars;

	//JUPITER
    Sphere* m_jupiter;

	//SATURN
    Sphere* m_saturn;

	//URANUS
    Sphere* m_uranus;

	//NEPTUNE
    Sphere* m_neptune;

    //RING
    Ring* m_ring;

    //COMET
    Sphere* m_comet;
	

    //STARSHIP
    Mesh* m_ship;

    Mesh* asteroid;
    AsteroidBelt* asteroidBelt;

    //NEW
	User* m_user;


};

#endif /* GRAPHICS_H */
