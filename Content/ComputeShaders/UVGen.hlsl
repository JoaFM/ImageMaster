
RWTexture2D<float4> BufferOut : register(u0);
//RWStructuredBuffer<float4> BufferOut : register(u0);

[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint sizex;
    uint sizey;
    BufferOut.GetDimensions(sizex, sizey);
    BufferOut[dispatchThreadID.xy] = float4(dispatchThreadID.x / float(sizex), dispatchThreadID.y / float(sizey),0, 1);
}