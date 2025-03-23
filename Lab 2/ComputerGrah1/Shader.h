#ifndef SHADER_H

#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glew.h>
using namespace std;
class Shader {
public:
    
    GLuint Program;

    
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    void Use();
    void SetUniform(const string& name, float value);
    void SetUniform(const string& name, int value);
    void SetUniform(const string& name, float x, float y, float z); 
    void SetUniform(const string& name, float r, float g, float b, float a); 
    void SetUniform(const string& name, float* matrix); 

private:
   
    string ReadFile(const GLchar* filename);
};

#endif
