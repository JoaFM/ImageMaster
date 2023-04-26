#pragma once

#include "MainApp/Default_Headers.h"
#include <d3d11shader.h>

class RenderUtils
{
public:
	static bool CalcReflect_Bind(D3D11_SHADER_INPUT_BIND_DESC& BindDescript, D3D_SHADER_INPUT_TYPE InputType, std::map <std::string, UINT>& ListToAdd);
};
