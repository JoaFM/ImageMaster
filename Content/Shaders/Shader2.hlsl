/*
cbuffer CB_General_Struct : register( b0 )
{
		float2 MousePos;
		float DeltaTime;
		float Time;
		float2 DisplayWindowSize;
		float2 RenderBufferSize;
		float4x4 ProjectionMatrix;
		float4x4 ViewMatrix;
		float4 BackgroundColor;
		float2 Padding;
}

cbuffer CB_PerScreenSprite_Struct : register( b1 )
{
	float4x4 ObjectToWorld;
	float4 ForegroundColor;
}
*/
#include <TAGlobals.h>


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


Texture2D    mytexture;
SamplerState mytexture_sampler;


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

	//return float4(input.PosClip.xy,0,1);
	float4 col = float4(0,0,0,1);
	

	col.xyz = mytexture.Sample(mytexture_sampler, input.uv).xyz;
	return  col ;// *ForegroundColor;
	//return  col * abs(sin(Time));// *ForegroundColor;
	//return float4(ForegroundColor ); // must return an RGBA colour
}