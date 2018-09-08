#version 440

in vec3 VertexPosition;
//in vec3 vColor;
in vec3 vNormal;

out vec3 ourColor;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4( VertexPosition, 1.0 );
	ourColor = vec3(0.3f,0.8f,0.6f);
	Normal = vNormal;
	FragPos = vec3(model * vec4(VertexPosition, 1.0));
}