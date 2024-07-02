#version 330 core
out vec3 TexCoords;

layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // makes the depth always == 1.0, since w / w = 1
}
