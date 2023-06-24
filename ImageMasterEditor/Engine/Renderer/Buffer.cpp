#include "pch.h"
#include "Buffer.h"
#include <d3d11.h>
#include "Renderer.h"



Buffer::Buffer()

{
}

void Buffer::Setup(UINT ElementSize, UINT ElementCount, BufferType TypeOfBuffer, ID3D11Device* Device)
{
	m_ElementSize = ElementSize;
	m_ElementCount = ElementCount;
	LoadedAndValid = false;
	m_BufferType = TypeOfBuffer;
	m_CPUData = new UINT8[ElementSize * ElementCount];
	CreateBuffer(Device);
}

bool Buffer::CreateBuffer(ID3D11Device* m_Device)
{ 
	//https://stackoverflow.com/questions/50396189/d3d11-usage-staging-what-kind-of-gpu-cpu-memory-is-used/50399199
	//IMMUTABLE // “Forever”: Can not update ??? TExture geo
	//DEFAULT 	//Long Lived Buffers : around for "a while" Character geometry ---- UpdateSubresource to update 
	//DYNAMIC 	//Temporary buffers // Prefer Map/Unmap to update these  -- UpdateSubresource involves an extra copy

	//https://github.com/walbourn/directx-sdk-samples/blob/master/BasicCompute11/BasicCompute11.cpp
	{
		Release();

		D3D11_BUFFER_DESC desc = {};

		switch (m_BufferType)
		{
		case Buffer::BufferType::Dynamic:
			
			desc.ByteWidth = m_ElementSize * m_ElementCount;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = m_ElementSize;

			// Fill in the subresource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = m_CPUData;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			TA_HRCHECK(m_Device->CreateBuffer(&desc, &InitData, &m_Buffer), L"Failed to create compute buffer")
			

			break;
		case Buffer::BufferType::UNORDERED:
			
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			desc.ByteWidth = m_ElementSize * m_ElementCount;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = m_ElementSize;
			TA_HRCHECK(m_Device->CreateBuffer(&desc, nullptr, &m_Buffer), L"Failed to create compute buffer")


			break;
		default:
			TA_ERROR_WS(L"Invalid buffer type");
			return false;
			break;
		}

		//desc.StructureByteStride = m_ElementSize;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		D3D11_BUFFER_DESC descBuf = {};
		m_Buffer->GetDesc(&descBuf);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};

		if (m_BufferType == Buffer::BufferType::UNORDERED)
		{
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.FirstElement = 0;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = m_ElementCount;
		}

		TA_HRCHECK(m_Device->CreateShaderResourceView(m_Buffer, &desc, &m_SRV), L"Failed to make view for Compute shader");
	}
	//////////////////////////////////////////////////////////////////////////

	if (m_BufferType == Buffer::BufferType::UNORDERED)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC descBuf = {};

		descBuf.Format = DXGI_FORMAT_UNKNOWN;
		descBuf.ViewDimension = D3D11_UAV_DIMENSION_BUFFER; // or 1d,2d,3d
		descBuf.Buffer.FirstElement = 0;
		descBuf.Buffer.NumElements = m_ElementCount;
		descBuf.Buffer.Flags = 0; //https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_buffer_uav_flag

		TA_HRCHECK(m_Device->CreateUnorderedAccessView(m_Buffer, &descBuf, &m_UAV), L"Failed to make view UAV for buffer");
	}

	LoadedAndValid = true;
	return true;

}



bool Buffer::UpdateBufferData(UINT8* NewData, UINT Size, ID3D11DeviceContext* Device_Context)
{
	if (!m_Buffer)
	{
		TA_ERROR_WS(L"Buffer was null. HAs this thing been created");
		return false;
	}

	if (Size == m_ElementSize * m_ElementCount)
	{

		memcpy(m_CPUData, NewData, m_ElementSize * m_ElementCount);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//  Disable GPU access to the vertex buffer data.
		Device_Context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, m_CPUData, Size);
		//  Re-enable GPU access to the vertex buffer data.
		Device_Context->Unmap(m_Buffer, 0);
		return true;
	}
	else
	{
		TA_ERROR_WS(L"New data is not the same as the old buffer data");
	}

	return false;

}


Buffer::~Buffer()
{

	if (m_CPUData) {
		delete[] m_CPUData;
		m_CPUData = nullptr;
	}
	Release();
}


void Buffer::Release()
{
	TA_SAFERELEASE(m_Buffer);
	TA_SAFERELEASE(m_SRV);
	TA_SAFERELEASE(m_UAV);
}