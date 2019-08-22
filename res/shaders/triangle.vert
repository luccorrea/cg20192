#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 textureCoordinates;

out vec3 N;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	N = normalize((transpose(inverse(model))*vec4(normal,0.0f)).xyz) ;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}