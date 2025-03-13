#include "UTexture.h"
#include <wincodec.h>

void UTexture::init()
{

    LoadTextureFromFile(FEngineLoop::graphicDevice.Device,
        FEngineLoop::graphicDevice.DeviceContext,
        L"Assets/Texture/font.png");
    CreateSampler(FEngineLoop::graphicDevice.Device);

    if (!m_TextureSRV || !m_SamplerState) {
        Console::GetInstance().AddLog(LogLevel::Error, "TextureSRV, SamplerState Createion Error");
    }
}

HRESULT UTexture::LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename)
{
    IWICImagingFactory* wicFactory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    // WIC 팩토리 생성
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return hr;

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;


    // 이미지 파일 디코딩
    hr = wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return hr;


    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    // WIC 포맷 변환기 생성 (픽셀 포맷 변환)
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    hr = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return hr;

    // 이미지 크기 가져오기
    UINT width, height;
    frame->GetSize(&width, &height);

    // 픽셀 데이터 로드
    BYTE* imageData = new BYTE[width * height * 4];
    hr = converter->CopyPixels(nullptr, width * 4, width * height * 4, imageData);
    if (FAILED(hr)) {
        delete[] imageData;
        return hr;
    }

    // DirectX 11 텍스처 생성
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = imageData;
    initData.SysMemPitch = width * 4;

    hr = device->CreateTexture2D(&textureDesc, &initData, &m_Texture);
    delete[] imageData;
    if (FAILED(hr)) return hr;

    // Shader Resource View 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureSRV);

    // 리소스 해제
    wicFactory->Release();
    decoder->Release();
    frame->Release();
    converter->Release();

    Console::GetInstance().AddLog(LogLevel::Warning, "Texture File Load Successs");
    return hr;
}

void UTexture::CreateSampler(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&samplerDesc, &m_SamplerState);
}
