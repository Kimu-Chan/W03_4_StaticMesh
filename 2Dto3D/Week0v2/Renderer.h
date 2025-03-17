#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>
#include "EngineBaseTypes.h"
#include "Define.h"
class FGraphicsDevice;
class FRenderer 
{

private:
    float litFlag = 0;
public:
    FGraphicsDevice* Graphics;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    ID3D11Buffer* ConstantBuffer = nullptr;
    ID3D11Buffer* LightingBuffer = nullptr;
    ID3D11Buffer* LitUnlitBuffer = nullptr;

    D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

    FLighting lightingData;


    uint32 Stride;
    ID3D11Buffer* NormalConstantBuffer = nullptr;
    struct FConstants {
        FMatrix MVP;      // 모델
        float Flag;
    };
    struct FNormalConstants {
        FMatrix ModelMatrixInverseTranspose; // normal 변환을 위한 행렬
    };
    struct FLitUnlitConstants {
        int isLit; // 1 = Lit, 0 = Unlit 
        float padding[3];
    };

public:
    void Initialize(FGraphicsDevice* graphics);
   
    void Prepare();
    void PrepareShader();
    
    //Render
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices);
    void RenderPrimitive(ID3D11Buffer* pVectexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices);
    void RenderBatch(ID3D11Buffer* pVectexBuffer, UINT numVertices, UINT stride, UINT offset);
   
    
    //Release
    void Release();
    void ReleaseShader();
    void ReleaseBuffer(ID3D11Buffer*& Buffer);
    void ReleaseConstantBuffer();

    void ResetVertexShader();
    void ResetPixelShader();
    void CreateShader();

    void SetVertexShader(const FWString filename, FString funcname, FString version);
    void SetPixelShader(const FWString filename, FString funcname, FString version);
    
    void ChangeViewMode(EViewModeIndex evi);
    
    // CreateBuffer
    void CreateConstantBuffer();
    void CreateLightingBuffer();
    void CreateLitUnlitBuffer();
    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);
    ID3D11Buffer* CreateVertexBuffer(const TArray<FVertexSimple>& vertices, UINT byteWidth);
    ID3D11Buffer* CreateDynamicBatchBuffer(size_t capacity);
    ID3D11Buffer* CreateIndexBuffer(uint32* indices, UINT byteWidth);
    ID3D11Buffer* CreateIndexBuffer(const TArray<uint32>& indices, UINT byteWidth);

    // update
    void UpdateLightBuffer();
    void UpdateBuffer(ID3D11Buffer* vertexBuffer, const TArray<FVertexSimple>& Vertices);
    void UpdateConstant(FMatrix _MVP, float _Flag);
    void UpdateNormalConstantBuffer(FMatrix _Model);
    void UpdateLitUnlitConstantBuffer(int isLit);

public://텍스쳐용 기능 추가
    ID3D11VertexShader* VertexTextureShader = nullptr;
    ID3D11PixelShader* PixelTextureShader = nullptr;
    ID3D11InputLayout* TextureInputLayout = nullptr;

    uint32 TextureStride;
    struct FSubUVConstant
    {
        float indexU;
        float indexV;
    };
    ID3D11Buffer* SubUVConstantBuffer = nullptr;

public:
    void CreateTextureShader();
    void ReleaseTextureShader();
    void PrepareTextureShader();
    ID3D11Buffer* CreateVertexTextureBuffer(FVertexTexture* vertices, UINT byteWidth);
    ID3D11Buffer* CreateIndexTextureBuffer(uint32* indices, UINT byteWidth);
    void RenderTexturePrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11Buffer* pIndexBuffer, UINT numIndices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState);
    void RenderTextPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState);
    ID3D11Buffer* CreateVertexBuffer(FVertexTexture* vertices, UINT byteWidth);

    void UpdateSubUVConstant(float _indexU, float _indexV);
    void PrepareSubUVConstant();

};

