#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct DirectionalLight
{
    float32_t4 color; //!< ライトの色
    float32_t3 direction; //!< ライトの向き
    float intensity; //!< 輝度
};

struct Camera
{
    float32_t3 worldPosition;
};

struct FogParam
{
    float32_t3 fogCenter;
    float radius;
    float32_t3 fogColor;
    float fogIntensity;
};

struct TimeParam
{
    float time;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<FogParam> gFogParam : register(b3);
ConstantBuffer<TimeParam> gTimeParam : register(b4);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    // textureのa値が0のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    // Cameraへの方向を算出
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    // 入射光の反射ベクトルを求める
    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    
    // 内積を取って、saturateして、shininess階乗すると鏡面反射の強度が求まる
    float RdotE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdotE), gMaterial.shininess); // 反射強度
    
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0) // Lightingする場合
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = (NdotL * 0.5f + 0.5f) * (NdotL * 0.5f + 0.5f);
        // 拡散反射
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // 鏡面反射
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        // 拡散反射+鏡面反射
        output.color.rgb = diffuse + specular;
        // アルファ
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else // Lightingしない場合。前回までと同じ演算
    {
        output.color = gMaterial.color * textureColor;
    }
    
    // output.colorのa値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    // フォグ係数の計算
    float distToFog = length(input.worldPosition - gFogParam.fogCenter);
    float fogFactor = saturate(1.0 - (distToFog / gFogParam.radius));
    fogFactor *= gFogParam.fogIntensity;
    
    // ノイズによる揺らぎ追加
    float32_t2 noiseInput = input.worldPosition.xz * 0.3 + gTimeParam.time * 0.5;
    float noise = (sin(noiseInput.x) + cos(noiseInput.y)) * 0.5 + 0.5;
    fogFactor *= noise;
    fogFactor = lerp(1.0, fogFactor, gFogParam.fogIntensity); // fogIntensity=0で霧なし、1でフル
    
    // フォグ色とのブレンド
    output.color.rgb = lerp(gFogParam.fogColor, output.color.rgb, fogFactor);
    
    return output;
}