#include <TAGlobals.h>



RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);


[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float2 Loc = dispatchThreadID ;

    
    float d = distance(Loc, MousePosition);
    d /= 50.0f;
    d = 1 - d;
    d = saturate(d);
    BufferOut[dispatchThreadID.xy] = saturate(BufferOut[dispatchThreadID.xy] + (float4(d, d, d, 1) * 0.01));
}