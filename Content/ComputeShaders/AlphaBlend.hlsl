
RWTexture2D<float4> BufferOut : register(u0);
//RWStructuredBuffer<float4> BufferOut : register(u0);

[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    BufferOut[dispatchThreadID.xy] = lerp(BufferOut[dispatchThreadID.xy], float4(dispatchThreadID.xy/128.0f,.15, 1), 0.5f);
}