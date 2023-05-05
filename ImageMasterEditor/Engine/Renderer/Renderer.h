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

#include "External/IMGUI/imgui.h"
#include "External/IMGUI/imgui_impl_dx11.h"
#include "External/IMGUI/imgui_impl_win32.h"
#include "Buffer.h"
#include "ComputeShader.h"
#include "Shader.h"
#include "RenderTypes.h"
#include "Mesh.h"
#include "Camera.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
class Renderer
{


public:
//management
	void Init(IM_Math::Int2 size, class Window* MainWindow);

	bool BindShader(std::wstring ShaderName);
	bool BindComputeShader(std::wstring ShaderName);
	void UnbindCurrentShader();
	void UnbindCurrentComputeShader();
	void SetRenderSize(IM_Math::Int2 DrawMeshSize);
	~Renderer();

	void SetAllCB(ID3D11VertexShader* VS, ID3D11PixelShader* PS);
public:

	//Internal for rendering
	ID3D11Device* GetDevice() { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_Device_Context; }

	// Loop
	void Present(Window* MainWindow);
	void DrawViewMesh(Window* MainWindow);
	void ReadyNextFrame(Window* window);


	//States
	void SetBlendState(Shader::BlendMode NewBlendMode);
	void SetRasterizer(bool Wireframe, bool backfaceCulling);
	void SetStencilState(bool EnableDepthTesting);

	std::vector<D3D_SHADER_MACRO> GetGlobalHashDefines();
	auto& GetConstantBuffers() { return m_ConstantBuffers; }
	SamplerState* GetSampler(RenderTypes::DefaultSamplers SamplerName) { return m_Samplers[SamplerName].get(); }

	//Do things
	void DrawMesh(Mesh* meshToDraw);
	void SetOutputRT(class RenderTarget* OutputRTParam);
	void UpdateCamera(CameraData CamData);

private:
	// Render objects
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Device_Context = nullptr;
	std::unique_ptr<SwapChain> m_SwapChain = nullptr;

	// Buffers
	RenderTypes::CB_General_Struct m_CB_General;
	RenderTypes::CB_PerScreenSprite_Struct m_CB_PerScreenSprite;
	//ID3D11Buffer* m_ConstantBuffers[(UINT)RenderTypes::ConstanBuffer::NumConstantBuffers];
	std::map<RenderTypes::ConstanBuffer, std::unique_ptr<ConstantBuffer>>  m_ConstantBuffers;

	//struct ID3D11SamplerState* m_Samplers[(UINT32)RenderTypes::DefaultSamplers::Sampler_COUNT];
	std::map<RenderTypes::DefaultSamplers, std::unique_ptr<SamplerState>>  m_Samplers;

	ID3D11BlendState* m_Blendstates[(UINT32)Shader::BlendMode::Blend_COUNT];

	
	// STATE
	ID3D11DepthStencilState* m_CurrentStencilState = nullptr;
	ID3D11RasterizerState* m_CurrentRasterState = nullptr;
	bool m_EnableDepthTesting = false;

	
	std::unique_ptr<Camera> m_ViewportCamera = nullptr;
	RenderTarget* m_ActiveRenderTarget = nullptr;
	std::unique_ptr<Mesh> m_ViewportMesh = nullptr;
	std::unique_ptr<Mesh> m_BackgroundMesh = nullptr;

	//------- Startup ----------
	void InternalSetRenderTarget(RenderTarget* RTarget);
	void CreateSwapChain(Window* MainWindow);
	void SetupGlobalHashDefines();
	void CreateDefaultSamplers();
	void CreateDefaultBlendStates();
	void SetupGeneral_CB();


	D3D11_VIEWPORT CreateViewport();
	void CheckWindowSize(Window* MainWindow);

	// Data

	std::vector<D3D_SHADER_MACRO> m_GlobalHashDefines;
	std::map<std::wstring, std::unique_ptr<Shader>> m_LoadedShaders;
	std::map<std::wstring, std::unique_ptr<ComputeShader>> m_LoadedComputeShaders;
	Shader* CurrentBoundShader = nullptr;
	ComputeShader* CurrentBoundComputeShader = nullptr;
public:
	std::map<std::wstring, std::unique_ptr<ComputeShader>>& GetComputeShaders() { return m_LoadedComputeShaders; };
	void RefreshShaders(std::vector<std::wstring> FoundShaders, std::vector<std::wstring> FoundComputeShaders);
	ComputeShader* GetCurrentComputeShader() const {return CurrentBoundComputeShader; }
};