#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <windows.h>
#include <d3d11.h>

#include "EngineBaseTypes.h"

class FGraphicsDevice {
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* DeviceContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    ID3D11Texture2D* FrameBuffer = nullptr;
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;
    ID3D11RasterizerState* RasterizerStateSOLID = nullptr;
    ID3D11RasterizerState* RasterizerStateWIREFRAME = nullptr;

    // Depth-Stencil 관련 변수
    ID3D11Texture2D* DepthStencilBuffer = nullptr;  // 깊이/스텐실 텍스처
    ID3D11DepthStencilView* DepthStencilView = nullptr;  // 깊이/스텐실 뷰
    ID3D11DepthStencilState* DepthStencilState = nullptr;
    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)
    D3D11_VIEWPORT ViewportInfo;

    ID3D11DepthStencilState* DepthStateDisable = nullptr;

    void Initialize(HWND hWindow);
    void CreateDeviceAndSwapChain(HWND hWindow);
    void CreateDepthStencilBuffer(HWND hWindow);
    void CreateDepthStencilState();
    void CreateRasterizerState();
    void ReleaseDeviceAndSwapChain();
    void CreateFrameBuffer();
    void ReleaseFrameBuffer();
    void ReleaseRasterizerState();
    void ReleaseDepthStencilResources();
    void Release();
    void SwapBuffer();
    void Prepare();
    void OnResize(HWND hWindow);
    ID3D11RasterizerState* GetCurrentRasterizer() { return CurrentRasterizer; }
    void ChangeRasterizer(EViewModeIndex evi);
    void ChangeDepthStencilState(ID3D11DepthStencilState* newDetptStencil);
private:
    ID3D11RasterizerState* CurrentRasterizer = nullptr;
};

