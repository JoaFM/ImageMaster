
RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);


[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    BufferOut[dispatchThreadID.xy] = float4(1,0,0,1);
}