#pragma once

#include "MainApp/Default_Headers.h"

#include <windows.h>
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler
#include <assert.h>
#include <DirectXMath.h>
#include <d3d11_1.h>


#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // direct x graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

#include "SwapChain.h"
#include "RenderTarget.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_dx11.h"
#include "IMGUI/imgui_impl_win32.h"
#include "Buffer.h"
#include "ComputeShader.h"
#include "Shader.h"
#include "RenderTypes.h"
#include "Mesh.h"
#include "Camera.h"
class Renderer
{
	


public:
//management
	void Init(IM_Math::Int2 size, class Window* MainWindow);

public:

public:

	//Internal for rendering
	ID3D11Device* GetDevice() { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_Device_Context; }
	void Tick(Window* MainWindow);


	std::vector<D3D_SHADER_MACRO> GetGlobalHashDefines();
	auto GetConstantBuffers() { return m_ConstantBuffers; }

	struct ID3D11SamplerState* GetSampler(RenderTypes::DefaultSamplers SamplerName) { return m_Samplers[(UINT32)SamplerName]; }

	//States
	void SetBlendState(Shader::BlendMode NewBlendMode);
	void SetRasterizer(bool Wireframe, bool backfaceCulling);
	void SetStencilState(bool EnableDepthTesting);

	//Do things
	void DrawMesh(Mesh* meshToDraw);

private:
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Device_Context = nullptr;
	std::unique_ptr<SwapChain> m_SwapChain = nullptr;

	// Buffers
	RenderTypes::CB_General_Struct m_CB_General;
	RenderTypes::CB_PerScreenSprite_Struct m_CB_PerScreenSprite;
	ID3D11Buffer* m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::NumConstantBuffers];
	
	struct ID3D11SamplerState* m_Samplers[(UINT32)RenderTypes::DefaultSamplers::Sampler_COUNT];
	ID3D11BlendState* m_Blendstates[(UINT32)Shader::BlendMode::Blend_COUNT];

	// Test compute
	std::unique_ptr<Buffer> m_testBuffer = nullptr;
	std::unique_ptr<RenderTarget> m_testRT = nullptr;
	std::unique_ptr<ComputeShader> m_testCompute = nullptr;
	std::unique_ptr<Mesh> m_Testmesh = nullptr;
	std::unique_ptr<Shader> m_TestShader = nullptr;
	std::unique_ptr<Camera> m_TestCamera = nullptr;
	
	
	// STATE
	ID3D11DepthStencilState* m_CurrentStencilState = nullptr;
	ID3D11RasterizerState* m_CurrentRasterState = nullptr;
	RenderTarget* ActiveRenderTarget = nullptr;
	bool m_EnableDepthTesting = false;

	void InternalSetRenderTarget(RenderTarget* RTarget);
	void CreateSwapChain(Window* MainWindow);
	void SetupGlobalHashDefines();
	void CreateDefaultSamplers();
	void CreateDefaultBlendStates();
	D3D11_VIEWPORT CreateViewport();
	void CheckWindowSize(Window* MainWindow);

	// Data

	std::vector<D3D_SHADER_MACRO> m_GlobalHashDefines;

	void SetupGeneral_CB();
	void ReadyNextFrame(Window* window);
};