#version 330 core

layout (location = 0) in vec2 aPos;

uniform float xOffset;
uniform float yOffset;

void main()
{
    gl_Position = vec4(
        aPos.x + xOffset,
        aPos.y + yOffset,
        0.0,
        1.0
    );
}
