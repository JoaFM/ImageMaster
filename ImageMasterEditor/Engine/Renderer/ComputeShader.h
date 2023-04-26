#pragma once

#include "MainApp/Default_Headers.h"

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();

	void SetShaderPath(std::wstring Newpath) { m_ShaderPath = Newpath; };
	const std::wstring GetShaderPath() { return m_ShaderPath; }
	void LoadReload(ID3D11Device* Device);

	void SetBuffer(class Buffer* NewTexture) { m_Buffer = NewTexture; };
	void SetRT(class RenderTarget* NewTexture) { m_RO0 = NewTexture; };

	class Buffer* GetBuffer() { return m_Buffer; }
	class RenderTarget* GetRT() { return m_RO0; }

	bool IsValid() { return LoadedAndValid; }
	
	void Dispatch(ID3D11DeviceContext* DeviceContext, INT32 ThreadsX, INT32 ThreadsY, INT32 ThreadsZ);
	void Dispatch(ID3D11DeviceContext* DeviceContext);
	
	
	bool Bind(ID3D11DeviceContext* DeviceContext);
	bool UnBind(ID3D11DeviceContext* DeviceContext);
	void Release();
private:
	std::wstring  m_ShaderPath;
	class Buffer* m_Buffer = nullptr;
	class RenderTarget* m_RO0 = nullptr;


	bool LoadedAndValid = false;
	struct ID3D11ComputeShader* m_ComputeShader = nullptr;
};
