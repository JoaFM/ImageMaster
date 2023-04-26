#include "RenderUtils.h"
#include "Utils/IM_STD.h"
#include "Buffer.h"
#include "RenderTarget.h"
#include "D3DCompiler.h"

bool RenderUtils::CalcReflect_Bind(D3D11_SHADER_INPUT_BIND_DESC& BindDescript, D3D_SHADER_INPUT_TYPE InputType, std::map <std::string, UINT>& ListToAdd)
{
	if (BindDescript.Type == InputType)
	{
		ListToAdd[BindDescript.Name] = BindDescript.BindPoint;
		return true;
	}
	return false;
}
