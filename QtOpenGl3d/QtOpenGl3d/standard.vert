#version 440

in vec3 VertexPosition;
in vec3 vColor;
in vec2 vTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 RotationMatrix;

void main()
{
    gl_Position = RotationMatrix * vec4( VertexPosition, 1.0 );
	ourColor = vColor;
	TexCoord = vTexCoord;
}