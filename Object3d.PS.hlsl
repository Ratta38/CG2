#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
};

struct DirectionalLight
{
    float32_t4 color; //!< ライトの色
    float32_t3 direction; //!< ライトの向き
    float intensity; //!< 輝度
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0) // Lightingする場合
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    else // Lightingしない場合。前回までと同じ演算
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}