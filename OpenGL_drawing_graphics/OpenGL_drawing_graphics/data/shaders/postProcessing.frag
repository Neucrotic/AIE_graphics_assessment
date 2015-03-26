#version 410

in vec2 fTexCoord;

out vec4 FragColour;

uniform sampler2D target;

//just sample the target and return the colour
vec4 Simple()
{
	return texture(target, fTexCoord);
}

//simple box blur
vec4 BoxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	
	//9-tap box kernel
	vec4 colour = texture(target, fTexCoord);
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0f));
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, fTexCoord + vec2(0f, texel.y));
	colour += texture(target, fTexCoord + vec2(0f, -texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, 0f));
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y));
	
	return colour / 9;
}

//distortion method
vec4 Distort()
{
	vec2 mid = vec2(0.5f);
	
	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;
	
	vec2 newCoord = mid + bias * normalizedCoord;
	
	return texture(target, newCoord);
}

void main()
{
	//FragColour = Simple();
	//FragColour = BoxBlur();
	FragColour = Distort();
}