
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