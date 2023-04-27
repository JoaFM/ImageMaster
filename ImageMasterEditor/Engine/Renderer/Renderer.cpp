


#include "Renderer.h"
#include "UXUI/Window.h"


void Renderer::Init(IM_Math::Int2 size, Window* MainWindow)
{
	//#TODO: Get this data from the window rect
	m_CB_General.RenderBufferSize.x = m_CB_General.DisplayWindowSize.x = (float)size.x;
	m_CB_General.RenderBufferSize.y = m_CB_General.DisplayWindowSize.y = (float)size.y;
	CreateSwapChain(MainWindow);
	SetupGlobalHashDefines();
	SetupGeneral_CB();
	CreateDefaultSamplers();
	CreateDefaultBlendStates();
	SetStencilState(true);


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
		}

		
		m_ViewportMesh->SetShader(L"Default");
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
		m_LoadedComputeShaders[ShaderName]->Bind(GetDeviceContext());
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
	CurrentBoundComputeShader->UnBind(GetDeviceContext());
	CurrentBoundComputeShader = nullptr;
}

void Renderer::SetRenderSize(IM_Math::Int2 DrawMeshSize)
{
	m_ViewportMesh->SetScale(IM_Math::float3((float)DrawMeshSize.x, (float)DrawMeshSize.y, 1));
}

void Renderer::SetOutputRT(class RenderTarget* DisplayTexture)
{
	m_LoadedShaders[L"Default"]->SetTexture(L"DisplayTexture", DisplayTexture);
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
	m_Device_Context->UpdateSubresource(m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_PerScreenSprite], 0, nullptr, &m_CB_PerScreenSprite, 0, 0);
	m_Device_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	meshToDraw->Render(this);
}


void Renderer::Present(Window* MainWindow)
{
	DrawMesh(m_ViewportMesh.get());
	m_SwapChain->Present(1, 0);
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
	TA_HRCHECK_Simple(
		GetDevice()->CreateSamplerState(&samplerDesc, &m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_WrapPoint]),
		L"failed to make Sampler_WrapPoint"
	);

	TAUtils::SetDebugObjectName(m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_WrapPoint], "Sampler_WrapPoint");

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	TA_HRCHECK_Simple(
		GetDevice()->CreateSamplerState(&samplerDesc, &m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_ClmapPoint]),
		L"failed to make Sampler_WrapPoint"
	);

	TAUtils::SetDebugObjectName(m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_ClmapPoint], "Sampler_ClmapPoint");

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	TA_HRCHECK_Simple(
		GetDevice()->CreateSamplerState(&samplerDesc, &m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_WrapLinear]),
		L"failed to make Sampler_WrapPoint"
	);
	TAUtils::SetDebugObjectName(m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_WrapLinear], "Sampler_WrapLinear");

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	TA_HRCHECK_Simple(
		GetDevice()->CreateSamplerState(&samplerDesc, &m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_ClmapLinear]),
		L"failed to make Sampler_WrapPoint"
	);
	TAUtils::SetDebugObjectName(m_Samplers[(INT32)RenderTypes::DefaultSamplers::Sampler_ClmapLinear], "Sampler_ClmapLinear");

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
				m_LoadedShaders[fileName] = std::make_unique<Shader>();
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
			if (!m_LoadedComputeShaders.contains(fileName) || m_LoadedComputeShaders[fileName] == nullptr)
			{
				m_LoadedComputeShaders[fileName] = std::make_unique<ComputeShader>();
			}
			m_LoadedComputeShaders[fileName]->SetShaderPath(ShaderToProcess);
			m_LoadedComputeShaders[fileName]->LoadReload(GetDevice());
		}
	}

	

}

void Renderer::SetupGeneral_CB()
{
	D3D11_BUFFER_DESC constantBufferDesc;

	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_General_Struct);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	TA_HRCHECK_Simple(
		m_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_General]),
		L"failed to make constant buffer : CB_General"
	);
	TAUtils::SetDebugObjectName(m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_General], "ConstanBuffer::CB_General");

	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(RenderTypes::CB_PerScreenSprite_Struct);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	TA_HRCHECK_Simple(
		m_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_PerScreenSprite]),
		L"failed to make constant buffer : CB_PerScreenSprite"
	);
	TAUtils::SetDebugObjectName(m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_PerScreenSprite], "ConstanBuffer::CB_PerScreenSprite");

}

void Renderer::UpdateCamera(CameraData CamData)
{
	if (!m_ViewportCamera) { return; }

	m_ViewportCamera->UpdateCamera(CamData);
}

void Renderer::ReadyNextFrame(Window* window)
{
	// 	if (m_testCompute->Bind(GetDeviceContext()))
// 	{
// 		m_testCompute->Dispatch(GetDeviceContext(), 16, 16, 1);
// 		m_testCompute->UnBind(GetDeviceContext());
// 	}

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

		m_Device_Context->UpdateSubresource(m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::CB_General], 0, nullptr, &m_CB_General, 0, 0);

	}

	/////////////////////// Viewport /////////////////////////
	{
		RECT winRect;
		window->GetTheWindowRect(&winRect);

		D3D11_VIEWPORT viewport = CreateViewport();
		m_Device_Context->RSSetViewports(1, &viewport);
		m_ActiveRenderTarget->Bind(this);
		//m_Device_Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	}
}
