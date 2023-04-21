#pragma once
#include "MainApp/Default_Headers.h"


class RenderTypes
{
public:
	enum class ConstanBuffer
	{
		CB_General,
		CB_PerScreenSprite,
		NumConstantBuffers
	};

	enum class DefaultSamplers {
		Sampler_WrapPoint,
		Sampler_WrapLinear,
		Sampler_ClmapPoint,
		Sampler_ClmapLinear,
		Sampler_COUNT
	};
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
	struct CB_PerScreenSprite_Struct
	{
		DirectX::XMMATRIX ObjectToWorld;
		DirectX::XMFLOAT4 ForegroundColor;

		static std::string GetHeaderDefine()
		{

			std::string ResultStr;
			ResultStr += "cbuffer CB_PerScreenSprite_Struct : register(b1)\n";
			ResultStr += "{\n";
			ResultStr += "	float4x4 ObjectToWorld;\n";
			ResultStr += "	float4 ForegroundColor;\n";
			ResultStr += "}\n";
			ResultStr += "\n";
			return ResultStr;
		}
	};

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetStandardVertLayout()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_StandardVertLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// /*
		// { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//  { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 

		};
		return m_StandardVertLayout;

	}
// 	 static  D3D11_INPUT_ELEMENT_DESC m_StandardVertLayout[2] = {
// 		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 
// 		// /*
// 		// { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 		// { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 		//  { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 		// 
// 
// 	};
};