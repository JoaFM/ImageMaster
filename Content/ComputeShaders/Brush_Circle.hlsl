#include <TAGlobals.h>


RWTexture2D<float4> BufferOut : register(u0);


float4 Blend_Over(float4 baselayer, float4 InputLayer)
{

    float a = InputLayer.a + (baselayer.a * (1.0 - InputLayer.a));
    if (a <= 0.00001)
    {
        return baselayer;
    }
    float3 color = (InputLayer.xyz * InputLayer.a) + ((baselayer.xyz * baselayer.a) * ((1.0 - InputLayer.a)));
    color /= a;

    return float4(color, a);
}

[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float2 Loc = dispatchThreadID ;
    
    float BrushMask = distance(Loc, MousePosition);
    BrushMask /= BrushSize;
    BrushMask = 1 - BrushMask;
    BrushMask = saturate(BrushMask);
    BrushMask = sin(BrushMask);
    BrushMask = saturate(BrushMask);

    BrushMask = smoothstep(0, BrushFalloff, BrushMask);
    BrushMask = saturate(BrushMask);

    BufferOut[dispatchThreadID.xy] = Blend_Over( BufferOut[dispatchThreadID.xy], float4(BrushMainColour, BrushMask * BrushAlpha));
}