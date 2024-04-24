#pragma once

//Every method returns time in seconds
class Clock
{
public:
	Clock() : t1(-1.0f), t2(-1.0f), last_update(GetCurTime()) {}

	static float GetCurTime()
	{
		return (float)clock();
	}

	void Begin()
	{
		t1 = GetCurTime();
	}

	void End()
	{
		t2 = GetCurTime();
	}

	void Update()
	{
		last_update = GetCurTime();
	}

	float GetElapsedSinceUpdate()
	{
		return (GetCurTime() - last_update) / CLOCKS_PER_SEC;
	}

	float GetElapsed()
	{
		return (t2 - t1) / CLOCKS_PER_SEC;
	}

private:
	float t1, t2, last_update;
};