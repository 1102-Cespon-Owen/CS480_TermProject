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
        s =
            "#version 460\n"
            "layout(location = 0) in vec3 v_position;\n"
            "layout(location = 1) in vec3 v_normal;\n"
            "layout(location = 2) in vec2 v_tc;\n"

            "out vec3 varNorm;\n"
            "out vec3 varLdir;\n"
            "out vec3 varPos;\n"
            "out vec2 tc;\n"

            "uniform mat4 projectionMatrix;\n"
            "uniform mat4 viewMatrix;\n"
            "uniform mat4 modelMatrix;\n"
            "uniform mat3 normMatrix;\n"

            "struct PositionalLight {\n"
            "  vec4 ambient;\n"
            "  vec4 diffuse;\n"
            "  vec4 spec;\n"
            "  vec3 position;\n"
            "};\n"

            "uniform vec4 GlobalAmbient;\n"
            "uniform PositionalLight light;\n"

            "struct Material {\n"
            "  vec4 ambient;\n"
            "  vec4 diffuse;\n"
            "  vec4 spec;\n"
            "  float shininess;\n"
            "};\n"
            "uniform Material material;\n"

            "void main(void) {\n"
            "  vec4 v = vec4(v_position, 1.0);\n"
            "  gl_Position = projectionMatrix * viewMatrix * modelMatrix * v;\n"
            "  tc = v_tc;\n"
            "  varPos = (viewMatrix * modelMatrix * v).xyz;\n"
            "  varLdir = light.position - varPos;\n"
            "  varNorm = normMatrix * v_normal;\n"
            "}\n";
    }
    else if (ShaderType == GL_FRAGMENT_SHADER)
    {
        s =
            "#version 460\n"
            "in vec3 varNorm;\n"
            "in vec3 varLdir;\n"
            "in vec3 varPos;\n"
            "in vec2 tc;\n"
            "\n"
            "uniform sampler2D sp;\n"
            "uniform sampler2D normalMap;\n"
            "uniform bool hasTexture;\n"
            "uniform bool hasNormalMap;\n"
            "\n"
            "struct Material {\n"
            "  vec4 ambient;\n"
            "  vec4 diffuse;\n"
            "  vec4 spec;\n"
            "  float shininess;\n"
            "};\n"
            "uniform Material material;\n"
            "\n"
            "struct PositionalLight {\n"
            "  vec4 ambient;\n"
            "  vec4 diffuse;\n"
            "  vec4 spec;\n"
            "  vec3 position;\n"
            "};\n"
            "uniform PositionalLight light;\n"
            "\n"
            "uniform vec4 GlobalAmbient;\n"
            "\n"
            "out vec4 frag_color;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "  vec3 L = normalize(varLdir);\n"
            "  vec3 N;\n"
            "  if (hasNormalMap)\n"
            "    N = normalize(varNorm + texture(normalMap, tc).xyz * 2.0 - 1.0);\n"
            "  else\n"
            "    N = normalize(varNorm);\n"
            "\n"
            "  vec3 V = normalize(-varPos);\n"
            "  vec3 R = normalize(reflect(-L, N));\n"
            "\n"
            "  float cosTheta = max(dot(L, N), 0.0);\n"
            "  float cosPhi = max(dot(R, V), 0.0);\n"
            "\n"
            "  vec3 texColor = hasTexture ? texture(sp, tc).xyz : vec3(1.0);\n"
            "\n"
            "  vec3 amb = GlobalAmbient.xyz + (texColor * light.ambient.xyz * material.ambient.xyz);\n"
            "  vec3 dif = light.diffuse.xyz * material.diffuse.xyz * texColor * cosTheta;\n"
            "  vec3 spc = light.spec.xyz * material.spec.xyz * pow(cosPhi, material.shininess);\n"
            "\n"
            "  frag_color = vec4(amb + dif + spc, 1.0);\n"
            "}\n";
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
