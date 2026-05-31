#version 460 core

in vec2 a_texcoord_out;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;

// TERRAIN RELATED UNIFORMS
uniform int enableCaustic;
uniform sampler2D texture_background;
uniform sampler2D texture_r;
uniform sampler2D texture_g;
uniform sampler2D texture_b;
uniform sampler2D texture_blendMap;
uniform float u_alpha;

uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

// CAUSTIC RELATED UNIFORMS
uniform vec2      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float temp;

out vec4 FragColor;



float tau = 6.3;

float sin01(float x) {
	return (sin(x*tau)+1.)/2.;
}
float cos01(float x) {
	return (cos(x*tau)+1.)/2.;
}

// rand func from theartofcode (youtube channel)
vec2 rand01(vec2 p) {
    vec3 a = fract(p.xyx * vec3(123.5, 234.34, 345.65));
    a += dot(a, a+34.45);
    
    return fract (vec2(a.x * a.y, a.y * a.z));
}

float circ(vec2 uv, vec2 pos, float r) {
    return smoothstep(r, 0., length(uv - pos));
}

float smoothFract(float x, float blurLevel) {
	return pow(cos01(x), 1./blurLevel);
}

float manDist(vec2 from, vec2 to) {
    return abs(from.x - to.x) + abs(from.y - to.y);
}


float distFn(vec2 from, vec2 to) {
	float x = length (from - to);
    return pow (x, 4.);
}

float voronoi(vec2 uv, float t, float seed, float size) {
    
    float minDist = 100.;
    
    float gridSize = size;
    
    vec2 cellUv = fract(uv * gridSize) - 0.5;
    vec2 cellCoord = floor(uv * gridSize);
    
    for (float x = -1.; x <= 1.; ++ x) {
        for (float y = -1.; y <= 1.; ++ y) {
            vec2 cellOffset = vec2(x,y);
            
            // Random 0-1 for each cell
            vec2 rand01Cell = rand01(cellOffset + cellCoord + seed);
			
            // Get position of point
            vec2 point = cellOffset + sin(rand01Cell * (t+10.)) * .5;
            
			// Get distance between pixel and point
            float dist = distFn(cellUv, point);
    		minDist = min(minDist, dist);
        }
    }
    
    return minDist;
}



void main(void)
{
// Light Related
vec3 unitNormal = normalize(surfaceNormal);
vec3 unitLightVector = normalize(toLightVector);

float nDotl = dot(unitNormal,unitLightVector);
float brightness = max(nDotl,0.2);
vec3 diffuse = brightness * lightColor;

vec3 unitVectorToCamera = normalize(toCameraVector);
vec3 lightDirection = -unitLightVector;
vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);

float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
specularFactor = max(specularFactor,0.0);
float dampedFactor = pow(specularFactor,shineDamper);
vec3 finalSpecular = dampedFactor * reflectivity * lightColor;
//vec4 TotallightColor = vec4(diffuse,1.0) * texture(modelTexture,pass_textureCoordinates) + vec4(finalSpecular,1.0);

// Texture Related
vec4 blendMapColor = texture(texture_blendMap ,a_texcoord_out );

float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);

vec2 tiledCoords= a_texcoord_out * 100.0f;

vec4 backgroundTextureColor  = texture(texture_background ,tiledCoords ) * backTextureAmount;
vec4 rTextureColor = texture(texture_r ,tiledCoords ) * blendMapColor.r;
vec4 gTextureColor = texture(texture_g ,tiledCoords ) * blendMapColor.g;
vec4 bTextureColor = texture(texture_b ,tiledCoords ) * blendMapColor.b;

vec4 TotalColor = backgroundTextureColor + rTextureColor + gTextureColor+ bTextureColor;


FragColor = (vec4(diffuse,1.0) * TotalColor + vec4(finalSpecular,1.0) ) * u_alpha;
/////FragColor = vec4(1.0,0.0,1.0,1.0);

}