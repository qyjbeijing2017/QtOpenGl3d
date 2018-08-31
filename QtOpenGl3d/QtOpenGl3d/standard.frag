#version 440

out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform vec3 lightPos;

vec3 ambientlightColor = vec3(1f,1f,1f);
float ambientStrength  = 0.1f;

void main()
{
	vec3 ambient = ambientlightColor*ambientStrength;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * ambientlightColor;

    FragColor = vec4(ambient + diffuse, 1f) * mix( texture(ourTexture,TexCoord) , texture(ourTexture1,TexCoord), 0.2f);
	//FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
	//FragColor = vec4(TexCoord.xy,0.0f, 1.0f); 
}