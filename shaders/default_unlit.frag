#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

in vec4 VertexColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;

vec4 texDiffuse = texture(material.texture_diffuse1, TexCoords);
vec4 texSpecular = texture(material.texture_specular1, TexCoords);

void main()
{
    FragColor = texDiffuse;
    return;
}