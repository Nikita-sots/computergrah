#include "Shader.h"
#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>
#include <iostream>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);

    struct {
        float min = -5.0f;
        float max = 5.0f;
    } zLimit;

    struct {
        float min = -3.0f;
        float max = 3.0f;
    } yLimit;

    struct {
        float min = -90.0f;
        float max = 90.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    if (index == 0) {
        return model;
    }

    if (index == 1) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint); 
        return model;
    }

    if (index == 2) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);
        return model;
    }

    if (index == 3) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);

        model = glm::translate(model, objectTransforms[3].position);
        return model;
    }

    return model;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Transformations", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("vertex_sheder.glsl", "fragment_shader.glsl");
    Model ourModel("Lab_3.obj");

    objectTransforms.resize(4);

    objectTransforms[1].pivotPoint = glm::vec3(0.0f, 0.0f, -1.4f);
    objectTransforms[2].pivotPoint = glm::vec3(0.0f, 0.0f, -2.67f);

    objectTransforms[3].zLimit = { -0.7f, 0.2f };
    objectTransforms[1].rotationLimit = { -90.0f, 90.0f };
    objectTransforms[2].rotationLimit = { -90.0f, 90.0f };

    shader.use();
    shader.setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
    shader.setVec3("light.ambient", glm::vec3(1.0f, 0.8f, 0.6f));
    shader.setVec3("light.diffuse", glm::vec3(1.0f, 0.8f, 0.6f));
    shader.setVec3("light.specular", glm::vec3(1.0f));
    shader.setVec3("material.ambient", glm::vec3(1.0f, 0.2f, 0.4f));
    shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.8f, 0.3f));
    shader.setVec3("material.specular", glm::vec3(1.0f, 0.8f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", cameraPos);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (size_t i = 0; i < ourModel.meshTransforms.size(); ++i) {
            ourModel.meshTransforms[i] = calculateModelMatrix(i);
        }

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = 1.5f * deltaTime;
    float rotateSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        objectTransforms[1].rotation.z += rotateSpeed;

        if (objectTransforms[1].rotation.z > objectTransforms[1].rotationLimit.max)
            objectTransforms[1].rotation.z = objectTransforms[1].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        objectTransforms[1].rotation.z -= rotateSpeed;

        if (objectTransforms[1].rotation.z < objectTransforms[1].rotationLimit.min)
            objectTransforms[1].rotation.z = objectTransforms[1].rotationLimit.min;
    }
        
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        objectTransforms[2].rotation.z += rotateSpeed;

        if (objectTransforms[2].rotation.z > objectTransforms[2].rotationLimit.max)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        objectTransforms[2].rotation.z -= rotateSpeed;

        if (objectTransforms[2].rotation.z < objectTransforms[2].rotationLimit.min)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.min;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        objectTransforms[3].position.y += moveSpeed;

        if (objectTransforms[3].position.y > objectTransforms[3].zLimit.max)
            objectTransforms[3].position.y = objectTransforms[3].zLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        objectTransforms[3].position.y -= moveSpeed;

        if (objectTransforms[3].position.y < objectTransforms[3].zLimit.min)
            objectTransforms[3].position.y = objectTransforms[3].zLimit.min;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}
