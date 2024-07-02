#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 VertexColor;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model; 
uniform mat4 view; 
uniform mat4 projection;
uniform mat4 transform;

void main() {
	vec4 transformed = transform * vec4(aPos, 1);
	gl_Position = projection * view * model * transformed;

	VertexColor = vec4(1);
	FragPos = vec3(model * transformed);

	// Ver [2]
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoord;
}