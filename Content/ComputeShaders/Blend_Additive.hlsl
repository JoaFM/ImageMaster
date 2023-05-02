
RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);


[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 InputLayer = CanvasTexture.Load(uint3(dispatchThreadID.xy, 0));
    float4 baselayer = BufferOut[dispatchThreadID.xy];

    BufferOut[dispatchThreadID.xy] = saturate(baselayer+ InputLayer);
}