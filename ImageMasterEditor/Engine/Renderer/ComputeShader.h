#pragma once

#include "pch.h"
#include "../IMBase.h"
#include "ConstantBufferManager.h"

class ComputeShader : IMBase
{
public:
	ComputeShader(std::string Name, class Renderer* renderer);
	~ComputeShader();

	void SetShaderPath(std::wstring Newpath) { m_ShaderPath = Newpath; };
	const std::wstring GetShaderPath() { return m_ShaderPath; }
	void LoadReload(ID3D11Device* Device);

	void SetBuffer(class Buffer* NewTexture) { m_PropertiesStructuredBuffer = NewTexture; };
	void SetTexture(std::string Name, class Texture* NewTexture);

	class Buffer* GetBuffer() { return m_PropertiesStructuredBuffer; }

	bool IsValid() { return LoadedAndValid; }
	
	void Dispatch(INT32 ThreadsX, INT32 ThreadsY, INT32 ThreadsZ);
	void Dispatch();
	void Dispatch( MaterialInstance& Params);
	
	
	bool BindTExturesFromParam(MaterialInstance& Params);
	bool Bind();
	bool UnBind();
	void Release();

private:
	std::wstring  m_ShaderPath;
	class Buffer* m_PropertiesStructuredBuffer = nullptr;


	bool LoadedAndValid = false;
	struct ID3D11ComputeShader* m_ComputeShader = nullptr;
	class Renderer* m_Renderer = nullptr;
private:

	void CalcRelection(ID3DBlob* Shader_blob_ptr);
	std::map <std::string, UINT> m_TextureRW_BindPoints;
	std::map <std::string, UINT> m_CBuffer_BindPoints;
	std::map <std::string, UINT> m_Texture_BindPoints;
	std::map <std::string, class Texture*>	m_App_BoundTextures;

	
	//Cleanup
	std::vector<UINT> m_ShaderBound_UAV;
	std::vector<UINT> m_ShaderBound_SRV;

	std::unique_ptr<ConstantBufferManager> GlobalsCBManager;

};
