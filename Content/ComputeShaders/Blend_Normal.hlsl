
RWTexture2D<float4> BufferOut : register(u0);
Texture2D CanvasTexture: register(t0);


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
    float4 InputLayer = CanvasTexture.Load(uint3(dispatchThreadID.xy, 0));
    float4 baselayer = BufferOut[dispatchThreadID.xy];
    BufferOut[dispatchThreadID.xy] = Blend_Over(baselayer, InputLayer);
}