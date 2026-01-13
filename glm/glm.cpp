#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string readShader(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Could not open " << filepath << std::endl;
        return "";
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

GLuint compileShaders(const std::string& vertex, const std::string& fragment)
{
    std::string vertexSource   = readShader("vertexShader.glsl");
    std::string fragmentSource = readShader("fragmentShader.glsl");

    if (vertexSource.empty() || fragmentSource.empty())
        return 0;

    const char* vsrc = vertexSource.c_str();
    const char* fsrc = fragmentSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

glm::vec3 position(0.0f);
float rotation = 0.0f;
float scale = 1.0f;
float w = .01f, s = 0.1f, a = .01f, d = 0.1f, q = 0.1f, e = 0.1f, c = 0.1f, v = 0.1f;

void processInput(GLFWwindow* window)
{
    const float moveSpeed = 0.0009f;
    const float rotateSpeed = 0.0009f;
    const float scaleSpeed = 0.0009f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position.y += moveSpeed;
        std::cout << w << ": W\n";
        w += .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position.y -= moveSpeed;
        std::cout << s << ": S\n";
        s += .01f;  
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position.x -= moveSpeed;
        std::cout << a << ": A\n";
        a += .01f;  
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position.x += moveSpeed;
        std::cout << d << ": D\n";
        d += .01f;  
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        rotation += rotateSpeed;
        std::cout << q << ": Q\n";
        q += .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        rotation -= rotateSpeed;
        std::cout << e << ": E\n";
        e += .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        scale -= scaleSpeed;
        std::cout << c << ": C\n";
        c += .01f;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        scale += scaleSpeed;
        std::cout << v << ": V\n";
        v += .01f;
    }

    if (scale < 0.1f)
        scale = 0.1f;
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(800, 600, "Textured Triangle", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    float vertices[] =
    {
        // position          // color           // texcoord
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* color */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* texture coords */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("texture.png", &w, &h, &channels, 0);
    if (!data)
    {
        std::cerr << "Failed to load texture\n";
        return -1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    GLuint shaderProgram = compileShaders("vertexShader.glsl", "fragmentShader.glsl");
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(scale));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);

        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
