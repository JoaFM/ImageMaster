#include "pch.h"
#include "IM_Math.h"
#include <cmath>
#include <string>

namespace IM_Math
{
	float4::float4()
		:
		x(0),
		y(0),
		z(0),
		w(0)
	{

	}

	float4::float4(float _x, float _y, float _z, float _w)
		:
		x(_x),
		y(_y),
		z(_z),
		w(_w)
	{
	}

	IM_Math::float3 float4::xyz()
	{
		return IM_Math::float3(x, y, z);
	}

	float3::float3()
		:
		x(0),
		y(0),
		z(0)
	{

	}

	float3::float3(float _x, float _y, float _z)
		:
		x(_x),
		y(_y),
		z(_z)
	{
	}

	IM_Math::float3 float3::operator*(const float InPut) const
	{
		return IM_Math::float3(this->x * InPut, this->y * InPut, this->z * InPut);
	}

	std::string float3::ToString()
	{
		return std::string("float3(" + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")");

	}

	IM_Math::float3& float3::operator+=(const float3& InPut)
	{
		this->x += InPut.x;
		this->y += InPut.y;
		this->z += InPut.z;
		return *this;
	}

	IM_Math::float3& float3::operator-=(const float3& InPut)
	{
		this->x -= InPut.x;
		this->y -= InPut.y;
		this->z -= InPut.z;
		return *this;
	}

	IM_Math::float3 float3::operator+(const float3 InPut)
	{
		return IM_Math::float3(x + InPut.x, y + InPut.y, z + InPut.z);
	}


	float2::float2()
		:
		x(0),
		y(0)
	{

	}

	float2::float2(float _x, float _y)
		:
		x(_x),
		y(_y)
	{

	}

	IM_Math::float2 float2::operator-(const float2& InPut)
	{
		return IM_Math::float2(x - InPut.x, y - InPut.y);
	}

	IM_Math::float2 float2::operator+(const float2& InPut)
	{
		return IM_Math::float2(x + InPut.x, y + InPut.y);
	}

	IM_Math::float2 float2::operator+=(const float2& InPut)
	{
		this->x += InPut.x;
		this->y += InPut.y;
		return *this;
	}

	IM_Math::float2 float2::operator-=(const float2& InPut)
	{
		this->x -= InPut.x;
		this->y -= InPut.y;
		return *this;
	}

	IM_Math::float2 float2::operator*(const float InPut)
	{
		return IM_Math::float2(x * InPut, y * InPut);
	}

	float float2::Distance(const IM_Math::float2& A, const IM_Math::float2& B)
	{
		return (float)sqrtf(powf((A.x - B.x), 2) + powf((A.y - B.y), 2));
	}

	std::string float2::ToString()
	{
		return std::string("float2(" + std::to_string(this->x) + "," + std::to_string(this->y) + ")");
	}

	IM_Math::float2 float2::operator/=(const float InPut)
	{
		this->x /= InPut;
		this->y /= InPut;
		return *this;
	}

	IM_Math::float2 float2::operator/(const float InPut)
	{
		return IM_Math::float2(x / InPut, y/InPut);
	}

	void float2::Normalize()
	{
		const float m = this->Magnetude();
		this->x /= m;
		this->y /= m;
	}

	const float float2::Magnetude()
	{
		return (float)sqrtf(powf((this->x), 2) + powf((this->y), 2));
	}

	Int2::Int2()
		:
		x(0),
		y(0)
	{

	}

	Int2::Int2(INT32 _x, INT32 _y)
		:
		x(_x),
		y(_y)
	{

	}

	IM_Math::Int2 Int2::operator*(const float InPut)
	{
		return IM_Math::Int2((INT32)(x * InPut), (INT32)(y * InPut));
	}

	IM_Math::Int2 Int2::operator/(const float InPut)
	{
		return IM_Math::Int2((INT32)(x / InPut), (INT32)(y / InPut));
	}

	IM_Math::Int2 Int2::operator-(const Int2 InPut)
	{
		return IM_Math::Int2(x - InPut.x, y - InPut.y);
	}

	Quaternion::Quaternion()
		:
		x(0),
		y(0),
		z(0),
		w(1)
	{
	}

	Quaternion::Quaternion(float _x, float _y, float _z, float _w) :
		x(_x),
		y(_y),
		z(_z),
		w(_w)
	{}

	IM_Math::Quaternion Quaternion::EulerToQuaternion(float yaw, float pitch, float roll)
	{
		//https://math.stackexchange.com/questions/2975109/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr
		float qx = sin(roll / 2) * cos(pitch / 2) * cos(yaw / 2) - cos(roll / 2) * sin(pitch / 2) * sin(yaw / 2);
		float qy = cos(roll / 2) * sin(pitch / 2) * cos(yaw / 2) + sin(roll / 2) * cos(pitch / 2) * sin(yaw / 2);
		float qz = cos(roll / 2) * cos(pitch / 2) * sin(yaw / 2) - sin(roll / 2) * sin(pitch / 2) * cos(yaw / 2);
		float qw = cos(roll / 2) * cos(pitch / 2) * cos(yaw / 2) + sin(roll / 2) * sin(pitch / 2) * sin(yaw / 2);
		return Quaternion((float)qx, (float)qy, (float)qz, (float)qw);
	}



	DirectX::XMMATRIX Transfrom::GetMatrix()
	{
		DirectX::XMMATRIX matRotate;
		DirectX::XMMATRIX matScale;
		DirectX::XMMATRIX matTranslate;
		DirectX::XMMATRIX LocalMatrix;

		DirectX::XMFLOAT4 InQuaternionRoation(Rotation.x, Rotation.y, Rotation.z, Rotation.w);
		DirectX::XMVECTOR QuadVecl = XMLoadFloat4(&InQuaternionRoation);
		DirectX::XMFLOAT3 m_InScale(Scale.x, Scale.y, Scale.z);
		DirectX::XMFLOAT3 Pos(Position.x, Position.y, Position.z);


		matRotate = DirectX::XMMatrixRotationQuaternion(QuadVecl);
		matScale = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_InScale));
		matTranslate = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos));
		LocalMatrix = matRotate * matScale * matTranslate;
		return LocalMatrix;
	}

	IM_Math::float3 Transfrom::Forward()
	{
		DirectX::XMMATRIX Mat = GetMatrix();
		DirectX::XMFLOAT3 f;
		XMStoreFloat3(&f, Mat.r[2]);
		return IM_Math::float3(f.x, f.y, f.z);
	}

	DirectX::XMVECTOR Transfrom::ForwardVec()
	{
		DirectX::XMMATRIX Mat = GetMatrix();
		return Mat.r[0];
	}

	IM_Math::float3 Transfrom::Right()
	{
		DirectX::XMMATRIX Mat = GetMatrix();
		DirectX::XMFLOAT3 f;
		XMStoreFloat3(&f, Mat.r[0]);
		return IM_Math::float3(f.x, f.y, f.z);

	}

	IM_Math::float3 Transfrom::Up()
	{
		DirectX::XMMATRIX Mat = GetMatrix();
		DirectX::XMFLOAT3 f;
		XMStoreFloat3(&f, Mat.r[1]);
		return IM_Math::float3(f.x, f.y, f.z);
	}

	Transfrom::Transfrom()
		:
		Position(float3(0, 0, 0)),
		Scale(float3(1, 1, 1)),
		Rotation(Quaternion(0, 0, 0, 1))
	{

	}

	Transfrom::Transfrom(float3 _Position, float3 _Scale, Quaternion _Rotation)
		:
		Position(_Position),
		Scale(_Scale),
		Rotation(_Rotation)
	{

	}

	float DegToRad(float Degree)
	{
		return Degree * 0.0174533f;
	}

	
	

}
