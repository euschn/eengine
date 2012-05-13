/*
 * Class for the material information of the obj-files
 */


#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H


#include "common.hpp"

#include <sstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <map>

class Material
{
public:
	Material();	//Constructor
	~Material();	//Deconstructor

	void setAmbientColor(float r, float g, float b);	//Set the ambient color
	void setDiffuseColor(float r, float g, float b);	//Set the diffuse color
	void setSpecularColor(float r, float g, float b);	//Set the specular color
	void setSpecularExpo(float expo);	//Set the specular coefficient
	float* getAmbientColor();	//Get the ambient color
	float* getDiffuseColor();	//get the diffuse color
	float* getSpecularColor();	//get the specular color
	float getSpecularExpo();		//get the specular coefficient

	//void uploadToShader(Shader* shader);


private:

	float ambient[3];	//ambient term
	float diffuse[3];	//diffuse term
	float specular[3];	//specular term
	float expo;			//specular coefficient

};
#endif