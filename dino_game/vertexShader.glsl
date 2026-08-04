#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 offset;
uniform vec2 scale;

void main()
{    vec2 finalPosition = (aPos * scale) + offset;
    
    gl_Position = vec4(finalPosition, 0.0f, 1.0f);
}