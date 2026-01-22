#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

/* Define missing OpenGL types */
typedef ptrdiff_t GLsizeiptr;
typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef double GLdouble;
typedef char GLchar;

/* ================= OpenGL 3.x constants ================= */
#define GL_VERTEX_SHADER            0x8B31
#define GL_FRAGMENT_SHADER          0x8B30
#define GL_ARRAY_BUFFER             0x8892
#define GL_STATIC_DRAW              0x88E4
#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_FLOAT                    0x1406
#define GL_FALSE                    0
#define GL_TRIANGLES                0x0004



/* ================= OpenGL function pointers ================= */

/* WGL */
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

/* Shader functions */
typedef GLuint(APIENTRY* PFNGLCREATESHADERPROC)(GLenum);
typedef void   (APIENTRY* PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const char* const*, const GLint*);
typedef void   (APIENTRY* PFNGLCOMPILESHADERPROC)(GLuint);
typedef GLuint(APIENTRY* PFNGLCREATEPROGRAMPROC)(void);
typedef void   (APIENTRY* PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef void   (APIENTRY* PFNGLLINKPROGRAMPROC)(GLuint);
typedef void   (APIENTRY* PFNGLUSEPROGRAMPROC)(GLuint);
typedef void   (APIENTRY* PFNGLDELETESHADERPROC)(GLuint);
typedef void   (APIENTRY* PFNGLDELETEPROGRAMPROC)(GLuint);

/* Buffers / VAO */
typedef void (APIENTRY* PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint*);
typedef void (APIENTRY* PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (APIENTRY* PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
typedef void (APIENTRY* PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void (APIENTRY* PFNGLDELETEVERTEXARRAYSPROC)(GLsizei, const GLuint*);

/* Globals */
static PFNGLCREATESHADERPROC            glCreateShader;
static PFNGLSHADERSOURCEPROC           glShaderSource;
static PFNGLCOMPILESHADERPROC          glCompileShader;
static PFNGLCREATEPROGRAMPROC          glCreateProgram;
static PFNGLATTACHSHADERPROC           glAttachShader;
static PFNGLLINKPROGRAMPROC            glLinkProgram;
static PFNGLUSEPROGRAMPROC             glUseProgram;
static PFNGLDELETESHADERPROC           glDeleteShader;
static PFNGLDELETEPROGRAMPROC          glDeleteProgram;

static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
static PFNGLGENBUFFERSPROC              glGenBuffers;
static PFNGLBINDBUFFERPROC              glBindBuffer;
static PFNGLBUFFERDATAPROC              glBufferData;
static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
static PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;

/* ================= Manual loader ================= */
void loadGLFunctions(void)
{
#define LOAD_GL(name) name = (void*)wglGetProcAddress(#name)

    wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    LOAD_GL(glCreateShader);
    LOAD_GL(glShaderSource);
    LOAD_GL(glCompileShader);
    LOAD_GL(glCreateProgram);
    LOAD_GL(glAttachShader);
    LOAD_GL(glLinkProgram);
    LOAD_GL(glUseProgram);
    LOAD_GL(glDeleteShader);
    LOAD_GL(glDeleteProgram);

    LOAD_GL(glGenVertexArrays);
    LOAD_GL(glBindVertexArray);
    LOAD_GL(glGenBuffers);
    LOAD_GL(glBindBuffer);
    LOAD_GL(glBufferData);
    LOAD_GL(glVertexAttribPointer);
    LOAD_GL(glEnableVertexAttribArray);
    LOAD_GL(glDeleteBuffers);
    LOAD_GL(glDeleteVertexArrays);

#undef LOAD_GL
}

/* ================= Shader utilities ================= */
char* readShader(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

/* ================= Shader compilation ================= */
GLuint compileShaders(const char* vertex, const char* fragment)
{
    char* vs = readShader(vertex);
    char* fs = readShader(fragment);
    if (!vs || !fs) return 0;

    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, (const char**)&vs, NULL);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, (const char**)&fs, NULL);
    glCompileShader(f);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);

    glDeleteShader(v);
    glDeleteShader(f);

    free(vs);
    free(fs);
    return p;
}

/* ================= Win32 globals ================= */
static HDC   g_hdc;
static HGLRC g_glrc;
static HWND  g_hwnd;
static int   running = 1;

/* ================= Win32 procedure ================= */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CLOSE || msg == WM_DESTROY)
    {
        running = 0;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* ================= OpenGL context ================= */
void createModernContext(HWND hwnd)
{
    g_hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(g_hdc, &pfd);
    SetPixelFormat(g_hdc, pf, &pfd);

    /* Temporary 1.1 context */
    HGLRC temp = wglCreateContext(g_hdc);
    wglMakeCurrent(g_hdc, temp);

    loadGLFunctions(); // load wglCreateContextAttribsARB

    /* Modern 3.3 core context */
    int attribs[] =
    {
        0x2091, 3,          // WGL_CONTEXT_MAJOR_VERSION_ARB
        0x2092, 3,          // WGL_CONTEXT_MINOR_VERSION_ARB
        0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB = CORE
        0
    };
    g_glrc = wglCreateContextAttribsARB(g_hdc, 0, attribs);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp);
    wglMakeCurrent(g_hdc, g_glrc);

    loadGLFunctions(); // load all modern OpenGL functions
}

/* ================= Entry point ================= */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "GLWindow";

    RegisterClass(&wc);

    g_hwnd = CreateWindow("GLWindow", "Win32 OpenGL 3.3 Core (Manual Loader)",
                          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                          CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                          NULL, NULL, hInst, NULL);

    createModernContext(g_hwnd);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    MSG msg;
    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(g_hdc);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(g_glrc);
    ReleaseDC(g_hwnd, g_hdc);

    return 0;
}
