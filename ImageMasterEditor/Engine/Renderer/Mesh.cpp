#include "Mesh.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include <assert.h>
#include "Utils/IM_STD.h"
#include "Renderer.h"


Mesh::Mesh()
{
}


void Mesh::Release()
{
	TA_SAFERELEASE(m_Vertex_Buffer);
}

void Mesh::SetData(Renderer* renderer, Mesh::VertData* VertData, INT32 VertCount)
{
	Release();
	D3D11_BUFFER_DESC vertex_buff_descr = {};
	vertex_buff_descr.ByteWidth = sizeof(Mesh::VertData) * VertCount ;
	vertex_buff_descr.Usage = D3D11_USAGE_DEFAULT;
	vertex_buff_descr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sr_data = { 0 };
	sr_data.pSysMem = VertData;


	HRESULT hr = renderer->GetDevice()->CreateBuffer(
		&vertex_buff_descr,
		&sr_data,
		&m_Vertex_Buffer);
	assert(SUCCEEDED(hr));
	m_Triangle_VertexCount = VertCount ;


}



void Mesh::SetPosition(IM_Math::float3 NewPosition)
{
	m_Transform.Position = NewPosition;
}

void Mesh::SetScale(IM_Math::float3 NewScale)
{
	m_Transform.Scale = NewScale;
}

void Mesh::SetRotation(IM_Math::Quaternion NewRotation)
{
	m_Transform.Rotation = NewRotation;
}

void Mesh::SetTransfrom(IM_Math::Transfrom NewTransform)
{
	m_Transform = NewTransform;
}

void Mesh::Render(class Renderer* renderer)
{
	ID3D11DeviceContext* Device_Context = renderer->GetDeviceContext();

	UINT Offset = 0;
	ID3D11Buffer* VB = GetVertexBuffer();
	Device_Context->IASetVertexBuffers(0, 1, &VB, &GetVerStride(), &Offset);

	renderer->BindShader(m_ShaderName);
	
	Device_Context->DrawInstanced(GetVertCount(), m_InstanceCount, 0, 0);

	renderer->UnbindCurrentShader();

}

void Mesh::SetInstanceCount(INT32 InstanceCount)
{
	if (InstanceCount > 0)
	{
		m_InstanceCount = InstanceCount;
	}
	else
	{
		TA_ERROR_WS(L"Not a valid instance count. Need to more than 0");
	}
}

Mesh::VertData::VertData(float _x, float _y, float _z, float _u, float _v)
	:
	x(_x),
	y(_y),
	z(_z),
	u(_u),
	v(_v)
{
}

Mesh::VertData::VertData() :
	x(0),
	y(0),
	z(0),
	u(0),
	v(0)
{
}

Mesh::VertData::VertData(IM_Math::float3& Position, IM_Math::float2& UV)
	:
	x(Position.x),
	y(Position.y),
	z(Position.z),
	u(UV.x),
	v(UV.y)
{

}
