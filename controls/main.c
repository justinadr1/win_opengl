#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char* readShader(const char* filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    int size = ftell(file);

    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    return buffer;
}

unsigned int compileShaders()
{
    char* vertexSource = readShader("vertexShader.shader");
    char* fragmentSource = readShader("fragmentShader.shader");

    if (!vertexSource || !fragmentSource)
    {
        free(vertexSource);
        free(fragmentSource);
        return 1;
    }

    const char* vsrc = vertexSource;
    const char* fsrc = fragmentSource;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsrc, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsrc, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(vertexSource);
    free(fragmentSource);

    return shaderProgram;

}  

/* input-controlled value */
float offsetX = 0.0f;
float offsetY = 0.0f;

void processInput(GLFWwindow* window) {
    float speed = 0.0003f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        offsetX -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        offsetX += speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        offsetY += speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        offsetY -= speed;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Controls", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    GLuint shaderProgram = compileShaders();

    float vertices[] = 
    {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
     {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), offsetX);
        glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), offsetY);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}