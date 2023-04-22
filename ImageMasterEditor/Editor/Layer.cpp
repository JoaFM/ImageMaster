#include "Layer.h"


Layer::Layer(std::wstring LayerName)
{
	m_LayerName = LayerName;
	m_BlendMode = L"AlphaBlend";
}
