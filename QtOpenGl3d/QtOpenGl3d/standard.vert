#version 440

in vec3 VertexPosition;
//in vec3 vColor;
in vec2 vTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4( VertexPosition, 1.0 );
	//ourColor = vColor;
	TexCoord = vTexCoord;
}