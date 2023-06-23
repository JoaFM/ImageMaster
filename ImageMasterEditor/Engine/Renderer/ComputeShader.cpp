#include "ComputeShader.h"
#include "Utils/IM_STD.h"
#include "Buffer.h"
#include "RenderTarget.h"
#include "D3DCompiler.h"
#include "RenderUtils.h"
#include "Texture.h"
#include "ShaderIncludeFramework.h"

ComputeShader::ComputeShader(std::string Name, class Renderer* renderer)
{
	m_Renderer = renderer;
	SetFriendlyName(Name + TAUtils::RandomString(5));
	GlobalsCBManager = std::make_unique<ConstantBufferManager>(m_Renderer);
}

ComputeShader::~ComputeShader()
{
	Release();
}

void ComputeShader::Release()
{
	TA_SAFERELEASE(m_ComputeShader);
}

void ComputeShader::LoadReload(ID3D11Device* Device)
{
	Release();

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
	LPCSTR profile = (Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	 
	FrameworkIncludeCompute IncludeFrameWork;

	//HRESULT hr = D3DCompileFromFile(TAUtils::Paths::instance().ABS_Path(m_ShaderPath).c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
	HRESULT hr = D3DCompileFromFile(m_ShaderPath.c_str(), defines, &IncludeFrameWork,
		"CSMain", profile,
		flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob) {
			//OutputDebugStringA((char*)error_blob->GetBufferPointer());
			TA_ERROR_C((char*)errorBlob->GetBufferPointer());
			TA_SAFERELEASE(errorBlob);
			return;
		}
		else
		{
			TA_ERROR_WS(L"Compute Shader failed to compile for some reason.");
		}
		LoadedAndValid = false;
		return;
	}

	TA_SAFERELEASE(m_ComputeShader);

	hr = Device->CreateComputeShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL, &m_ComputeShader);


	if (FAILED(hr))
	{
		if (errorBlob) {
			//OutputDebugStringA((char*)error_blob->GetBufferPointer());
			TA_ERROR_C((char*)errorBlob->GetBufferPointer());
			TA_SAFERELEASE(errorBlob);
			return;
		}
		else
		{
			TA_ERROR_WS(L"Compute Shader failed to create. But did compile for some reason.");
		}
		LoadedAndValid = false;
		return;
	}

	TAUtils::SetDebugObjectName(m_ComputeShader, GetFriendlyName());
	CalcRelection(shaderBlob);
	TA_SAFERELEASE(shaderBlob);
	LoadedAndValid = true;
}

void ComputeShader::SetTexture(std::string Name, class Texture* NewTexture)
{
	m_App_BoundTextures[Name] = NewTexture;
}


void ComputeShader::Dispatch(INT32 ThreadsX, INT32 ThreadsY, INT32 ThreadsZ)
{
	Bind();
	m_Renderer->GetDeviceContext()->Dispatch(ThreadsX, ThreadsY, ThreadsZ);
	UnBind();
}


void ComputeShader::Dispatch()
{
	if (m_App_BoundTextures.contains("BufferOut") && m_App_BoundTextures["BufferOut"])
	{
		Dispatch( m_App_BoundTextures["BufferOut"]->GetSize().x / 8, m_App_BoundTextures["BufferOut"]->GetSize().y / 8, 1);
	}

}

void ComputeShader::Dispatch(MaterialInstance& Params)
{
	// update custom constant buffer
	if (!LoadedAndValid) { return; }

	std::string StdOutputTexture = "BufferOut";
	auto it = Params.m_TexturesOut.find(StdOutputTexture);
	if (it != Params.m_TexturesOut.end())
	{
		GlobalsCBManager->UpdateCB(Params);

		if (ConstantBuffer* CB = GlobalsCBManager->GetCB())
		{
			BindTExturesFromParam(Params);
			CB->Bind_CB_to_CS();
			m_Renderer->GetDeviceContext()->Dispatch(Params.m_TexturesOut["BufferOut"]->GetSize().x / 8, Params.m_TexturesOut["BufferOut"]->GetSize().y / 8, 1);
			CB->UnBind_CB();
			UnBind();
		}
	}
}
bool ComputeShader::BindTExturesFromParam(MaterialInstance& Params)
{

	if (!LoadedAndValid)
	{
		TA_ERROR_WS(L"Compute Shader not Valid cannot run, Not Properly loaded\n");
		return false;
	}

	m_Renderer->GetDeviceContext()->CSSetShader(m_ComputeShader, NULL, 0);

	for (auto& RWTexture : m_TextureRW_BindPoints)
	{
		
		if (Params.m_TexturesOut.contains(RWTexture.first))
		{
			ID3D11UnorderedAccessView* m_UAV = Params.m_TexturesOut[RWTexture.first]->GetUAV();
			m_Renderer->GetDeviceContext()->CSSetUnorderedAccessViews(RWTexture.second, 1, &m_UAV, 0);
			m_ShaderBound_UAV.push_back(RWTexture.second);
		}
		else
		{
			TA_ERROR_WS(L"Could not bind RW texture in compute shader. Texture not set\n");
			return false;
		}
	}

	for (auto& TextureBP : m_Texture_BindPoints)
	{
		if (Params.m_TexturesIn.contains(TextureBP.first))
		{
			ID3D11ShaderResourceView* m_SRV = Params.m_TexturesIn[TextureBP.first]->GetSRV();
			m_Renderer->GetDeviceContext()->CSSetShaderResources(TextureBP.second, 1, &m_SRV);
			m_ShaderBound_SRV.push_back(TextureBP.second);
		}
		else
		{
			TA_ERROR_WS(L"Could not bind RW texture in compute shader. Texture not set\n");
			return false;
		}
	}


	ID3D11UnorderedAccessView* UAV = nullptr;
	if (m_PropertiesStructuredBuffer)
	{
		UAV = m_PropertiesStructuredBuffer->GetUAV();
	}
	return true;
}

bool ComputeShader::Bind()
{

	if (!LoadedAndValid)
	{
		TA_ERROR_WS(L"Compute Shader not Valid cannot run, Not Properly loaded\n");
		return false;
	}

	m_Renderer->GetDeviceContext()->CSSetShader(m_ComputeShader, NULL, 0);
	
	for (auto& RWTexture : m_TextureRW_BindPoints)
	{
		if (m_App_BoundTextures.contains(RWTexture.first))
		{
			ID3D11UnorderedAccessView* m_UAV = m_App_BoundTextures[RWTexture.first]->GetUAV();
			m_Renderer->GetDeviceContext()->CSSetUnorderedAccessViews(RWTexture.second, 1, &m_UAV, 0);
			m_ShaderBound_UAV.push_back(RWTexture.second);
		}
		else
		{
			TA_ERROR_WS(L"Could not bind RW texture in compute shader. Texture not set\n");
			return false;
		}
	}

	for (auto& TextureBP : m_Texture_BindPoints)
	{
		if (m_App_BoundTextures.contains(TextureBP.first))
		{
			ID3D11ShaderResourceView* m_SRV = m_App_BoundTextures[TextureBP.first]->GetSRV();
			m_Renderer->GetDeviceContext()->CSSetShaderResources(TextureBP.second, 1, &m_SRV);
			m_ShaderBound_SRV.push_back(TextureBP.second);
		}
		else
		{
			TA_ERROR_WS(L"Could not bind RW texture in compute shader. Texture not set\n");
			return false;
		}
	}


 	ID3D11UnorderedAccessView* UAV = nullptr;
 	if (m_PropertiesStructuredBuffer)
 	{
 		 UAV = m_PropertiesStructuredBuffer->GetUAV();
 	}
	return true;
}

bool ComputeShader::UnBind()
{
	ID3D11UnorderedAccessView* nullUAV = { NULL };
	for (INT32 i = 0; i < m_ShaderBound_UAV.size(); i++)
	{
		m_Renderer->GetDeviceContext()->CSSetUnorderedAccessViews(m_ShaderBound_UAV[i], 1, &nullUAV, 0);
	}
	ID3D11ShaderResourceView* nullSRV = { NULL };
	for (INT32 i = 0; i < m_ShaderBound_SRV.size(); i++)
	{
		m_Renderer->GetDeviceContext()->CSSetShaderResources(m_ShaderBound_SRV[i], 1, &nullSRV);
	}
	m_ShaderBound_SRV.clear();
	m_ShaderBound_UAV.clear();
	SetBuffer(nullptr);
	m_App_BoundTextures.clear();
	m_Renderer->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
	return true;
}

void ComputeShader::CalcRelection(ID3DBlob* Shader_blob_ptr)
{
	m_TextureRW_BindPoints.clear();

	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(Shader_blob_ptr->GetBufferPointer(), Shader_blob_ptr->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&pReflector);

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc(&desc);


	for (UINT i = 0; i < desc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		pReflector->GetResourceBindingDesc(i, &bindDesc);
	

		UINT BindPoint = bindDesc.BindPoint;

		if (!(
			RenderUtils::CalcReflect_Bind(bindDesc, D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED, m_TextureRW_BindPoints) ||
			RenderUtils::CalcReflect_Bind(bindDesc, D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER, m_CBuffer_BindPoints) ||
			RenderUtils::CalcReflect_Bind(bindDesc, D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE, m_Texture_BindPoints)
			))
		{
			TA_ERROR_WS(L"Unknown Texture Resource type in Compute Shader");
		}
	}

	if (m_CBuffer_BindPoints.contains("$Globals"))
	{
		GlobalsCBManager->Update(m_CBuffer_BindPoints["$Globals"], pReflector);
	}
	else
	{
		GlobalsCBManager->Update(0, nullptr);
	}

	TA_SAFERELEASE(pReflector);
}