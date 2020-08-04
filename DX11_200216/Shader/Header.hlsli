
#define BONE_MAX_SIZE 200
#define MAX_INSTANCE 100

cbuffer VP : register(b1)
{
    matrix view;
    matrix projection;
    matrix invView;
    matrix prevView;
    matrix prevProj;
}

cbuffer W : register(b0)
{
    matrix world;
}

cbuffer Light : register(b2)
{
    float3 lightDirection;
    float specExp;

    float4 ambientLight;
}

cbuffer Material : register(b1)
{
    float4 mDiffuse;
    float4 mSpecular;
    float4 mAmbient;
    int isDiffuseMap;
    int isSpecMap;
    int isNormalMap;
    float padding;
}

struct Bone
{
    float Bonesweight;
    float HasAnimation;
    float isChanging;
    float ChangingWeight;
};

cbuffer BoneData : register(b10)
{
    Bone bones[MAX_INSTANCE];
    Bone ChangeBones[MAX_INSTANCE];
}

cbuffer ModelInstanceData : register(b11)
{
    int isActive[MAX_INSTANCE];
}

Texture2D diffuseMap : register(t0);
SamplerState diffuseSamp : register(s0);

Texture2D specularMap : register(t1);
SamplerState specularSamp : register(s1);

Texture2D normalMap : register(t2);
SamplerState normalSamp : register(s2);

Texture2D texNormalsFit : register(t13);
SamplerState PointClamp : register(s6);

struct Vertex
{
    float4 position : POSITION;
};

struct VertexUV
{
    float4 position : POSITION;
    float2 uv : UV;
};

struct VertexUVNormal
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct VertexUVNormalTangent
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VertexUVNormalTangentModel
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 BlendIndex : BLENDINDICES;
    float4 BlendWeight : BLENDWEIGHT;
};

struct PS_OUTPUT
{
    float4 RT0 : SV_Target0;
    float4 RT1 : SV_Target1;
    float4 RT2 : SV_Target2;
    float2 RT3 : SV_Target3;
};


void CompressUnsignedNormalToNormalsBuffer(in Texture2D texNormalsFit, inout float3 vNormal)
{
    float3 vNormalUns = abs(vNormal);

    float maxAbsNormalComponent = max(max(vNormalUns.x, vNormalUns.y), vNormalUns.z);

    float2 vTexCoord = abs(vNormalUns.y) < maxAbsNormalComponent ? (abs(vNormalUns.z) < maxAbsNormalComponent ? abs(vNormalUns.zy) : abs(vNormalUns.xy)) : abs(vNormalUns.xz);
    vTexCoord = vTexCoord.x < vTexCoord.y ? (vTexCoord.yx) : (vTexCoord.xy);
    vTexCoord.y /= vTexCoord.x;

    vNormal /= maxAbsNormalComponent;

    float fFittingScale = texNormalsFit.SampleLevel(PointClamp, vTexCoord, 0).r;
    vNormal *= fFittingScale;
}

float3 normal_encode_xyz(in float3 n)
{
    return n * 0.5 + 0.5;
}


