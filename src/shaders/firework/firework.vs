#version 460 core

in vec3 aPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float u_time;
uniform float u_pointSize;

out float v_life;

const float PI = 3.14159;
const float lifetime = 1000.0;
const float height = 6.0;
const float width = 2.25;
void main() 
{
    float currentTime = u_time + aPosition.z * lifetime;
    v_life = mod(currentTime, lifetime) / lifetime;
	// First 35% of life represents first 50% of animation progress, last 65% of life
	// represents last 50% of animation.
    float progress = v_life < 0.35 ? (v_life / 0.35 * 0.5) : ((v_life - 0.35) / 0.65 * 0.5 + 0.5);
	// Further, we scale the progress based on how high the particles will go.
	// Lower particles will play faster, to give the fountain a better sense of speed.
    float scaledProgress = clamp(progress / pow(aPosition.y, 0.65), 0.0, 1.0);
    float x = aPosition.x * width * scaledProgress;
    float y = -1.0 + aPosition.y * 2.0 * height * sin(scaledProgress * PI);
    float z = aPosition.z * scaledProgress;
    gl_PointSize = u_pointSize * ((1.0 - progress) * 0.75 + 0.25);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(x, y, z, 1.0);
}
