#version 440

in vec3 VertexPosition;
//in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4( VertexPosition, 1.0 );
	//ourColor = vColor;
	TexCoord = vTexCoord;
	Normal = vNormal;
	FragPos = vec3(model * vec4(VertexPosition, 1.0));
}