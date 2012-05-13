#include "material.h"

//Constructor
Material::Material()
{
	//set default values
	ambient[0] = 0.25f;
	ambient[1] = 0.25f;
	ambient[2] = 0.25f;
	diffuse[0] = 0.8f;
	diffuse[1] = 0.8f;
	diffuse[2] = 0.8f;
	specular[0] = 0.0f;
	specular[1] = 0.0f;
	specular[2] = 0.0f;
	expo = 70.0f;
}


//Deconstructor
Material::~Material()
{
}


/*
 *Set the ambient color of the material
 * float r - red component of the ambient color
 * float g - green component of the ambient color
 * float b - blue component of the ambient color
 */
void Material::setAmbientColor(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}


/*
 * Set the diffuse color of the material
 * float r - red component of the diffuse colour
 * float g - green component of the diffuse colour
 * float b - blue component of the diffuse colour
 */
void Material::setDiffuseColor(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}


/*
 * Set the specular color of the material
 * float r - red component of the specular color
 * float g - green component of the specular color
 * float b - blue component of the specular color
 */
void Material::setSpecularColor(float r, float g, float b)
{
	specular[0] =r;
	specular[1] = g;
	specular[2] = b;
}


/*
 * Set the specular coefficient of the specular color
 * float _expo - the specular coefficient
 */
void Material::setSpecularExpo(float _expo)
{
	expo = _expo;
}


/*
 * Get the ambient colour
 * return - pointer to the ambient color
 */
float* Material::getAmbientColor()
{
	return ambient;
}


/*
 * Get the diffuse color
 * return - pointer to the diffuse color
 */
float* Material::getDiffuseColor()
{
	return diffuse;
}


/*
 * Get the specular color
 * return  pointer to the specular color
 */
float* Material::getSpecularColor()
{
	return specular;
}


/*
 * Get the specular coefficient
 * return - the specular coefficient
 */
float Material::getSpecularExpo()
{
	return expo;
}