#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char* ReadFile(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Failed to allocate memory for: %s\n", filename);
        return NULL;
    }

    size_t read_length = fread(buffer, 1, length, file);
    buffer[read_length] = '\0'; 

    fclose(file);
    return buffer;
}

GLuint CreateShader(const char* filename, GLuint shaderType) {
    char* shaderSource = ReadFile(filename);
    if (!shaderSource)  {
        return 0; /* Failed to load source */
    }

    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, (const char**)&shaderSource, NULL);
    glCompileShader(shader);

    /* Free the buffer allocated in ReadFile */
    free(shaderSource);

    return shader;
}

GLuint CreateProgram(GLuint vertex, GLuint fragment) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

typedef struct {
    float x;
    float y;
    float width;
    float height;
} GameObject;

bool CheckCollision(GameObject a, GameObject b) {
    return (
        a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y
    );
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Dino game", NULL, NULL);
        
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    GLuint vertexShader = CreateShader("vertexShader.glsl", GL_VERTEX_SHADER);

    GLuint fragmentShader = CreateShader("fragmentShader.glsl", GL_FRAGMENT_SHADER);

    GLuint program = CreateProgram(vertexShader, fragmentShader);

    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    GLint offsetLocation = glGetUniformLocation(program, "offset");
    GLint scaleLocation  = glGetUniformLocation(program, "scale");
    GLint colorLocation  = glGetUniformLocation(program, "color");

    GameObject dino;
    dino.x = -0.8f;
    dino.y = -0.7f;
    dino.width = 0.12f;
    dino.height = 0.20f;

    GameObject cactus;
    cactus.x = 1.0f;
    cactus.y = -0.7f;
    cactus.width = 0.08f;
    cactus.height = 0.25f;

    float velocityY = 0.0f;

    const float gravity = -1.8f;
    const float jumpForce = 1.0f;

    bool gameOver = false;

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();

        float deltaTime = (float)(currentTime - lastTime);

        lastTime = currentTime;

        glfwPollEvents();

        // Restart
        if (gameOver && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            gameOver = false;

            cactus.x = 1.0f;
            dino.y = -0.7f;
            velocityY = 0.0f;
        }

        // Jump & Physics
        if (!gameOver) {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                if (dino.y <= -0.7f) {
                    velocityY = jumpForce;
                }
            }

            velocityY += gravity * deltaTime;
            dino.y += velocityY * deltaTime;

            // Ground collision
            if (dino.y < -0.7f) {
                dino.y = -0.7f;
                velocityY = 0.0f;
            }

            // Move cactus
            cactus.x -= 1.2f * deltaTime;

            // Reset cactus
            if (cactus.x < -1.2f) {
                cactus.x = 1.0f;
            }

            // Collision
            if (CheckCollision(dino, cactus)) {
                gameOver = true;

                printf("GAME OVER\n");
                printf("Press R to restart\n");
            }
        }

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        glBindVertexArray(vao);

        // Draw Dino
        glUniform2f(offsetLocation, dino.x, dino.y);
        glUniform2f(scaleLocation, dino.width, dino.height);
        glUniform3f(colorLocation, 0.2f, 0.2f, 0.2f);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw Cactus
        glUniform2f(offsetLocation, cactus.x, cactus.y);
        glUniform2f(scaleLocation, cactus.width, cactus.height);
        glUniform3f(colorLocation, 0.0f, 0.7f, 0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}