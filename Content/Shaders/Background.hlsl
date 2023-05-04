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

vs_out vs_main(vs_in input) {
	vs_out output = (vs_out)0; // zero the memory first
	output.uv = input.uv.xy;

	float3 pos = input.position_local;// MousePos / DisplayWindowSize;

	pos = mul(ObjectToWorld, float4(pos.xyz, 1));
	pos = mul(ViewMatrix, float4(pos.xyz, 1));
	pos = mul(ProjectionMatrix, float4(pos.xyz, 1));

	output.position_clip = float4(pos, 1.0);
	output.PosClip = output.position_clip;
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET{

	float4 col = float4(0,0,0,1);
	float4 DisplayColor = input.PosClip ;
	col.xy = input.PosClip.xy;
	col.xy = (col.xy * DisplayWindowSize.xy) / 1200.0 ;
	col.xy -= 10000;

	col.xy = ((col.xy - 10000)  % .05) * 10;
	col.xy = abs(col.xy);

	col.xy = abs(.25 - col.xy)  ;
	col.xy = max(col.y, col.x);
	col.xy = dot(col.xy, .5);
	col.xy = smoothstep(col.xy, .00, .1);
	col.xyz = dot(col.xy,.1);
	col.xyz = lerp(float3(.01, .01, .01), float3(.04, .04, .04), col.x);

	return  col ;
}