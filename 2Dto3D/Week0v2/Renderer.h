#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>
#include "Define.h"
class FGraphicsDevice;
class FRenderer {
public:
    FGraphicsDevice* Graphics;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    ID3D11Buffer* ConstantBuffer = nullptr;
    ID3D11Buffer* LightingBuffer = nullptr;

    D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

    FLighting lightingData{};


    uint32 Stride;

    void Initialize(FGraphicsDevice* graphics);
    void Release();
    void Prepare();
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices);
    void RenderPrimitive(ID3D11Buffer* pVectexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices);
    void RenderBatch(ID3D11Buffer* pVectexBuffer, UINT numVertices, UINT stride, UINT offset);
    void InitLightBuffer();
    void CreateShader();
    void CreateConstantBuffer();
    void CreateLightingBuffer();
    void ReleaseShader();
    void PrepareShader();
    void ResetVertexShader();
    void ResetPixelShader();
    void SetVertexShader(const FWString filename, FString funcname, FString version);
    void SetPixelShader(const FWString filename, FString funcname, FString version);
    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);
    ID3D11Buffer* CreateVertexBuffer(const TArray<FVertexSimple>& vertices, UINT byteWidth);
    ID3D11Buffer* CreateDynamicBatchBuffer(size_t capacity);
    ID3D11Buffer* CreateIndexBuffer(uint32* indices, UINT byteWidth);
    ID3D11Buffer* CreateIndexBuffer(const TArray<uint32>& indices, UINT byteWidth);

    void ReleaseBuffer(ID3D11Buffer*& Buffer);
    void ReleaseConstantBuffer();
    void UpdateLightBuffer();
    void UpdateBuffer(ID3D11Buffer* vertexBuffer, const TArray<FVertexSimple>& Vertices);
    void UpdateConstant(FMatrix _MVP, float _Flag);
   
    struct FConstants
    {
        FMatrix MVP;
        float   Flag;
    };
public://텍스쳐용 기능 추가
    ID3D11VertexShader* VertexTextureShader = nullptr;
    ID3D11PixelShader* PixelTextureShader = nullptr;
    ID3D11InputLayout* TextureInputLayout = nullptr;

    uint32 TextureStride;
    void CreateTextureShader();
    void ReleaseTextureShader();
    void PrepareTextureShader();
    ID3D11Buffer* CreateVertexTextureBuffer(FVertexTexture* vertices, UINT byteWidth);
    ID3D11Buffer* CreateIndexTextureBuffer(uint32* indices, UINT byteWidth);
    void RenderTexturePrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11Buffer* pIndexBuffer, UINT numIndices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState);
};

