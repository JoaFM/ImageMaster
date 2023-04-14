#include "Texture.h"

Texture::Texture()
{

}


void Texture::Release()
{
	TA_SAFERELEASE(m_SRV);
}

