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

    D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보



    unsigned int Stride;

    void Initialize(FGraphicsDevice* graphics);
    void Release();
    void Prepare();
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices);
    void CreateShader();
    void CreateConstantBuffer();
    void ReleaseShader();
    void PrepareShader();
    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);
    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer);
    void ReleaseConstantBuffer();
    void UpdateConstant(FMatrix _MVP, float _Flag);

    struct FConstants
    {
        FMatrix MVP;
        float   Flag;
    };
};

