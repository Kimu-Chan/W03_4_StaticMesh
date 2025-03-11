#pragma once

class UWorld;
class ControlPanel
{
private:
	ControlPanel();

public:
	~ControlPanel();
	static ControlPanel GetInstance()
	{
		static ControlPanel Inst;
		return Inst;
	}
	void Draw(UWorld* world, double elapsedTime);
};

