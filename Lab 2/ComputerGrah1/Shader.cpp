#include "Shader.h"
using namespace std;
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
   
    string vertexCode = ReadFile(vertexPath);
    string fragmentCode = ReadFile(fragmentPath);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);

    GLint success;
    GLchar infoLog[1024];
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        exit(1); 
    }

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        exit(1); 
    }

    // Создание и линковка программы
    Program = glCreateProgram();
    glAttachShader(Program, vertexShader);
    glAttachShader(Program, fragmentShader);
    glLinkProgram(Program);

    // Проверка линковки программы
    glGetProgramiv(Program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(Program, 1024, NULL, infoLog);
        cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        exit(1); // Выход из программы при ошибке линковки
    }

    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Use() {
    glUseProgram(Program);
}

string Shader::ReadFile(const GLchar* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

void Shader::SetUniform(const std::string& name, float value) {
    Use();
    glUniform1f(glGetUniformLocation(Program, name.c_str()), value);
}

void Shader::SetUniform(const string& name, int value) {
    Use();
    glUniform1i(glGetUniformLocation(Program, name.c_str()), value);
}

void Shader::SetUniform(const string& name, float x, float y, float z) {
    Use();
    glUniform3f(glGetUniformLocation(Program, name.c_str()), x, y, z);
}

void Shader::SetUniform(const string& name, float r, float g, float b, float a) {
    Use();
    glUniform4f(glGetUniformLocation(Program, name.c_str()), r, g, b, a);
}

void Shader::SetUniform(const string& name, float* matrix) {
    Use();
    glUniformMatrix4fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, matrix);
}
