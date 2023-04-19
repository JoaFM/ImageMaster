#include "ComputeShader.h"
#include "Utils/IM_STD.h"
#include "Buffer.h"
#include "RenderTarget.h"
#include "D3DCompiler.h"

ComputeShader::ComputeShader()
{
}

ComputeShader::~ComputeShader()
{
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


	HRESULT hr = D3DCompileFromFile(TAUtils::Paths::instance().ABS_Path(m_ShaderPath).c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
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
	TA_SAFERELEASE(shaderBlob);
	LoadedAndValid = true;
	//OnNameOpdate();
}

void ComputeShader::Dispatch(ID3D11DeviceContext* DeviceContext,  INT32 ThreadsX, INT32 ThreadsY, INT32 ThreadsZ)
{
	Bind(DeviceContext);
	DeviceContext->Dispatch(ThreadsX, ThreadsY, ThreadsZ);
	UnBind(DeviceContext);
}


bool ComputeShader::Bind(ID3D11DeviceContext* DeviceContext)
{

	if (!LoadedAndValid)
	{
		TA_ERROR_WS(L"Compute Shader not Valid cannot run, Not Properly loaded");
		return false;
	}

	if (!m_Buffer && !m_RO0)
	{
		TA_ERROR_WS(L"No RT or buffer loaded");
		return false;
	}

	ID3D11UnorderedAccessView* UAV = nullptr;
	if (m_Buffer)
	{
		 UAV = m_Buffer->GetUAV();
	}
	else
	{
		UAV = m_RO0->GetUAV();
	}


	DeviceContext->CSSetShader(m_ComputeShader, NULL, 0);
	DeviceContext->CSSetUnorderedAccessViews(0, 1, &UAV, 0);


	return true;
}

bool ComputeShader::UnBind(ID3D11DeviceContext* DeviceContext)
{
	ID3D11UnorderedAccessView* nullSRV = { NULL };
	DeviceContext->CSSetUnorderedAccessViews(0, 1, &nullSRV, 0);
	DeviceContext->CSSetShader(nullptr, nullptr, 0);


	return true;
}