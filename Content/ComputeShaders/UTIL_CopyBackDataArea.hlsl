
#include <TAGlobals.h>

int2 PixelOffset2;



RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);



[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 InputLayer = CanvasTexture.Load(uint3(dispatchThreadID.xy + PixelOffset2, 0));
    BufferOut[dispatchThreadID.xy] = InputLayer;
}