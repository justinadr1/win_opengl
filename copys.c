#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 8192

const char* get_filename(const char* path)
{
    const char* slash = strrchr(path, '/');
#ifdef _WIN32
    const char* backslash = strrchr(path, '\\');
    if (!slash || backslash > slash)
        slash = backslash;
#endif
    return slash ? slash + 1 : path;
}

int write(const char* source, const char* destination)
{
    const char* src_path = source;
    const char* dst_dir  = destination;
    const char* filename = get_filename(src_path);

    char dst_path[1024];
    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, filename);

    FILE* src = fopen(src_path, "rb");
    if (!src)
    {
        perror("Failed to open source file");
        return 1;
    }

    FILE* dst = fopen(dst_path, "wb");
    if (!dst)
    {
        perror("Failed to open destination file");
        fclose(src);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;

    while ((bytes = fread(buffer, 1, BUFFER_SIZE, src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, dst) != bytes)
        {
            perror("Write error");
            fclose(src);
            fclose(dst);
            return 1;
        }
    }

    fclose(src);
    fclose(dst);

    printf("Wrote '%s' to '%s'\n", src_path, dst_path);
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [options]\n", argv[0]);
        printf("\ttri\n\ttex_tri\n\trot_tri\n\tidx_squ\n");
        return 1;
    }

    if (strcmp(argv[1], "tri") == 0)
    {
        write("triangle/vertexShader.glsl", ".");
        write("triangle/fragmentShader.glsl", ".");
    }
    else if (strcmp(argv[1], "tex_tri") == 0)
    {
        write("textured_triangle/vertexShader.glsl", ".");
        write("textured_triangle/fragmentShader.glsl", ".");
    }
    else if (strcmp(argv[1], "rot_tri") == 0)
    {
        write("rotating_triangle/vertexShader.glsl", ".");
        write("rotating_triangle/fragmentShader.glsl", ".");
    }
    else if (strcmp(argv[1], "idx_tri") == 0)
    {
        write("indexed_triangle/vertexShader.glsl", ".");
        write("indexed_triangle/fragmentShader.glsl", ".");
    }
    else if (strcmp(argv[1], "glm") == 0)
    {
        write("glm/vertexShader.glsl", ".");
        write("glm/fragmentShader.glsl", ".");
    }


    return 0;
    
}
