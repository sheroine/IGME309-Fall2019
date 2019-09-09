#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool ifComplementary = true;

out vec4 Fragment;

void main()
{
	if(ifComplementary)
	{
		Fragment = vec4(vec3(1.0 - Color.r, 1.0 - Color.g, 1.0 - Color.b),1);
	}
	else
	{
		Fragment = vec4(Color,1);
	}
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		if(ifComplementary)
		{
			Fragment = vec4(vec3(1.0 - SolidColor.r, 1.0 - SolidColor.g, 1.0 - SolidColor.b), 1);
		}
		else
		{
			Fragment = vec4(SolidColor, 1);
		}

	return;
}