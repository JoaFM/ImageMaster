#include "Shader.h"

#include <iostream>
#include <string>
#include <set>

#include <d3d11.h>       // D3D interface
#include <d3dcompiler.h> // shader compiler
#include <d3d11shader.h>
#include <assert.h>

#include "Renderer.h"
#include "Texture.h"
#include "Buffer.h"

#include <utility>
#include <filesystem>
#include "ShaderIncludes.h"
#include "RenderTypes.h"
#include "ShaderIncludeFramework.h"




Shader::Shader()
{
}


void Shader::SetTexture(std::wstring Name, class Texture* NewTexture)
{
	m_App_BoundTextures[Name] = NewTexture;
}

void Shader::SetBuffer(std::wstring Name, class Buffer* NewBuffer)
{
	m_App_BoundBuffers[Name] = NewBuffer;
}

void Shader::FindDefines(std::string& FileDataString)
{
	auto FindDefine = [&](std::string DefineName)
	{
		INT32 Index = (INT32)FileDataString.find(DefineName);
		return Index >= 0;
	};

	if (FindDefine("#define Blend_AlphaBlend"))
	{
		TA_LOG_WS(L"ShaderSet: Blend_AlphaBlend");
		m_BlendMode = BlendMode::AlphaBlend;
	}
	else if (FindDefine("#define Blend_Add"))
	{
		TA_LOG_WS(L"ShaderSet: Blend_Add");
		m_BlendMode = BlendMode::Addetive;
	}
	else if (FindDefine("#define Blend_AlphaMask"))
	{
		TA_LOG_WS(L"ShaderSet: Blend_AlphaMask");
		m_BlendMode = BlendMode::AlphaMask;
	}
	else
	{
		TA_LOG_WS(L"ShaderSet: Blendmode default (OPTION: Blend_AlphaBlend, Blend_Add, Blend_AlphaMask) ");
		m_BlendMode = BlendMode::Default;
	}

	if (FindDefine("#define Wireframe"))
	{
		TA_LOG_WS(L"ShaderSet: Rendering Wireframe");
		m_Wireframe = true;
	}
	else
	{
		TA_LOG_WS(L"OPTION: '#define Wireframe' ");
		m_Wireframe = false;
	}

	if (FindDefine("#define DontCullBack"))
	{
		TA_LOG_WS(L"ShaderSet: Dont Cull backfaces");
		m_BackfaceCulling = false;
	}
	else
	{
		TA_LOG_WS(L"OPTION: '#define DontCullBack'");
		m_BackfaceCulling = true;
	}

	if (FindDefine("#define DontDepthTest"))
	{
		TA_LOG_WS(L"ShaderSet: #define DontDepthTest");
		m_EnableDepthTesting = false;
	}
	else
	{
		TA_LOG_WS(L"OPTION: '#define DontDepthTest'");
		m_EnableDepthTesting = true;

	}
	
}




void Shader::LoadReload(Renderer* renderer)
{
	m_LoadedAndValid = false;
	/// compile shaders

	UINT flags1 = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags1 |= D3DCOMPILE_DEBUG; // add more debug output
#endif

	ID3DBlob* error_blob = nullptr;
	{
		//cleanup
		Release();


		m_PS_BoundTextures.clear();
		m_PS_BoundBuffers.clear();
		m_PS_BoundSamplers.clear();

		m_SRVSlotsBoundInPS.clear();
		m_ShaderSlotsBoundedPS.clear();

		// #TODO Just read some of the CB meta data back so one can ask about it in c#
		// #TODO Vertex shader texture support
		//m_PS_BoundCB.clear();
		//m_VS_BoundTextures.clear();
		//m_VS_BoundBuffers.clear();
		//m_VS_BoundSamplers.clear();
		//m_VS_BoundCB.clear();

	}



	std::vector<D3D_SHADER_MACRO> dataDebugVEC = renderer->GetGlobalHashDefines();
	D3D_SHADER_MACRO* pDefines = dataDebugVEC.data();

	std::wstring  ABS_ShaderPath = TAUtils::Paths::instance().ABS_Path(m_ShaderPath);
	TA_LOG_WS(L"Compiling: " + ABS_ShaderPath);

	if (!TAUtils::FileExist(ABS_ShaderPath))
	{
		TA_ERROR_WS(L"Shader path not valid: ");
		TA_ERROR_WS(ABS_ShaderPath.c_str());
		m_LoadedAndValid = false;
		return;
	}


	FrameworkInclude IncludeFrameWork;
	// COMPILE VERTEX 
	std::string FileData = TAUtils::GetFileAsSting(ABS_ShaderPath);

	FindDefines(FileData);



	HRESULT hr = D3DCompile(
		FileData.c_str(),
		FileData.size(),
		nullptr,
		pDefines,
		& IncludeFrameWork,
		"vs_main",
		"vs_5_0",
		flags1,
		0,
		& vs_blob_ptr,
		& error_blob
	);
	

	if (FAILED(hr))
	{
		if (error_blob) {
			//OutputDebugStringA((char*)error_blob->GetBufferPointer());

			TA_ERROR_C((char*)error_blob->GetBufferPointer());
			error_blob->Release();
		}
		else
		{
			TA_ERROR_WS(L"Failed to compile the shader for some unknown reason");
		}
		if (vs_blob_ptr) { vs_blob_ptr->Release(); }
		//assert(false);
		m_LoadedAndValid = false;
		return;
	}

	// COMPILE PIXEL SHADER
	hr = D3DCompileFromFile(
		ABS_ShaderPath.c_str(),
		pDefines,
		&IncludeFrameWork,
		"ps_main",
		"ps_5_0",
		flags1,
		0,
		&ps_blob_ptr,
		&error_blob);

	if (FAILED(hr))
	{
		if (error_blob) {
			//OutputDebugStringA((char*)error_blob->GetBufferPointer());
			TA_ERROR_C((char*)error_blob->GetBufferPointer());
			error_blob->Release();
			return;
		}
		else
		{
			TA_ERROR_WS(L"Failed to compile for some unknown reason");
		}
		if (ps_blob_ptr) { ps_blob_ptr->Release(); }
		m_LoadedAndValid = false;
		return;
	}

	/// use shaders


	hr = renderer->GetDevice()->CreateVertexShader(
		vs_blob_ptr->GetBufferPointer(),
		vs_blob_ptr->GetBufferSize(),
		NULL,
		&vertex_shader_ptr);
	if (FAILED(hr))
	{
		TA_ERROR_WS(L"Could not create vertex shader ");
		m_LoadedAndValid = false;
		return;
	}
	//assert(SUCCEEDED(hr));


	hr = renderer->GetDevice()->CreatePixelShader(
		ps_blob_ptr->GetBufferPointer(),
		ps_blob_ptr->GetBufferSize(),
		NULL,
		&pixel_shader_ptr);
	//assert(SUCCEEDED(hr));

	if (FAILED(hr))
	{
		TA_ERROR_WS(L"Could not create Pixel shader ");
		m_LoadedAndValid = false;
		return;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VLayout = RenderTypes::GetStandardVertLayout();
	hr = renderer->GetDevice()->CreateInputLayout(
		&VLayout[0],
		(UINT32)VLayout.size(),
		vs_blob_ptr->GetBufferPointer(),
		vs_blob_ptr->GetBufferSize(),
		&m_inputLayout);
	//assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		TA_ERROR_WS(L"Could not Create Input Layout  ");
		m_LoadedAndValid = false;
		return;
	}

	TA_LOG_WS(L"2-> --Relection--");
	CalcRelection(vs_blob_ptr);
	CalcRelection(ps_blob_ptr);
	
	TAUtils::SetDebugObjectName(pixel_shader_ptr, m_ShaderPath.c_str());
	//TA_SetName(pixel_shader_ptr, "Hello");
	
	m_LoadedAndValid = true;
}

void Shader::Bind(Renderer* renderer)
{
	ID3D11DeviceContext* Device_Context = renderer->GetDeviceContext();

	Device_Context->IASetInputLayout(GetVertexLayout());

	Device_Context->VSSetShader(GetVertexShader(), NULL, 0);
	Device_Context->VSSetConstantBuffers(0, (UINT)RenderTypes::ConstanBuffer::NumConstantBuffers, renderer->GetConstantBuffers());

	Device_Context->PSSetShader(GetPixelShader(), NULL, 0);
	Device_Context->PSSetConstantBuffers(0, (UINT)RenderTypes::ConstanBuffer::NumConstantBuffers, renderer->GetConstantBuffers());

	std::set<std::wstring> SetShaders;


	for (auto PS_Texture : m_PS_BoundTextures)
	{
		const std::wstring TextureName = PS_Texture.first;

		auto AppTexture = m_App_BoundTextures.find(TextureName);
		
		if (AppTexture != m_App_BoundTextures.end())
		{
			Texture* CurrTexture = AppTexture->second;
			ID3D11ShaderResourceView* textureView = CurrTexture->GetSRV();
			Device_Context->PSSetShaderResources(PS_Texture.second, 1, &textureView);
			m_SRVSlotsBoundInPS.push_back(PS_Texture.second);
			struct ID3D11SamplerState* samplerState = renderer->GetSampler(CurrTexture->GetSampler());

			auto TexSampler = m_PS_BoundSamplers.find(TextureName + L"_sampler");
			SetShaders.insert(TextureName + L"_sampler");
			if (TexSampler != m_PS_BoundSamplers.end())
			{
				Device_Context->PSSetSamplers(TexSampler->second, 1, &samplerState);
				m_ShaderSlotsBoundedPS.push_back(TexSampler->second);
			}

		}
		else
		{
			TA_ERROR_WS(L"To texture was set on texture:" + TextureName);
		}
	}

	for (auto PS_samper : m_PS_BoundSamplers)
	{
		
		auto sampIt = SetShaders.find(PS_samper.first);
		if (sampIt == SetShaders.end())
		{
			TA_ERROR_WS(L"This sampler was never set:" + PS_samper.first);
		}

	}

	for (auto CurrBuffer : m_VS_BoundBuffers)
	{
		const std::wstring BufferName = CurrBuffer.first;
		auto AppBuffer = m_App_BoundBuffers.find(BufferName);
		if (AppBuffer != m_App_BoundBuffers.end())
		{
			if (ID3D11ShaderResourceView* SRV = AppBuffer->second->GetSRV())
			{
				Device_Context->VSSetShaderResources(CurrBuffer.second, 1, &SRV);
				m_SRVSlotsBoundInPS.push_back(CurrBuffer.second);
			}
			else
			{
				TA_ERROR_WS(L"Could not get SRV for the buffer so it can be used in the shader");
			}
		}
		else
		{
			TA_ERROR_WS(L"No buffer was bound for this shader:" + CurrBuffer.first);
		}
	}

	for (auto CurrBuffer : m_PS_BoundBuffers)
	{
		const std::wstring BufferName = CurrBuffer.first;
		auto AppBuffer = m_App_BoundBuffers.find(BufferName);
		if (AppBuffer != m_App_BoundBuffers.end())
		{
			if (ID3D11ShaderResourceView* SRV = AppBuffer->second->GetSRV())
			{
				Device_Context->PSSetShaderResources(CurrBuffer.second, 1, &SRV);
				m_SRVSlotsBoundInPS.push_back(CurrBuffer.second);
			}
			else
			{
				TA_ERROR_WS(L"Could not get SRV for the buffer so it can be used in the shader");
			}
		}
		else
		{
			TA_ERROR_WS(L"No buffer was bound for this shader:" + CurrBuffer.first);
		}
	}
	

	renderer->SetBlendState(m_BlendMode);
	renderer->SetRasterizer(m_Wireframe, m_BackfaceCulling);
	renderer->SetStencilState(m_EnableDepthTesting);
		
}

void Shader::UnBind(Renderer* renderer)
{
	ID3D11DeviceContext* Device_Context =renderer->GetDeviceContext();
	Device_Context->VSSetShader(nullptr, nullptr, 0);
	Device_Context->PSSetShader(nullptr, nullptr, 0);

	ID3D11ShaderResourceView* SRV_NULL = { 0 };

	for (UINT ResourceSlot : m_SRVSlotsBoundInPS)
	{
		Device_Context->PSSetShaderResources(ResourceSlot, 1, &SRV_NULL);
	}
	m_SRVSlotsBoundInPS.clear();
	struct ID3D11SamplerState* SS = { 0 };

	for (UINT SamplerSlot : m_ShaderSlotsBoundedPS)
	{
		Device_Context->PSSetSamplers(SamplerSlot, 1, &SS);
	}
	m_ShaderSlotsBoundedPS.clear();
}



void Shader::CalcRelection(ID3DBlob* Shader_blob_ptr)
{
	bool isPixel = true;

	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(Shader_blob_ptr->GetBufferPointer(), Shader_blob_ptr->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&pReflector);

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc(&desc);
	

	if(desc.Version == 0x10050)
	{
		D3D11_SHADER_VERSION_TYPE ShaderType= (D3D11_SHADER_VERSION_TYPE)D3D11_SHVER_GET_TYPE(desc.Version);

		if (ShaderType == D3D11_SHVER_PIXEL_SHADER)
		{
			isPixel = true;
			

		}
		else if (ShaderType == D3D11_SHVER_VERTEX_SHADER)
		{
			isPixel = false;
			
		}
		else
		{
			TA_ERROR_WS(L"Dont know what Shader type this is to do reflection on");
			return;
		}
	}

	if (isPixel) TA_LOG_WS(L"1-> Pixel Shader :");
	else TA_LOG_WS(L"1-> Vertex Shader :");
	TA_LOG_WS(L"->\t\tInstructionCount: " + std::to_wstring(desc.InstructionCount));

	for (UINT i = 0; i < desc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		pReflector->GetResourceBindingDesc(i, &bindDesc);

		std::wstring nameWS;
		{
			std::string name = std::string(bindDesc.Name);
			std::wstring wsTmp(name.begin(), name.end());
			nameWS = wsTmp;
		}

		TA_LOG_WS(L"->\t\tBinding: " + nameWS);

		UINT BindPoint = bindDesc.BindPoint;
		if (bindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
		{
			if (isPixel) m_PS_BoundTextures[nameWS] = BindPoint;
			else  m_PS_BoundTextures[nameWS] = BindPoint;
		}
		else if (bindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
		{
			if (isPixel) m_PS_BoundBuffers[nameWS] = BindPoint;
			else  m_VS_BoundBuffers[nameWS] = BindPoint;
		}
		else if (bindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
		{
			if (isPixel) m_PS_BoundSamplers[nameWS] = BindPoint;
			else  m_PS_BoundSamplers[nameWS] = BindPoint;
		}
		else if (bindDesc.Type == D3D_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER)
		{
			//if (isPixel) m_PS_BoundCB[nameWS] = BindPoint;
			//else  m_PS_BoundCB[nameWS] = BindPoint;
		}
		else
		{
			TA_ERROR_WS(L"Unknown Resource type in shader");
		}
	}
}

void Shader::Release()
{
	TA_SAFERELEASE(vs_blob_ptr);
	TA_SAFERELEASE(vertex_shader_ptr);
	TA_SAFERELEASE(pixel_shader_ptr);
	TA_SAFERELEASE(m_inputLayout);
}

