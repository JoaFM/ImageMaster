#pragma once

#include "pch.h"
#include "RenderTypes.h"
#include "../IMBase.h"
#include "Texture.h"

class Texture2D : public Texture
{
public:
	Texture2D(class Renderer* renderer);
	void LoadReload();
	void SetTexturePath(std::string TexturePath);
private:
	Renderer* m_Renderer = nullptr;
	std::string m_TexturePath;
};

