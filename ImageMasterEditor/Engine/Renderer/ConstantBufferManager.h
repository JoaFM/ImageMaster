#pragma once

#include "../../MainApp/Default_Headers.h"
#include <d3d11.h>
#include <d3d11shader.h>
#include "../Renderer/RenderTypes.h"
#include "ConstantBuffer.h"



struct VaribleDesk
{
	D3D_SHADER_VARIABLE_TYPE Type;
	UINT Offset = 10000;
	UINT SizeInBytes = 10000;
	UINT VaribleIndex = 10000;
	RenderTypes::VaribleGroup VarData;
};

struct VaribleContainer
{
public:
	VaribleContainer(D3D_SHADER_VARIABLE_TYPE Type, void* Data, UINT SizeInBytes);

	D3D_SHADER_VARIABLE_TYPE Type;
	UINT Count = 1;
	RenderTypes::VaribleGroup Data;

	VaribleContainer() {};
};

struct MaterialInstance
{
	std::map<std::string, VaribleContainer> m_ShaderVaribles;
	std::map<std::string, class Texture*> m_TexturesIn;
	std::map<std::string, class Texture*> m_TexturesOut;

	void SetParamater(std::string VaribleName, IM_Math::Int2& Value);
};

class ConstantBufferManager 
{
public:
	ConstantBufferManager(class Renderer* renderer);
	void Update(UINT32 GlobalsBindPoint, ID3D11ShaderReflection* pReflector);
	bool IsReady() { return IsUpdated; }
	void UpdateCB(MaterialInstance& Params);
	ConstantBuffer* GetCB() { return m_CB_Object.get(); };
private:
	bool IsUpdated = false;
	UINT m_CB_BindPoint = 0;
	std::map<std::string, VaribleDesk> m_ShaderVaribles;
	void BuildCB_render();
	UINT CalcSizeOfInputData();
	std::unique_ptr<ConstantBuffer> m_CB_Object = nullptr;
	class Renderer* m_Renderer = nullptr;
};