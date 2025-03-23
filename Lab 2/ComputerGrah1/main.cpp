#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

class Shader {
public:
    GLuint program;

    Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexCode = loadShaderFromFile(vertexPath);
        GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);

        std::string fragmentCode = loadShaderFromFile(fragmentPath);
        GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void use() {
        glUseProgram(program);
    }

    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
        GLint location = glGetUniformLocation(program, name.c_str());
        glUniform4f(location, v0, v1, v2, v3);
    }

private:
    std::string loadShaderFromFile(const std::string& path) {
        std::ifstream shaderFile(path);
        if (!shaderFile.is_open()) {
            std::cerr << "Failed to open shader file: " << path << std::endl;
            return "";
        }
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    }

    GLuint compileShader(const char* shaderCode, GLenum shaderType) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCode, nullptr);
        glCompileShader(shader);
        checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        return shader;
    }

    void checkCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};

void generateStar(float cx, float cy, float outerRadius, float innerRadius, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    vertices.push_back(cx);
    vertices.push_back(cy);
    vertices.push_back(0.0f);

    int numPoints = 8; // 4 внешних и 4 внутренних точки
    for (int i = 0; i < numPoints; ++i) {
        float angle = 2.0f * 3.1415926f * float(i) / float(numPoints);
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        vertices.push_back(cx + x);
        vertices.push_back(cy + y);
        vertices.push_back(0.0f);

        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }
    indices.push_back(0);
    indices.push_back(numPoints);
    indices.push_back(1);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Star Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateStar(0.0f, 0.0f, 0.5f, 0.25f, vertices, indices);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // Фон (0.3, 0.3, 0.3)
        glClear(GL_COLOR_BUFFER_BIT);

        // Вычисление цвета на основе времени
        float timeValue = glfwGetTime();
        float redValue = (sin(timeValue) / 2.0f) + 0.5f;
        float greenValue = (cos(timeValue) / 2.0f) + 0.5f;
        float blueValue = (sin(timeValue + 3.14f) / 2.0f) + 0.5f;

        shader.use();
        shader.setUniform4f("ourColor", redValue, greenValue, blueValue, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}