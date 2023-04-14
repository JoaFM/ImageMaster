#pragma once

#include "MainApp/Default_Headers.h"

class Texture 
{
public:
	Texture();
	~Texture() { Release(); }


	struct ID3D11ShaderResourceView* GetSRV() { return m_SRV; }

protected:
	bool LoadedAndValid = false;
	struct ID3D11ShaderResourceView* m_SRV = nullptr;
	struct ID3D11Texture2D* m_textureBuffer = nullptr;
	void Release();

private:

};

