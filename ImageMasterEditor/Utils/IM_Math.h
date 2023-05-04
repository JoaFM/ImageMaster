#pragma once
#include "intsafe.h"
#include <DirectXMath.h>
#include <string>

namespace IM_Math
{
	struct float4
	{
		float x;
		float y;
		float z;
		float w;

		float4();
		float4(float _x, float _y, float _z, float _w);
	};

	struct float3
	{
		float x;
		float y;
		float z;

		float3();
		float3(float _x, float _y, float z);

		float3& operator+=(const float3& InPut);
		float3& operator-=(const float3& InPut);
		float3 operator+(const float3 InPut);
		float3 operator*(const float InPut) const;

	};

	struct float2
	{
		float x;
		float y;

		float2();
		float2(float _x, float _y);

		float2 operator-(const float2& InPut);
		float2 operator+(const float2& InPut);
		float2 operator/=(const float InPut);
		float2 operator+=(const float2& InPut);
		float2 operator*(const float InPut);


		static float Distance(const IM_Math::float2& A, const IM_Math::float2& B);
		
		std::string ToString();
		void Normalize();
		const float Magnetude();
	};

	struct Int2
	{
		INT32 x;
		INT32 y;

		Int2();
		Int2(INT32 _x, INT32 _y);

		Int2 operator-(const Int2 InPut);
	};
	struct Quaternion
	{
		float x;
		float y;
		float z;
		float w;

		Quaternion();
		Quaternion(float _x, float _y, float _z, float _w);

		static IM_Math::Quaternion EulerToQuaternion(float yaw, float pitch, float roll);

	};

	struct Transfrom
	{
		float3 Position;
		float3 Scale;
		Quaternion Rotation;

		Transfrom();
		Transfrom(float3 Position, float3 Scale, Quaternion Rotation);
		DirectX::XMMATRIX GetMatrix();


		/// <summary>
		/// X r0 Forward Vecor
		/// </summary>
		/// <returns></returns>
		float3 Forward();

		// Forwards as DX Vector
		DirectX::XMVECTOR ForwardVec();


		/// <summary>
		/// Y
		/// </summary>
		/// <returns></returns>
		float3 Right();

		/// <summary>
		/// Z
		/// </summary>
		/// <returns></returns>
		float3 Up();

	};

	float DegToRad(float Degree);


}