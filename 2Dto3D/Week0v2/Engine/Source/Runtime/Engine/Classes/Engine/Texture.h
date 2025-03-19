#pragma once
#include "Define.h"
#include "GraphicDevice.h"
struct FTexture
{
	FTexture(ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, ID3D11SamplerState* Sampler, uint32 _width, uint32 _height)
		: TextureSRV(SRV), Texture(Texture2D), SamplerState(Sampler), width(_width), height(_height)
	{}
	~FTexture()
	{
		TextureSRV->Release();
		Texture->Release();
		SamplerState->Release();
	}
	ID3D11ShaderResourceView* TextureSRV = nullptr;
	ID3D11Texture2D* Texture = nullptr;
	ID3D11SamplerState* SamplerState = nullptr;
	uint32 width;
	uint32 height;
};
