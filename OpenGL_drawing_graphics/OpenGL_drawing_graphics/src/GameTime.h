#pragma once

class GameTime
{
protected:
	static double previousTime;
	static double deltaTime;

public:
	static void Update();
	static double GetDeltaTime();
};