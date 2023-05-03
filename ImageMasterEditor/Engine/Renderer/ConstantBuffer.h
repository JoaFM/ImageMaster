#pragma once

#include "../../MainApp/Default_Headers.h"
#include <d3d11.h>

class ConstantBuffer 
{
public:
	ConstantBuffer(D3D11_BUFFER_DESC Desk, std::string FriendlyName, class Renderer* renderer, UINT slot);
	~ConstantBuffer();
	void UpdateData(const void* DataToUpdate );
	void CSBind();

	ID3D11Buffer* Get_CB_Object() { return m_CB_Object; };
private:
	ID3D11Buffer* m_CB_Object = nullptr;
	UINT m_slot = 0;
	class Renderer* m_Renderer = nullptr;
public:
	void BindPS(struct ID3D11PixelShader* PS);
	void BindVS(struct ID3D11VertexShader* VS) ;
};