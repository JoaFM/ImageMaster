#include "pch.h"
#include "ConstantBufferManager.h"
#include "Utils/IM_STD.h"
#include <d3d11.h>
#include "Renderer.h"


ConstantBufferManager::ConstantBufferManager(class Renderer* renderer)
{
	m_Renderer = renderer;
}

void ConstantBufferManager::Update(UINT32 GlobalsBindPoint, ID3D11ShaderReflection* pReflector)
{
	IsUpdated = false;
	m_ShaderVaribles.clear();
	if (pReflector == nullptr) { return; }
	m_CB_BindPoint = GlobalsBindPoint;
	ID3D11ShaderReflectionConstantBuffer* Globals = pReflector->GetConstantBufferByName("$Globals");
	D3D11_SHADER_BUFFER_DESC ShaderDesk;
	Globals->GetDesc(&ShaderDesk);

	

	for (UINT VarIndex = 0; VarIndex < ShaderDesk.Variables; VarIndex++)
	{
		ID3D11ShaderReflectionVariable* VarDes = Globals->GetVariableByIndex(VarIndex);
		ID3D11ShaderReflectionType* Type = VarDes->GetType();
		
		D3D11_SHADER_TYPE_DESC ShaderTypeD;
		 Type->GetDesc(&ShaderTypeD);

		D3D11_SHADER_VARIABLE_DESC SaderVarD;
		VarDes->GetDesc(&SaderVarD);

		VaribleDesk VD;
		VD.Type = ShaderTypeD.Type;
		VD.VaribleIndex = VarIndex;
		VD.Offset = ShaderTypeD.Offset;
		VD.SizeInBytes = SaderVarD.Size;
		m_ShaderVaribles[std::string(SaderVarD.Name)] = VD;
	}
	IsUpdated = true;

	BuildCB_render();
}

void ConstantBufferManager::UpdateCB(MaterialInstance& Params)
{
	if (!IsUpdated) { return; }
	std::vector<UINT8> NewData;
	NewData.clear();
	NewData.assign(CalcSizeOfInputData(), 0);

	for (auto& var : m_ShaderVaribles)
	{
		VaribleDesk& VD = var.second;
		std::memcpy(&NewData[VD.Offset], &Params.m_ShaderVaribles[var.first].Data, VD.SizeInBytes);
	}

	m_CB_Object->UpdateData(&NewData[0]);
}



void ConstantBufferManager::BuildCB_render()
{

	D3D11_BUFFER_DESC constantBufferDesc;

	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = CalcSizeOfInputData();;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	m_CB_Object = std::make_unique<ConstantBuffer>(constantBufferDesc, "CB_Globals", m_Renderer, m_CB_BindPoint);

}

UINT ConstantBufferManager::CalcSizeOfInputData()
{
	UINT totalSize = 0;
	for (auto& var : m_ShaderVaribles)
	{
		totalSize += var.second.SizeInBytes;
	}

	double totalSizef = (double)totalSize;
	totalSizef /= 16;
	totalSizef = ceil(totalSizef);
	totalSizef *= 16;

	return (UINT)totalSizef;
}

void MaterialInstance::SetParamater(std::string VaribleName, IM_Math::Int2& Value)
{
	m_ShaderVaribles[VaribleName] = VaribleContainer(D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_INT, &Value , sizeof(IM_Math::Int2));
}

VaribleContainer::VaribleContainer(D3D_SHADER_VARIABLE_TYPE _Type, void* _Data, UINT SizeInBytes)
{
	Type = _Type;
	Count = SizeInBytes/4;
	memcpy(&Data, _Data, SizeInBytes);
}
