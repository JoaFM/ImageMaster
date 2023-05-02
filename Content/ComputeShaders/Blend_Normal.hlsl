
RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);

float3 Blend_Normal(float3 Layer_Base, float3 Layer_New, float Alpha)
{
   return lerp(Layer_Base, Layer_New, Alpha);
}


[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 InputLayer = CanvasTexture.Load(uint3(dispatchThreadID.xy, 0));
    float4 baselayer = BufferOut[dispatchThreadID.xy];
    BufferOut[dispatchThreadID.xy] = float4(Blend_Normal(InputLayer.xyz, baselayer.xyz,1.0- InputLayer.a), 1);
}