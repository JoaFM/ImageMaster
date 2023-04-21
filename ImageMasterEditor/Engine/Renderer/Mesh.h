#pragma once

//c++
#include <intsafe.h>

// StD
#include <iostream>
#include <vector>
#include "Utils/IM_Math.h"

class Mesh 
{
public:
	enum class StandardShapes {
		shape_Triangle,
		shape_Square,
		Count
	};


	Mesh();
	~Mesh() { Release(); }
	struct VertData
	{
		float x, y, z;
		float u, v;
		// normal
		//color
		VertData();
		VertData(float _x, float _y, float _z, float _u, float _v);
		VertData(IM_Math::float3& Position, IM_Math::float2& UV);
	};



private:

	
	// Rendering
	struct ID3D11Buffer* m_Vertex_Buffer = nullptr;
	UINT m_Sprite_Vertex_Stride = sizeof(VertData);
	INT32 m_Triangle_VertexCount = 0;
	INT32 m_InstanceCount = 1;


	// LocationData
	IM_Math::Transfrom m_Transform;
	 
	void Release();
public:
	// mesh data
	void SetData(class Renderer* renderer, Mesh::VertData* VertData, INT32 VertCount);
	
	void SetPosition(IM_Math::float3 NewLocation);
	void SetScale(IM_Math::float3 NewLocation);
	void SetRotation(IM_Math::Quaternion NewLocation);
	void SetTransfrom(IM_Math::Transfrom NewTransform);
	IM_Math::Transfrom GetTransform() const { return m_Transform; }

	UINT& GetVerStride() { return m_Sprite_Vertex_Stride; };
	INT32 GetVertCount() { return m_Triangle_VertexCount; };

	//Shader
	void SetShader(std::wstring shaderName) { m_ShaderName = shaderName; };
	std::wstring GetShader() const { return m_ShaderName; }

	void SetInstanceCount(INT32 InstanceCount);

	//Rendering
	struct ID3D11Buffer* GetVertexBuffer() { return m_Vertex_Buffer; };
	void Render(class Renderer* renderer);

private:
	std::wstring m_ShaderName = L"Default";

};

