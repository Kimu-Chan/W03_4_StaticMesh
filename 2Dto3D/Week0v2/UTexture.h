#pragma once
#include "Object.h"
class UTexture
{
public:
	void init();
private:
	HRESULT LoadTextureFromFile(ID3D11Device* device,
		ID3D11DeviceContext* context, const wchar_t* filename);
	void CreateSampler(ID3D11Device* device);

	ID3D11ShaderResourceView* m_TextureSRV = nullptr;
	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;

};

