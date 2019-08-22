#version 330 core

in vec3 N;

void main()
{
	// isso e oq da a cor do trianfulo
	gl_FragColor = vec4((N+1)/2, 1.0f);
}