#include <TAGlobals.h>
//#define Blend_Add
//#define DontDepthTest

/* vertex attributes go here to input to the vertex shader */
struct vs_in {
	float3 position_local : POSITION;
	float3 uv : UV;
	uint Instance: SV_InstanceID;

};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out {
	float4 position_clip : SV_POSITION; // required output of VS
	float2 uv  : TEXCOORD0;
	float4 PosClip : TEXCOORD1;
};

float2 MousePos_CS()
{
	float2 MP = ( MousePos / DisplayWindowSize);
	return MP;
}

Texture2D    DisplayTexture;
SamplerState DisplayTexture_sampler;

vs_out vs_main(vs_in input) {
	vs_out output = (vs_out)0; // zero the memory first
	output.uv = input.uv;

	float3 pos = input.position_local;// MousePos / DisplayWindowSize;

	pos = mul(ObjectToWorld, float4(pos.xyz, 1));
	pos = mul(ViewMatrix, float4(pos.xyz, 1));
	pos = mul(ProjectionMatrix, float4(pos.xyz, 1));

	output.position_clip = float4(pos, 1.0);
	output.PosClip = output.position_clip;
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET{

	float4 DisplayColor =  DisplayTexture.Sample(DisplayTexture_sampler, input.uv);
	DisplayColor *= DisplayColor.a;
	return  DisplayColor;
}