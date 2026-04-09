#include <cmath>
#include <cstddef>
#include "./glshaderloader.h"
#include "./glLight.h"
#include <iostream>
#include <string>

using namespace std;
using namespace vmath;

static int mode = 0;
static int selectedLight = 0;

SceneLight::SceneLight(bool envLight) : envProgram(nullptr),
										irradianceProgram(nullptr),
										prefilterProgram(nullptr),
										precomputeBRDF(nullptr)
{

	// setup light program
	this->indirectLight = envLight;
	ambient = vec3(0.0f);
}

SceneLight::~SceneLight()
{

	if (envProgram)
		delete envProgram;

	if (irradianceProgram)
		delete irradianceProgram;

	if (prefilterProgram)
		delete prefilterProgram;

	if (precomputeBRDF)
		delete precomputeBRDF;
}

void SceneLight::setEnvmap(GLuint &tex)
{
	// this->envirounmentMap = envMap;
	this->envirounmentMap = tex;
	// set irradiance and prefilter positions
	// this->irradianceMap->setPosition(this->envirounmentMap->getPosition());
	// this->prefilterMap->setPosition(this->envirounmentMap->getPosition());
}

void SceneLight::setAmbient(vec3 ambient)
{
	this->ambient = ambient;
}

void SceneLight::addDirectionalLights(vector<DirectionalLight> dl)
{
	this->directional.insert(this->directional.end(), dl.begin(), dl.end());
}

void SceneLight::addPointLights(vector<PointLight> pl)
{
	this->points.insert(this->points.end(), pl.begin(), pl.end());
}

void SceneLight::addSpotLights(vector<SpotLight> sl)
{
	this->spots.insert(this->spots.end(), sl.begin(), sl.end());
}

void SceneLight::setLightUniform(glshaderprogram *program, bool useIndirectLight)
{
	// Directional Lights
	glUniform1i(program->getUniformLocation("numOfDL"), directional.size());
	for (size_t i = 0; i < directional.size(); i++)
	{
		glUniform3fv(program->getUniformLocation("dl[" + to_string(i) + "].base.color"), 1, directional[i].color);
		glUniform3fv(program->getUniformLocation("dl[" + to_string(i) + "].direction"), 1, normalize(directional[i].direction));
		glUniform1f(program->getUniformLocation("dl[" + to_string(i) + "].base.intensity"), directional[i].intensity);
	}

	glUniform1i(program->getUniformLocation("numOfPoints"), points.size());
	for (size_t i = 0; i < points.size(); i++)
	{

		glUniform3fv(program->getUniformLocation("pl[" + to_string(i) + "].base.color"), 1, points[i].color);
		glUniform3fv(program->getUniformLocation("pl[" + to_string(i) + "].position"), 1, points[i].position);
		glUniform1f(program->getUniformLocation("pl[" + to_string(i) + "].base.intensity"), points[i].intensity);
		glUniform1f(program->getUniformLocation("pl[" + to_string(i) + "].radius"), points[i].radius);
	}

	glUniform1i(program->getUniformLocation("numOfSpots"), spots.size());
	for (size_t i = 0; i < spots.size(); i++)
	{

		glUniform3fv(program->getUniformLocation("sl[" + to_string(i) + "].point.base.color"), 1, spots[i].color);
		glUniform3fv(program->getUniformLocation("sl[" + to_string(i) + "].point.position"), 1, spots[i].position);
		glUniform3fv(program->getUniformLocation("sl[" + to_string(i) + "].direction"), 1, spots[i].direction);
		glUniform1f(program->getUniformLocation("sl[" + to_string(i) + "].point.base.intensity"), spots[i].intensity);
		glUniform1f(program->getUniformLocation("sl[" + to_string(i) + "].point.radius"), spots[i].radius);
		glUniform1f(program->getUniformLocation("sl[" + to_string(i) + "].inner_angle"), spots[i].inner_angle);
		glUniform1f(program->getUniformLocation("sl[" + to_string(i) + "].outer_angle"), spots[i].outer_angle);
	}

	if (this->indirectLight && useIndirectLight)
	{
		// glUniform1i(program->getUniformLocation("IBL"),this->indirectLight);
		// glBindTextureUnit(8,irradianceMap->cubemap_texture);
		// glBindTextureUnit(9,prefilterMap->cubemap_texture);
		// glBindTextureUnit(10,brdf.brdfTex);
	}
	else
	{
		glUniform3fv(program->getUniformLocation("ambientColor"), 1, ambient);
	}
}

void SceneLight::setDirectionalLightColor(int i, vmath::vec3 color)
{
	this->directional[i].update_color(color);
}

void SceneLight::setDirectionalLightDirection(int i, vmath::vec3 direction)
{
	this->directional[i].setDirection(direction);
}

void SceneLight::setPointLightColor(int i, vmath::vec3 color)
{
	this->points[i].update_color(color);
}
void SceneLight::setPointLightPosition(int i, vmath::vec3 position)
{
	this->points[i].update_position(position);
}

void SceneLight::setSpotLightPosition(int i, vmath::vec3 position)
{
	this->spots[i].update_position(position);
}
