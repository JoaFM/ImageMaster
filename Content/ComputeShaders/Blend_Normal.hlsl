
RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);


[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 InputLayer = CanvasTexture.Load(uint3(dispatchThreadID.xy, 0));
  
    float4 baselayer = BufferOut[dispatchThreadID.xy];
    float maxBaseAlpha = saturate(1 - InputLayer.a);
    float BaseAlpha = min(baselayer.a, maxBaseAlpha);

    BufferOut[dispatchThreadID.xy] = float4((baselayer.xyz * BaseAlpha) + (InputLayer.xyz), BaseAlpha+ InputLayer.a);
}