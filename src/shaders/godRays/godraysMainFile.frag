#version 460 core

in vec2 texCoord;

layout(binding = 0)uniform sampler2D occlusionTexture;
layout(location = 0)uniform float density;
layout(location = 1)uniform float weight;
layout(location = 2)uniform float decay;
layout(location = 3)uniform float exposure;
layout(location = 4)uniform int numSamples;
layout(location = 5)uniform vec2 screenSpaceLightPos = vec2(0.0);

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 fragColor = texture(occlusionTexture,texCoord).xyz;
	
	vec2 deltaTextCoord = vec2(texCoord - screenSpaceLightPos.xy);

	vec2 textCoo = texCoord.xy;
	deltaTextCoord *= (1.0 /  float(numSamples)) * density;
	float illuminationDecay = 1.0;

	for(int i=0; i < numSamples ; i++){
		textCoo -= deltaTextCoord;
		vec3 samp = texture(occlusionTexture, textCoo).xyz;
		samp *= illuminationDecay * weight;
		fragColor += samp;
		illuminationDecay *= decay;
	}
	fragColor *= exposure;
	FragColor = vec4(fragColor, 1.0);

}
