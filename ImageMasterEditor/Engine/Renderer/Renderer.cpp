#include "pch.h"

#include "Renderer.h"
#include "UXUI/Window.h"

#include "dxgidebug.h"
#include "dxgi1_3.h"
#include "RenderUtils.h"
 


Renderer::~Renderer()
{
	for (INT32 i = 0; i < (INT32)Shader::BlendMode::Blend_COUNT; i++)
	{
		TA_SAFERELEASE(m_Blendstates[i]);
	}

	TA_SAFERELEASE(m_CurrentStencilState);
	TA_SAFERELEASE(m_CurrentRasterState);

	TA_SAFERELEASE(m_Device_Context);
	TA_SAFERELEASE(m_Device);
}

void Renderer::SetAllCB(ID3D11VertexShader* VS, ID3D11PixelShader* PS)
{
	for (auto& CB : m_ConstantBuffers )
	{
		CB.second->BindPS(PS);
		CB.second->BindVS(VS);
	}
}

RenderTarget* Renderer::GetSmallStageTexture()
{
	if (!m_UtilRT64)
	{
		m_UtilRT64 = std::make_unique<RenderTarget>("Render_target_m_UtilRT64");
		m_UtilRT64->CreateTarget(8, 8, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, this);
		m_UtilRT64->SetKeepStagingActive(true);
	}
	return m_UtilRT64.get();
}

void Renderer::Init(IM_Math::Int2 size, Window* MainWindow)
{
	//#TODO: Get this data from the window rect
	m_CB_General.RenderBufferSize.x = m_CB_General.DisplayWindowSize.x = (float)size.x;
	m_CB_General.RenderBufferSize.y = m_CB_General.DisplayWindowSize.y = (float)size.y;
	CreateSwapChain(MainWindow);
	SetupGlobalHashDefines();
	SetupInitialConstantBuffers();
	CreateDefaultSamplers();
	CreateDefaultBlendStates();
	SetStencilState(true);
	SetupDebugAndPerf();


	{ // test setup
		
		// We assume one camera atm. Maybe later we have multi cam support
		m_ViewportCamera = std::make_unique<Camera>();;

		{
			// Set up the viewport mesh that we show the RT on. This will kind of work. But will not resize
			m_ViewportMesh = std::make_unique<Mesh>();
			Mesh::VertData* NewMesh = new  Mesh::VertData[6];
			NewMesh[0] = Mesh::VertData(0, 0, 0, 0, 0);
			NewMesh[1] = Mesh::VertData(1, 0, 0, 1, 0);
			NewMesh[2] = Mesh::VertData(1, 1, 0, 1, 1);
			NewMesh[3] = Mesh::VertData(0, 0, 0, 0, 0);
			NewMesh[5] = Mesh::VertData(0, 1, 0, 0, 1);
			NewMesh[4] = Mesh::VertData(1, 1, 0, 1, 1);
			m_ViewportMesh->SetData(this, NewMesh, 6);
			m_ViewportMesh->SetShader(L"Default");
		}

		{
			// Set up the viewport mesh that we show the RT on. This will kind of work. But will not resize
 			m_ViewportUIOVerlay = std::make_unique<Mesh>();
 			Mesh::VertData* NewMesh = new  Mesh::VertData[6];
 			NewMesh[0] = Mesh::VertData(0, 0, 0, 0, 0);
 			NewMesh[1] = Mesh::VertData(1, 0, 0, 1, 0);
 			NewMesh[2] = Mesh::VertData(1, 1, 0, 1, 1);
 			NewMesh[3] = Mesh::VertData(0, 0, 0, 0, 0);
 			NewMesh[5] = Mesh::VertData(0, 1, 0, 0, 1);
 			NewMesh[4] = Mesh::VertData(1, 1, 0, 1, 1);
 			m_ViewportUIOVerlay->SetData(this, NewMesh, 6);
 			m_ViewportUIOVerlay->SetShader(L"ViewUIOverlay");
		}

		{
			// Set up the viewport mesh that we show the RT on. This will kind of work. But will not resize
			m_BackgroundMesh = std::make_unique<Mesh>();
			Mesh::VertData* NewMesh = new  Mesh::VertData[6];
			NewMesh[0] = Mesh::VertData(0, 0, 0, 0, 0);
			NewMesh[1] = Mesh::VertData(1, 0, 0, 1, 0);
			NewMesh[2] = Mesh::VertData(1, 1, 0, 1, 1);
			NewMesh[3] = Mesh::VertData(0, 0, 0, 0, 0);
			NewMesh[5] = Mesh::VertData(0, 1, 0, 0, 1);
			NewMesh[4] = Mesh::VertData(1, 1, 0, 1, 1);
			m_BackgroundMesh->SetData(this, NewMesh, 6);
			m_BackgroundMesh->SetShader(L"Background");
			m_BackgroundMesh->SetScale(IM_Math::float3(100000, 100000, 1));
			m_BackgroundMesh->SetPosition(IM_Math::float3(-50000, -50000, -100));
		}

		
	}

}

bool Renderer::BindShader(std::wstring ShaderName)
{
	if (m_LoadedShaders.contains(ShaderName))
	{
		m_LoadedShaders[ShaderName]->Bind(this);
		CurrentBoundShader = m_LoadedShaders[ShaderName].get();
		return true;
	}
	else
	{
		m_LoadedShaders[L"Default"]->Bind(this);
		CurrentBoundShader = m_LoadedShaders[L"Default"].get();
		return false;
	}
}

bool Renderer::BindComputeShader(std::wstring ShaderName)
{
	if (m_LoadedComputeShaders.contains(ShaderName))
	{
		m_LoadedComputeShaders[ShaderName]->Bind();
		CurrentBoundComputeShader = m_LoadedComputeShaders[ShaderName].get();
		return true;
	}
	else
	{
		CurrentBoundComputeShader = nullptr;
		return false;
	}
}

void Renderer::UnbindCurrentShader()
{
	CurrentBoundShader->UnBind(this);
	CurrentBoundShader = nullptr;
}

void Renderer::UnbindCurrentComputeShader()
{
	CurrentBoundComputeShader->UnBind();
	CurrentBoundComputeShader = nullptr;
}

void Renderer::SetRenderSize(IM_Math::Int2 DrawMeshSize)
{
	m_ViewportMesh->SetScale(IM_Math::float3((float)DrawMeshSize.x, (float)DrawMeshSize.y, 1));
	m_ViewportUIOVerlay->SetScale(IM_Math::float3((float)DrawMeshSize.x, (float)DrawMeshSize.y, 1));
}

void Renderer::SetOutputRT(class RenderTarget* DisplayTexture)
{
	m_LoadedShaders[L"Default"]->SetTexture(L"DisplayTexture", DisplayTexture);
}

void Renderer::SetUIRT(class RenderTarget* UIRT)
{
	m_LoadedShaders[L"ViewUIOverlay"]->SetTexture(L"DisplayTexture", UIRT);
}
void Renderer::CreateSwapChain(Window* MainWindow)
{

	if (!m_SwapChain)
	{
		m_SwapChain = std::make_unique<SwapChain>();

		m_SwapChain->CreateSwapChain(
			IM_Math::Int2((INT32)m_CB_General.DisplayWindowSize.x, (INT32)m_CB_General.DisplayWindowSize.y),
			MainWindow,
			this,
			m_Device, 
			m_Device_Context
		);

		InternalSetRenderTarget(m_SwapChain->GetRenderTarget());
	}


}

void Renderer::DrawMesh(Mesh* meshToDraw)
{
	m_CB_PerScreenSprite.ObjectToWorld = meshToDraw->GetTransform().GetMatrix();
	m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_PerScreenSprite]->UpdateData(&m_CB_PerScreenSprite);
	m_Device_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	meshToDraw->Render(this);
}


void Renderer::Present(Window* MainWindow)
{
	m_SwapChain->Present(1, 0);
}

void Renderer::DrawViewMesh(Window* MainWindow)
{
	DrawMesh(m_BackgroundMesh.get());
	DrawMesh(m_ViewportMesh.get());
	DrawMesh(m_ViewportUIOVerlay.get());
}

std::vector<D3D_SHADER_MACRO> Renderer::GetGlobalHashDefines()
{
	return m_GlobalHashDefines;
}

void Renderer::InternalSetRenderTarget(RenderTarget* NewRT)
{
	m_ActiveRenderTarget = NewRT;
	m_ActiveRenderTarget->Bind(this);
}

void Renderer::SetupGlobalHashDefines()
{
	// Add any #defines you want

// 	{
// 		CBDefines.clear();
// 		CBDefines += CB_General_Struct::GetHeaderDefine();
// 		CBDefines += CB_PerScreenSprite_Struct::GetHeaderDefine();
// 		
// 		D3D_SHADER_MACRO Macro;
// 		Macro.Name = "GLOBALDEFINE";
// 		Macro.Definition = CBDefines.c_str();
// 		m_GlobalHashDefines.push_back(Macro);
// 
// 	}


	//Just need to do this. Who knows why, it is in the documentation
	{
		D3D_SHADER_MACRO NullTernination;
		NullTernination.Name = NullTernination.Definition = nullptr;
		m_GlobalHashDefines.push_back(NullTernination);
	}
}

void Renderer::CreateDefaultSamplers()
{

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_Samplers[RenderTypes::DefaultSamplers::Sampler_WrapPoint] = std::make_unique<SamplerState>(samplerDesc, "Sampler_WrapPoint", this);


	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_Samplers[RenderTypes::DefaultSamplers::Sampler_ClmapPoint] = std::make_unique<SamplerState>(samplerDesc, "Sampler_ClmapPoint", this);


	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_Samplers[RenderTypes::DefaultSamplers::Sampler_WrapLinear] = std::make_unique<SamplerState>(samplerDesc, "Sampler_WrapLinear", this);


	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	m_Samplers[RenderTypes::DefaultSamplers::Sampler_ClmapLinear] = std::make_unique<SamplerState>(samplerDesc, "Sampler_ClmapLinear", this);

}


void Renderer::CreateDefaultBlendStates()
{
	D3D11_BLEND_DESC BlendState = { 0 };
	//----------Default -----------
	BlendState.RenderTarget[0].BlendEnable = false;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	GetDevice()->CreateBlendState(&BlendState, &m_Blendstates[(INT32)Shader::BlendMode::Default]);
	TAUtils::SetDebugObjectName(m_Blendstates[(INT32)Shader::BlendMode::Default], "BlendMode::Default");


	//---------- AlphaBlend -----------
	BlendState.RenderTarget[0].BlendEnable = true;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	GetDevice()->CreateBlendState(&BlendState, &m_Blendstates[(INT32)Shader::BlendMode::AlphaBlend]);
	TAUtils::SetDebugObjectName(m_Blendstates[(INT32)Shader::BlendMode::AlphaBlend], "BlendMode::AlphaBlend");


	//------- Addetive --------------
	BlendState.RenderTarget[0].BlendEnable = true;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	GetDevice()->CreateBlendState(&BlendState, &m_Blendstates[(INT32)Shader::BlendMode::Addetive]);
	TAUtils::SetDebugObjectName(m_Blendstates[(INT32)Shader::BlendMode::Addetive], "BlendMode::Addetive");


	//------- AlphaMask --------------
	BlendState.RenderTarget[0].BlendEnable = true;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	GetDevice()->CreateBlendState(&BlendState, &m_Blendstates[(INT32)Shader::BlendMode::AlphaMask]);
	TAUtils::SetDebugObjectName(m_Blendstates[(INT32)Shader::BlendMode::AlphaMask], "BlendMode::AlphaMask");
}


void Renderer::SetStencilState(bool EnableDepthTesting)
{
	if (EnableDepthTesting == m_EnableDepthTesting) return;
	m_EnableDepthTesting = EnableDepthTesting;
	////////// ------------- Create State ----------/////

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };


	// Depth test parameters
	dsDesc.DepthEnable = m_EnableDepthTesting;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state

	TA_SAFERELEASE(m_CurrentStencilState);
	GetDevice()->CreateDepthStencilState(&dsDesc, &m_CurrentStencilState);
	GetDeviceContext()->OMSetDepthStencilState(m_CurrentStencilState, 1);
	TAUtils::SetDebugObjectName(m_CurrentStencilState, "m_CurrentStencilState");

}

void Renderer::SetRasterizer(bool Wireframe, bool backfaceCulling)
{
	TA_SAFERELEASE(m_CurrentRasterState);
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.DepthClipEnable = true;
	wfdesc.FillMode = Wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	wfdesc.CullMode = backfaceCulling ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	wfdesc.MultisampleEnable = true;// Defined bt RT
	GetDevice()->CreateRasterizerState(&wfdesc, &m_CurrentRasterState);
	GetDeviceContext()->RSSetState(m_CurrentRasterState);
	TAUtils::SetDebugObjectName(m_CurrentRasterState, "m_CurrentRasterState");

}

void Renderer::SetBlendState(Shader::BlendMode NewBlendMode)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	GetDeviceContext()->OMSetBlendState(m_Blendstates[(INT32)NewBlendMode], blendFactor, sampleMask);
}

D3D11_VIEWPORT Renderer::CreateViewport()
{
	D3D11_VIEWPORT NewVP = { 0 };
	NewVP.Width = (float)m_CB_General.DisplayWindowSize.x;
	NewVP.Height = (float)m_CB_General.DisplayWindowSize.y;
	NewVP.MinDepth = 0.0f;
	NewVP.MaxDepth = 1.0f;
	NewVP.TopLeftX = 0;
	NewVP.TopLeftY = 0;
	return NewVP;
}

void Renderer::CheckWindowSize(Window* MainWindow)
{
	// Would be better to do this through a callback. But this is simple and for now not that many cpu cycles;

	RECT rect;
	if (!MainWindow->GetTheWindowRect(&rect))
	{
		return;
	}

	int NewWidth = rect.right - rect.left;
	int NewHeight = rect.bottom - rect.top;

	if (m_CB_General.DisplayWindowSize.x == (float)NewWidth &&
		m_CB_General.DisplayWindowSize.y == (float)NewHeight
		)
	{
		return;
	}

	m_Device_Context->OMSetRenderTargets(0, 0, 0);

	m_SwapChain->ResizeToWindow(this);

	m_CB_General.DisplayWindowSize.x = (float)NewWidth;
	m_CB_General.DisplayWindowSize.y = (float)NewHeight;

	m_CB_General.RenderBufferSize.x = (float)m_SwapChain->GetSize().x;
	m_CB_General.RenderBufferSize.y = (float)m_SwapChain->GetSize().y;

	// Set up the viewport.
	D3D11_VIEWPORT vp = CreateViewport();
	m_Device_Context->RSSetViewports(1, &vp);
	TA_WARN_WS(L"Resized window");
}

void Renderer::SetupDebugAndPerf()
{
	HRESULT hr = GetDeviceContext()->QueryInterface(__uuidof(m_PerfObject), reinterpret_cast<void**>(&m_PerfObject));
}

ComputeShader* Renderer::GetComputeShader(std::wstring ShaderName)
{
	if (m_LoadedComputeShaders.contains(ShaderName))
	{
		return m_LoadedComputeShaders[ShaderName].get();
	}
	return nullptr;
}

void Renderer::RefreshShaders(std::vector<std::wstring> FoundShaders, std::vector<std::wstring> FoundComputeShaders)
{
	{
		{// delete removed shaders

			std::set<std::wstring> CurrentShaders;
			for (auto& shader : m_LoadedShaders)
			{
				CurrentShaders.insert(shader.first);
			}

			for (auto& Exisitingshader : FoundShaders)
			{
				std::filesystem::path p(Exisitingshader);
				std::wstring fileName = p.stem();
				CurrentShaders.erase(fileName);
			}

			for (auto& ShaderToDelete : CurrentShaders)
			{
				m_LoadedShaders.erase(ShaderToDelete);
			}
		}

		for (std::wstring& ShaderToProcess : FoundShaders)
		{
			std::filesystem::path p(ShaderToProcess.c_str());
			std::wstring fileName = p.stem();
			if (!m_LoadedShaders.contains(fileName) || m_LoadedShaders[fileName] == nullptr)
			{
				m_LoadedShaders[fileName] = std::make_unique<Shader>(TAUtils::WStringToChar(fileName.c_str()));
			}
			m_LoadedShaders[fileName]->SetShaderPath(ShaderToProcess);
			m_LoadedShaders[fileName]->LoadReload(this);
		}
	}

	{
		{// delete removed shaders

			std::set<std::wstring> CurrentShaders;
			for (auto& shader : m_LoadedComputeShaders)
			{
				CurrentShaders.insert(shader.first);
			}

			for (auto& Exisitingshader : FoundComputeShaders)
			{
				std::filesystem::path p(Exisitingshader);
				std::wstring fileName = p.stem();
				CurrentShaders.erase(fileName);
			}

			for (auto& ShaderToDelete : CurrentShaders)
			{
				m_LoadedComputeShaders.erase(ShaderToDelete);
			}
		}

		for (std::wstring& ShaderToProcess : FoundComputeShaders)
		{
			std::filesystem::path p(ShaderToProcess.c_str());
			std::wstring fileName = p.stem(); 

			std::wstring fileName_Prefix = fileName.substr(0,5);
			if (fileName_Prefix == L"Blend" || fileName_Prefix == L"Brush" || fileName_Prefix == L"Utill")
			{
				if (!m_LoadedComputeShaders.contains(fileName) || m_LoadedComputeShaders[fileName] == nullptr)
				{
					m_LoadedComputeShaders[fileName] = std::make_unique<ComputeShader>("ComputeShader::fileName" + TAUtils::WStringToChar(fileName.c_str()), this);
				}
				m_LoadedComputeShaders[fileName]->SetShaderPath(ShaderToProcess);
				m_LoadedComputeShaders[fileName]->LoadReload(GetDevice());
			}
		}
	}

}

void Renderer::SetupInitialConstantBuffers()
{
	D3D11_BUFFER_DESC constantBufferDesc;

	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_PerScreenSprite_Struct);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;


	// General 
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_General_Struct);
	m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_General] = std::make_unique<ConstantBuffer>(constantBufferDesc, "CB_General", this, (UINT)RenderTypes::ConstanBuffer::CB_General);
	GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_General]->Bind_CB_to_CS();

	// Sprite??????
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_PerScreenSprite_Struct);
	m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_PerScreenSprite] = std::make_unique<ConstantBuffer>(constantBufferDesc, "CB_PerScreenSprite", this, (UINT)RenderTypes::ConstanBuffer::CB_PerScreenSprite);
	GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_PerScreenSprite]->Bind_CB_to_CS();

	// Brush
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_BrushInput_Struct);
	m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_BrushInput] = std::make_unique<ConstantBuffer>(constantBufferDesc, "CB_BrushInput", this, (UINT)RenderTypes::ConstanBuffer::CB_BrushInput);
	GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_BrushInput]->Bind_CB_to_CS();

	// Brush
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_Layer_Struct);
	m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_Layer] = std::make_unique<ConstantBuffer>(constantBufferDesc, "CB_Layer", this, (UINT)RenderTypes::ConstanBuffer::CB_Layer);
	GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_Layer]->Bind_CB_to_CS();

}

void Renderer::UpdateCamera(CameraData CamData)
{
	if (!m_ViewportCamera) { return; }

	m_ViewportCamera->UpdateCamera(CamData);
}

void Renderer::ReadyNextFrame(Window* window)
{
	RenderUtils::ScopedProfile Scope(this, std::wstring(L"ReadyNextFrame"));

	m_ActiveRenderTarget->Bind(this);

	CheckWindowSize(window);
	m_SwapChain->GetRenderTarget()->Clear(m_CB_General.BackgroundColor.x, m_CB_General.BackgroundColor.y, m_CB_General.BackgroundColor.z, m_CB_General.BackgroundColor.w,this);

	///////////////////////////////////////////////// /
	// Update CBs
	//////////////////////////////////////////////////

	// General params --------------------------------
	{
		//Mouse
		m_CB_General.MousePos.x = (float)window->GetMouseX();
		m_CB_General.MousePos.y = (float)window->GetMouseY();
		m_CB_General.ViewMatrix = m_ViewportCamera->GetViewMatrix();
		
		m_ViewportCamera->SetCameraSizeX((float)m_CB_General.DisplayWindowSize.x);
		m_ViewportCamera->SetCameraSizeY((float)m_CB_General.DisplayWindowSize.y);

		m_CB_General.ProjectionMatrix = m_ViewportCamera->GetProjectionMatrix();

		m_ConstantBuffers[RenderTypes::ConstanBuffer::CB_General]->UpdateData(&m_CB_General);

	}


	/////////////////////// Viewport /////////////////////////
	{
		RECT winRect;
		window->GetTheWindowRect(&winRect);

		D3D11_VIEWPORT viewport = CreateViewport();
		m_Device_Context->RSSetViewports(1, &viewport);
		m_ActiveRenderTarget->Bind(this);
	}
}
