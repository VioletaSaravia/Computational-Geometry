#version 330 core
out vec4 FragColor;

in vec4 VertexColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    FragColor = vec4(0, 0, 0, 1);
}
