#pragma once
#include <windows.h>
#include <intsafe.h>
#include <d3d11.h>       // D3D interface

#include <iostream>
#include <vector>
#include <map>
#include "Utils/IM_STD.h"

//ID3D11ShaderReflection get info from shader	
/*
ID3D11ShaderReflection* reflectionInterface;
D3DReflect(bytecode, bytecodeLength, IID_ID3D11ShaderReflection, (void**)&reflectionInterface);
D3D11_SHADER_INPUT_BIND_DESC bindDesc;
reflectionInterface->GetResourceBindingDescByName("textureMap", &bindDesc);


//---   bindDesc.BindPoint is the index of the slot the texture is bound to.

// looks really good
https://cpp.hotexamples.com/examples/-/ID3D11ShaderReflection/-/cpp-id3d11shaderreflection-class-examples.html

*/
class Shader 
{
public:

	enum class ShadardShaders {
		Shader_Default,
		Shader_COUNT
	};

	enum class BlendMode {
		Default,
		AlphaMask,
		AlphaBlend,
		Addetive,
		Blend_COUNT
	};

	Shader();
	~Shader() { Release(); };
	// Data
	ID3D11InputLayout* GetVertexLayout() { return m_inputLayout; }
	ID3D11VertexShader* GetVertexShader() { return vertex_shader_ptr; }
	ID3D11PixelShader* GetPixelShader() { return pixel_shader_ptr; }

	// textures
	void SetTexture(std::wstring Name, class Texture* NewTexture);
	
	//Buffers
	void SetBuffer(std::wstring Name, class Buffer* NewBuffer) ;

	//Util
	void LoadReload(class Renderer* renderer);
	void Bind(class Renderer* renderer);
	void UnBind(Renderer* renderer);
	bool IsValid() {return  m_LoadedAndValid; }
	
	void SetShaderPath(std::wstring Newpath) { m_ShaderPath = Newpath; };
	const std::wstring GetShaderPath() { return m_ShaderPath; }


private:
	std::wstring  m_ShaderPath;
	bool m_LoadedAndValid = false;
	bool m_Wireframe = false;
	bool m_BackfaceCulling = true;
	bool m_EnableDepthTesting = true;

	// Data
	ID3D11InputLayout* m_inputLayout = nullptr;
	ID3D11VertexShader* vertex_shader_ptr = nullptr;
	ID3DBlob* vs_blob_ptr = nullptr;
	ID3DBlob* ps_blob_ptr = nullptr;
	ID3D11PixelShader* pixel_shader_ptr = nullptr;


	// Defined Params
	BlendMode m_BlendMode = BlendMode::Default;

	// for cleanup
	std::vector<UINT> m_SRVSlotsBoundInPS;
	std::vector<UINT> m_ShaderSlotsBoundedPS;

	// Data given to shader to look at
	std::map <std::wstring, class Texture*>	m_App_BoundTextures;
	std::map <std::wstring, class Buffer*>	m_App_BoundBuffers;
	
	// Reflection of shader
	std::map <std::wstring, UINT> m_PS_BoundTextures;
	std::map <std::wstring, UINT> m_PS_BoundBuffers;
	std::map <std::wstring, UINT> m_VS_BoundBuffers;

	std::map <std::wstring, UINT> m_PS_BoundSamplers;
	void CalcRelection(ID3DBlob* Shader_blob_ptr);


// #TODO have better  
// 
//	std::map <std::wstring, UINT> m_PS_BoundCB;
// 	std::map <std::wstring, UINT> m_VS_BoundTextures;
//-	std::map <std::wstring, UINT> m_VS_BoundBuffers;
// 	std::map <std::wstring, UINT> m_VS_BoundSamplers;
// 	std::map <std::wstring, UINT> m_VS_BoundCB;

	void Release();
	void FindDefines(std::string& FileDataString);
};

