#version 330 core
in vec4 ourColor;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D Texture0;

uniform vec3 textureSize;
uniform float lineWidth;
uniform vec4 lineColor;

const float cosArray[12] = {1, 0.866, 0.5, 0, -0.5, -0.866, -0.1, -0.866, -0.5, 0, 0.5, 0.866};  
const float sinArray[12] = {0, 0.5, 0.866, 1, 0.866, 0.5, 0, -0.5, -0.866, -1, -0.866, -0.5};  

bool HasAlphaWithAngelIndex(int index)
{
    vec2 texCoords = vec2(TexCoords.x + lineWidth*cosArray[index]/textureSize.x,
					TexCoords.y + lineWidth*sinArray[index]/textureSize.y);
	vec4 texColor = texture(Texture0, texCoords);
	
	if(texColor.a < 0.5 || 
		texCoords.x < 0.0 || texCoords.x > 1.0 ||
		texCoords.y < 0.0 || texCoords.y > 1.0){
		return false;
	}
	else{
		return true;
	}
}

void main()
{   
	vec4 texColor = texture(Texture0, TexCoords);
	if(texColor.a > 0.5)
	{
		color = texColor;
		return;
	}

	// alpha is zero,judge if stroke
	bool bStroke = false;
	for(int i = 0; i < 12; i++){
		if(HasAlphaWithAngelIndex(i)){
			bStroke = true;
			break;
		}
	}

	if(bStroke){
		color = vec4(1.0, 1.0, 1.0, 1.0);
	}

}