#include "UHangulText.h"

void UHangulText::SetText(FWString _text)
{
	if (_text.empty())
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text is empty");
	}

	int textSize = static_cast<int>(_text.size());


	uint32 BitmapWidth = Texture->width;
	uint32 BitmapHeight = Texture->height;

	uint32 CellWidth = BitmapWidth / ColumnCount;
	uint32 CellHeight = BitmapHeight / RowCount;

	uint32 nTexelUOffset = CellWidth / BitmapWidth;
	uint32 nTexelVOffset = CellHeight / BitmapHeight;

	for (int i = 0; i < _text.size(); i++)
	{
		FVertexTexture leftUP = { -1.0f,1.0f,0.0f,0.0f,0.0f };
		FVertexTexture rightUP = { 1.0f,1.0f,0.0f,1.0f,0.0f };
		FVertexTexture leftDown = { -1.0f,-1.0f,0.0f,0.0f,1.0f };
		FVertexTexture rightDown = { 1.0f,-1.0f,0.0f,1.0f,1.0f };
		rightUP.u *= nTexelUOffset;
		leftDown.v *= nTexelVOffset;
		rightDown.u *= nTexelUOffset;
		rightDown.v *= nTexelVOffset;

		leftUP.x += quadWidth * i;
		rightUP.x += quadWidth * i;
		leftDown.x += quadWidth * i;
		rightDown.x += quadWidth * i;

		float startU = 0.0f;
		float startV = 0.0f;

		setStartUV(_text[i], startU, startV);
		leftUP.u += (nTexelUOffset * startU);
		leftUP.v += (nTexelVOffset * startV);
		rightUP.u += (nTexelUOffset * startU);
		rightUP.v += (nTexelVOffset * startV);
		leftDown.u += (nTexelUOffset * startU);
		leftDown.v += (nTexelVOffset * startV);
		rightDown.u += (nTexelUOffset * startU);
		rightDown.v += (nTexelVOffset * startV);

		vertexTextureArr.push_back(leftUP);
		vertexTextureArr.push_back(rightUP);
		vertexTextureArr.push_back(leftDown);
		vertexTextureArr.push_back(rightUP);
		vertexTextureArr.push_back(rightDown);
		vertexTextureArr.push_back(leftDown);
	}
	UINT byteWidth = static_cast<UINT>(vertexTextureArr.size() * sizeof(FVertexTexture));

	CreateTextTextureVertexBuffer(vertexTextureArr, byteWidth);
}

void UHangulText::setStartUV(wchar_t hangul, float& outStartU, float& outStartV)
{
	//대문자만 받는중
	int StartU = 0;
	int StartV = 0;
	int offset = -1;

	offset = hangul - L'가';

	UE_LOG(LogLevel::Display, "UHangulText");
	Console::GetInstance().AddLog(LogLevel::Display, "UHangulText");

	if (offset == -1)
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text Error");
	}

	int offsetV = (offset + StartU) / ColumnCount;
	int offsetU = (offset + StartU) % ColumnCount;

	outStartU = static_cast<float>(offsetU);
	outStartV = static_cast<float>(StartV + offsetV);
}
