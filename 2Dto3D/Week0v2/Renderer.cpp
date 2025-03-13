#include "Renderer.h"
#include "GraphicDevice.h"
void FRenderer::Initialize(FGraphicsDevice* graphics) {
    Graphics = graphics;
    CreateShader();
    CreateTextureShader();
    CreateConstantBuffer();

}

void FRenderer::Release() {
    ReleaseShader();
    ReleaseTextureShader();
    if (ConstantBuffer) ConstantBuffer->Release();

}

void FRenderer::Prepare() {
    //float ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
    //Graphics->DeviceContext->ClearRenderTargetView(Graphics->FrameBufferRTV, ClearColor);
    //Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //Graphics->DeviceContext->RSSetViewports(1, &Graphics->ViewportInfo);


    //Graphics->DeviceContext->RSSetState(Graphics->RasterizerState);
    //Graphics->DeviceContext->OMSetRenderTargets(1, &Graphics->FrameBufferRTV, Graphics->DepthStencilView);
}


void FRenderer::CreateShader() {
    ID3DBlob* vertexshaderCSO;
    ID3DBlob* pixelshaderCSO;

    D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);
    Graphics->Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VertexShader);

    D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
    Graphics->Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    Graphics->Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &InputLayout);

    Stride = sizeof(FVertexSimple);
    vertexshaderCSO->Release();
    pixelshaderCSO->Release();
}
void  FRenderer::ReleaseShader()
{
    if (InputLayout)
    {
        InputLayout->Release();
        InputLayout = nullptr;
    }

    if (PixelShader)
    {
        PixelShader->Release();
        PixelShader = nullptr;
    }

    if (VertexShader)
    {
        VertexShader->Release();
        VertexShader = nullptr;
    }
}
void FRenderer::PrepareShader()
{
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);

    if (ConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
}
void FRenderer::ResetVertexShader()
{
    Graphics->DeviceContext->VSSetShader(nullptr, nullptr, 0);
    VertexShader->Release();
}
void FRenderer::ResetPixelShader()
{
    Graphics->DeviceContext->PSSetShader(nullptr, nullptr, 0);
    PixelShader->Release();
}
void FRenderer::SetVertexShader(const FWString filename, FString funcname, FString version)
{
    // 에러 발생의 가능성이 있음
    if (Graphics == nullptr)
        assert(0);
    if (VertexShader != nullptr)
        ResetVertexShader();
    if (InputLayout != nullptr)
        InputLayout->Release();
    ID3DBlob* vertexshaderCSO;

    D3DCompileFromFile(filename.c_str(), nullptr, nullptr, funcname.c_str(), version.c_str(), 0, 0, &vertexshaderCSO, nullptr);
    Graphics->Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VertexShader);
    vertexshaderCSO->Release();
}
void FRenderer::SetPixelShader(const FWString filename, FString funcname, FString version)
{
    // 에러 발생의 가능성이 있음
    if (Graphics == nullptr)
        assert(0);
    if (VertexShader != nullptr)
        ResetVertexShader();
    ID3DBlob* pixelshaderCSO;
    D3DCompileFromFile(filename.c_str(), nullptr, nullptr, funcname.c_str(), version.c_str(), 0, 0, &pixelshaderCSO, nullptr);
    Graphics->Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PixelShader);

    pixelshaderCSO->Release();
}
void FRenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) {
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderPrimitive(ID3D11Buffer* pVectexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer,  UINT numIndices)
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVectexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT,0);

    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

ID3D11Buffer* FRenderer::CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
{
    // 2. Create a vertex buffer
    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = byteWidth;
    vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

    ID3D11Buffer* vertexBuffer;

    HRESULT hr = Graphics->Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "VertexBuffer Creation faild");
    }
    return vertexBuffer;
}

ID3D11Buffer* FRenderer::CreateVertexBuffer(const TArray<FVertexSimple>& vertices, UINT byteWidth)
{
    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = byteWidth;
    vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD;
    vertexbufferSRD.pSysMem = vertices.data();

    ID3D11Buffer* vertexBuffer;

    HRESULT hr = Graphics->Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "VertexBuffer Creation faild");
    }
    return vertexBuffer;
}

ID3D11Buffer* FRenderer::CreateIndexBuffer(uint32* indices, UINT byteWidth)
{
    D3D11_BUFFER_DESC indexbufferdesc = {};						// buffer의 종류, 용도 등을 지정
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;			        // immutable: gpu가 읽기 전용으로 접근할 수 있다.
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;	        // index buffer로 사용하겠다.
    indexbufferdesc.ByteWidth = byteWidth;	// buffer 크기 지정

    D3D11_SUBRESOURCE_DATA indexbufferSRD = { indices };

    ID3D11Buffer* indexBuffer;

    HRESULT hr = Graphics->Device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "IndexBuffer Creation faild");
    }
    return indexBuffer;
}

ID3D11Buffer* FRenderer::CreateIndexBuffer(const TArray<uint32>& indices, UINT byteWidth)
{
    D3D11_BUFFER_DESC indexbufferdesc = {};						// buffer의 종류, 용도 등을 지정
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;			        // immutable: gpu가 읽기 전용으로 접근할 수 있다.
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;	        // index buffer로 사용하겠다.
    indexbufferdesc.ByteWidth = byteWidth;	// buffer 크기 지정

    D3D11_SUBRESOURCE_DATA indexbufferSRD;
    indexbufferSRD.pSysMem = indices.data();

    ID3D11Buffer* indexBuffer;

    HRESULT hr = Graphics->Device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "IndexBuffer Creation faild");
    }
    return indexBuffer;
}

void FRenderer::ReleaseBuffer(ID3D11Buffer*& Buffer)
{
    if (Buffer) {
        Buffer->Release();
        Buffer = nullptr;
    }
}

void FRenderer::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC constantbufferdesc = {};
    constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
    constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
    constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Graphics->Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
}

void FRenderer::ReleaseConstantBuffer()
{
    if (ConstantBuffer)
    {
        ConstantBuffer->Release();
        ConstantBuffer = nullptr;
    }
}
void FRenderer::UpdateConstant(FMatrix _MVP, float _Flag)
{
    if (ConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;// GPU의 메모리 주소 매핑

        Graphics->DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        FConstants* constants = (FConstants*)constantbufferMSR.pData; //GPU 메모리 직접 접근
        {
            constants->MVP = _MVP;
            constants->Flag = _Flag;
        }
        Graphics->DeviceContext->Unmap(ConstantBuffer, 0); // GPU가 다시 사용가능하게 만들기
    }
}

void FRenderer::CreateTextureShader()
{
    ID3DBlob* vertextextureshaderCSO;
    ID3DBlob* pixeltextureshaderCSO;

    D3DCompileFromFile(L"VertexTextureShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertextextureshaderCSO, nullptr);
    Graphics->Device->CreateVertexShader(vertextextureshaderCSO->GetBufferPointer(), vertextextureshaderCSO->GetBufferSize(), nullptr, &VertexTextureShader);

    D3DCompileFromFile(L"PixelTextureShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixeltextureshaderCSO, nullptr);
    Graphics->Device->CreatePixelShader(pixeltextureshaderCSO->GetBufferPointer(), pixeltextureshaderCSO->GetBufferSize(), nullptr, &PixelTextureShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    Graphics->Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertextextureshaderCSO->GetBufferPointer(), vertextextureshaderCSO->GetBufferSize(), &TextureInputLayout);

    //?
    Stride = sizeof(FVertexSimple);
    vertextextureshaderCSO->Release();
    pixeltextureshaderCSO->Release();
}

void FRenderer::ReleaseTextureShader()
{
    if (TextureInputLayout)
    {
        TextureInputLayout->Release();
        TextureInputLayout = nullptr;
    }

    if (PixelTextureShader)
    {
        PixelTextureShader->Release();
        PixelTextureShader = nullptr;
    }

    if (VertexTextureShader)
    {
        VertexTextureShader->Release();
        VertexTextureShader = nullptr;
    }
}

void FRenderer::PrepareTextureShader()
{
    Graphics->DeviceContext->VSSetShader(VertexTextureShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelTextureShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(TextureInputLayout);
    
    /*텍스쳐용 ConstantBuffer 추가필요
    if (ConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
    */
}



