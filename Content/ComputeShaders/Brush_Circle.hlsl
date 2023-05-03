#include <TAGlobals.h>

RWTexture2D<float4> BufferOut : register(u0);

float3 Blend_Normal(float3 Layer_Base, float3 Layer_New, float Alpha)
{
    return lerp(Layer_Base, Layer_New, Alpha);
}

float4 Blend_Normal4(float4 Layer_Base, float4 Layer_New)
{
    float baseAlpha = min(Layer_Base.a, 1 - Layer_New.a);
    float3 BaseL = Layer_Base.xyz * baseAlpha;
    float3 NewL = Layer_New.xyz * Layer_New.a;

    float  FinAlpha = baseAlpha + Layer_New.w;

    if (FinAlpha > 0)
    {
        float3 FinCol = (BaseL +  NewL) / FinAlpha;
        return float4 (FinCol, FinAlpha);
    }
    return float4(0, 0, 0, 0);
}



[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float2 Loc = dispatchThreadID ;

    
    float BrushMask = distance(Loc, MousePosition);
    BrushMask /= BrushSize;
    BrushMask = 1 - BrushMask;
    BrushMask = saturate(BrushMask);

    BufferOut[dispatchThreadID.xy] = Blend_Normal4( BufferOut[dispatchThreadID.xy], float4(BrushMainColour, BrushMask * BrushAlpha));
}