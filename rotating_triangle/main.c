
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

char* readShader(const char* filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    int size = ftell(file);     
    
    rewind(file);
    
    char* buffer = (char*)malloc(size + 1); 
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    
    fclose(file);
    
    return buffer;
}

GLuint compileShaders(const char* vertex, const char* fragment)
{
    char* vertexSource   = readShader(vertex);
    char* fragmentSource = readShader(fragment);

    if (!vertexSource || !fragmentSource)
    {
        free(vertexSource);
        free(fragmentSource);
        return -1;
    }

    const char* vsrc = vertexSource;
    const char* fsrc = fragmentSource;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1,  &vsrc, NULL);
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


int main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "window", NULL, NULL);
    if (!window)
    {
        printf("failed to create window\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        printf("failed to initialize glew\n");
        return 1;
    }

    GLuint shaderProgram = compileShaders("vertexShader.glsl", "fragmentShader.glsl");

    float vertices[] =
    {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f 
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate rotation
        float timeValue = (float)glfwGetTime(); // seconds since GLFW started
        float angle = timeValue; // rotate 1 rad/sec
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // 4x4 rotation matrix around Z
        float transform[16] = 
        {
            cosA, -sinA, 0.0f, 0.0f,
            sinA,  cosA, 0.0f, 0.0f,
            0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,  0.0f, 0.0f, 1.0f
        };

        // Pass matrix to shader
        int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

        // Draw triangle
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}