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

class Renderer
{

	struct CB_General_Struct
	{
		DirectX::XMFLOAT2 MousePos;
		float deltaTime;
		float Time;
		DirectX::XMFLOAT2 DisplayWindowSize;
		DirectX::XMFLOAT2 RenderBufferSize;

		/// <summary>
		/// Shape the verts to fit the camera. Camera to clip space
		/// </summary>
		DirectX::XMMATRIX ProjectionMatrix;

		/// <summary>
		/// The cameras offset to the world. The camera is still but we offset the world.
		/// </summary>
		DirectX::XMMATRIX ViewMatrix;
		DirectX::XMFLOAT4 BackgroundColor;
		DirectX::XMFLOAT2 PADDING;

		CB_General_Struct()
		{
			MousePos.x = 0;
			MousePos.y = 0;
			deltaTime = 1;
			Time = 0;
			DisplayWindowSize.x = 1280;
			DisplayWindowSize.y = 720;
			RenderBufferSize.x = 1280;
			RenderBufferSize.y = 720;
			ProjectionMatrix = DirectX::XMMatrixIdentity();
			ViewMatrix = DirectX::XMMatrixIdentity();
			BackgroundColor = DirectX::XMFLOAT4(.1f, .2f, .3f, 1.0f);
			PADDING = DirectX::XMFLOAT2();
		}

		static std::string GetHeaderDefine()
		{
			std::string ResultStr;

			ResultStr += "cbuffer CB_General_Struct : register( b0 )\n";
			ResultStr += "{\n";
			ResultStr += "	float2 MousePos;\n";
			ResultStr += "	float DeltaTime;\n";
			ResultStr += "	float Time;\n";
			ResultStr += "	float2 DisplayWindowSize;\n";
			ResultStr += "	float2 RenderBufferSize;\n";
			ResultStr += "	float4x4 ProjectionMatrix;\n";
			ResultStr += "	float4x4 ViewMatrix;\n";
			ResultStr += "	float4 BackgroundColor;\n";
			ResultStr += "	float2 PADDING;\n";
			ResultStr += "}\n";
			ResultStr += "\n";

			return ResultStr;

		}
	};

public:
//management
	void Init(IM_Math::Int2 size, Window* MainWindow);

public:
	//Internal for rendering
	ID3D11Device* GetDevice() { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_Device_Context; }
	void Tick();
private:
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Device_Context = nullptr;
	std::unique_ptr<SwapChain> m_SwapChain = nullptr;

	// Buffers
	CB_General_Struct m_CB_General;

	// Test compute
	std::unique_ptr<Buffer> m_testBuffer = nullptr;
	std::unique_ptr<RenderTarget> m_testRT = nullptr;
	std::unique_ptr<ComputeShader> m_testCompute = nullptr;



	RenderTarget* ActiveRenderTarget = nullptr;

	void InternalSetRenderTarget(RenderTarget* RTarget);
	void CreateSwapChain(Window* MainWindow);

};