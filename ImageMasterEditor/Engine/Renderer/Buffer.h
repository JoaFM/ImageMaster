#pragma once

#include <intsafe.h>


class Buffer 
{




public:
	enum BufferType : INT32
	{
		/// <summary>
		/// Not Been set
		/// </summary>
		NONE = 0,
		
		/// <summary>
		/// For updating from the CPU all the time
		/// </summary>
		Dynamic = 1, 

		/// <summary>
		/// Read and write on GPU. Like compute shader stuff
		/// </summary>
		UNORDERED = 2
	};



	Buffer();
	~Buffer();
	/// <summary>
	/// SRV for random assess reads: probably for use in shader
	///  t# registers
	/// </summary>
	struct ID3D11ShaderResourceView* GetSRV() { return m_SRV; };


	/// <summary>
	/// UAV: You can write to this view
	/// RWStructuredBuffers,RWByteAddressBuffers  u# registers
	/// </summary>
	struct ID3D11UnorderedAccessView* GetUAV() { return m_UAV; };
	void Setup(UINT ElementSize, UINT ElementCount, BufferType TypeOfBuffer, struct ID3D11Device* Device);

	/// <summary>
	/// Change a dynamic resource
	/// </summary>
	/// <param name="NewData">Pointer to the data</param>
	/// <param name="DataLength">Size in bytes</param>
	bool UpdateBufferData(UINT8* NewData, UINT Size, struct ID3D11DeviceContext* Device_Context);

private:

	/// <summary>
	/// Create a buffer atm it is dynamic. Not the fastes but quick to update
	/// </summary>
	/// <returns>True</returns>
	bool CreateBuffer(struct ID3D11Device* m_Device);

	BufferType m_BufferType = BufferType::NONE;


	bool LoadedAndValid = false;

	INT32 m_ElementSize = -1;
	INT32 m_ElementCount = -1;


	struct ID3D11Buffer* m_Buffer = nullptr;
	struct ID3D11ShaderResourceView* m_SRV = nullptr;
	struct ID3D11UnorderedAccessView* m_UAV = nullptr;

	UINT8* m_CPUData = nullptr;

	
	void Release();
};

