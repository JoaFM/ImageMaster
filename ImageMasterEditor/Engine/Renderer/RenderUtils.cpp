#include "RenderUtils.h"
#include "Utils/IM_STD.h"
#include "Buffer.h"
#include "RenderTarget.h"
#include "D3DCompiler.h"
#include "Renderer.h"

bool RenderUtils::CalcReflect_Bind(D3D11_SHADER_INPUT_BIND_DESC& BindDescript, D3D_SHADER_INPUT_TYPE InputType, std::map <std::string, UINT>& ListToAdd)
{
	if (BindDescript.Type == InputType)
	{
		ListToAdd[BindDescript.Name] = BindDescript.BindPoint;
		return true;
	}
	return false;
}

void RenderUtils::ProfileTag(class Renderer* renderer, std::wstring Name)
{
	if (renderer != nullptr)
	{
		renderer->m_PerfObject->SetMarker(Name.c_str());
	}
}

RenderUtils::ScopedProfile::ScopedProfile(class Renderer* renderer, std::wstring Name)
{
	if (renderer != nullptr)
	{
		m_Renderer = renderer;
		m_Renderer->m_PerfObject->BeginEvent(Name.c_str());
	}
}

RenderUtils::ScopedProfile::~ScopedProfile()
{
	if (m_Renderer != nullptr)
	{
		m_Renderer->m_PerfObject->EndEvent();
	}
}
