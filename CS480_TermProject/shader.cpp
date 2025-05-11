#include "shader.h"

Shader::Shader()
{
  m_shaderProg = 0;
}

Shader::~Shader()
{
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
  {
    glDeleteShader(*it);
  }

  if (m_shaderProg != 0)
  {
    glDeleteProgram(m_shaderProg);
    m_shaderProg = 0;
  }
}

bool Shader::Initialize()
{
  m_shaderProg = glCreateProgram();

  if (m_shaderProg == 0) 
  {
    std::cerr << "Error creating shader program\n";
    return false;
  }

  return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Shader::AddShader(GLenum ShaderType)
{
    std::string s;

    if (ShaderType == GL_VERTEX_SHADER)
    {
        s = R"(

            #version 450

            layout(location = 0) in vec3 v_position;
            layout(location = 1) in vec3 v_normal;
            layout(location = 2) in vec2 v_tc;

            out vec3 varNorm;

            out vec3 varPos;
            out vec2 tc;

            uniform mat4 projectionMatrix;
            uniform mat4 viewMatrix;
            uniform mat4 modelMatrix;
            uniform mat3 normMatrix;

            struct PositionalLight {
              vec4 ambient;
              vec4 diffuse;
              vec4 spec;
              vec3 position;
            };

            uniform vec4 GlobalAmbient;
            uniform PositionalLight light;

            void main()
            {
                varPos = vec3(modelMatrix * vec4(v_position, 1.0));
                varNorm = mat3(transpose(inverse(modelMatrix))) * v_normal;
                tc = v_tc;

                gl_Position = projectionMatrix * viewMatrix * vec4(varPos, 1.0);
            }
        )";
      
    }
    else if (ShaderType == GL_FRAGMENT_SHADER)
    {
            
          s = R"(

                #version 450

                in vec3 varNorm;
                in vec3 varPos;
                in vec2 tc;

                uniform sampler2D sp;
                uniform sampler2D normalMap;

                uniform bool hasTexture;
                uniform bool hasNormalMap;

                struct Material {
                  vec4 ambient;
                  vec4 diffuse;
                  vec4 specular;
                  float shininess;
                };
                uniform Material material;

                struct PositionalLight {
                  vec4 ambient;
                  vec4 diffuse;
                  vec4 spec;
                  vec3 position;
                };
                uniform PositionalLight light;
                
                uniform vec3 viewPos;
                uniform vec3 lightColor;
                uniform vec3 lightPos;

                uniform vec4 GlobalAmbient;

                out vec4 frag_color;

                void main()
                {


                    vec3 normal = normalize(varNorm);
                    vec3 lightDir = normalize(lightPos - varPos);
                    vec3 viewDir = normalize(viewPos - varPos);
                    vec3 reflectDir = reflect(-lightDir, normal);

                    vec4 ambient = material.ambient * vec4(lightColor, 1.0);

                    float diff = max(dot(normal, lightDir), 0.0);
                    vec4 diffuse = material.diffuse * diff * vec4(lightColor, 1.0);

                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec4 specular = material.specular * spec * vec4(lightColor, 1.0);

                    vec4 texColor = texture(sp, tc);

                    frag_color = (ambient + diffuse + specular) * texColor;
                }
            )";
            // changed vec4 ambient: vec4 ambient = light.ambient * material.ambient; 
           //Added the gammaCorrected above, replaced frag_color with, frag_color = ambient + diffuse + specular;frag_color = ambient + diffuse + specular;
           //GAMMA CORRECTION: brightens up the screen to help see
           

           //ISSUE there are no shadows

    }

    GLuint ShaderObj = glCreateShader(ShaderType);
    if (ShaderObj == 0)
    {
        std::cerr << "Error creating shader type " << ShaderType << std::endl;
        return false;
    }

    m_shaderObjList.push_back(ShaderObj);
    const GLchar* p[1] = { s.c_str() };
    GLint Lengths[1] = { (GLint)s.size() };
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        std::cerr << "Error compiling: " << InfoLog << std::endl;
        return false;
    }

    glAttachShader(m_shaderProg, ShaderObj);
    return true;
}
// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Shader::Finalize()
{
  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(m_shaderProg);

  glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
  if (Success == 0)
  {
    glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
    std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
    return false;
  }

  glValidateProgram(m_shaderProg);
  glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
  if (!Success)
  {
    glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
    std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
    return false;
  }

  // Delete the intermediate shader objects that have been added to the program
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
  {
    glDeleteShader(*it);
  }

  m_shaderObjList.clear();

  return true;
}


void Shader::Enable()
{
    glUseProgram(m_shaderProg);
}


GLint Shader::GetUniformLocation(const char* pUniformName)
{
    GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint Shader::GetAttribLocation(const char* pAttribName)
{
    GLuint Location = glGetAttribLocation(m_shaderProg, pAttribName);

    if (Location == -1) {
        fprintf(stderr, "Warning! Unable to get the location of attribute '%s'\n", pAttribName);
    }

    return Location;
}
