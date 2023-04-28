#pragma once

#include <iostream>
#include <string>
#include "Renderer.h"
#include <map>
#include "RenderTypes.h"
class ShaderIncludes
{

public:

	static ShaderIncludes& Instance()
	{
		static ShaderIncludes  instance; // Guaranteed to be destroyed.
								 // Instantiated on first use.
		return instance;
	}
	ShaderIncludes(ShaderIncludes const&) = delete;
	void operator=(ShaderIncludes const&) = delete;


	std::string GetInclude(std::string FileName)
	{
		if (Includes.find(FileName) != Includes.end())
		{
			return Includes[FileName];
		}
		else
		{
			return std::string();
		}
	}


private:
	std::map<std::string, std::string> Includes;
	ShaderIncludes() 
	{
		Includes["TAGlobals.h"] =
			RenderTypes::CB_General_Struct::GetHeaderDefine() +
			RenderTypes::CB_PerScreenSprite_Struct::GetHeaderDefine() +
			RenderTypes::CB_BrushInput_Struct::GetHeaderDefine();
	}

};