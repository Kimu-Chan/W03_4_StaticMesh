#pragma once
#include "UText.h"
class UHangulText : public UText
{
public:
	void SetText(FWString _text);
	void setStartUV(wchar_t hangul, float& outStartU, float& outStartV);
};

