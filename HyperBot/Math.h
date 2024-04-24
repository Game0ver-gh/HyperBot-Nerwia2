#pragma once

namespace Math
{
	void RotatePoint(Vector* point, const Vector& origin, float angle);
	float GetDistance(const Vector& src, const Vector& dst);
	bool CalcDeltaPoints(const Vector& src, const Vector& dst, float delta, std::vector<Vector>& points_out, float dist_override = -1.f);
	
}