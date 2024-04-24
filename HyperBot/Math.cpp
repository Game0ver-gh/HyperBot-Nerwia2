#include "pch.h"

void Math::RotatePoint(Vector* point, const Vector& origin, float rad_angle)
{
	float x = point->x - origin.x;
	float y = point->y - origin.y;

	float s = sinf(rad_angle);
	float c = cosf(rad_angle);

	point->x = x * c - y * s + origin.x;
	point->y = x * s + y * c + origin.y;
}

float Math::GetDistance(const Vector& src, const Vector& dst)
{
	return sqrtf(powf(dst.x - src.x, 2) + powf(dst.y - src.y, 2));
}

bool Math::CalcDeltaPoints(const Vector& src, const Vector& dst, float delta, std::vector<Vector>& points_out, float dist_override)
{
    auto dir = (dst - src);
    dir.Normalize();
    float dist = (dst - src).Length();
    int numPoints = static_cast<int>(dist / delta);

    for (int i = 1; i < numPoints; i++)
    {
        Vector point = src + dir * (i * delta);
        float pointDist = (point - dst).Length();
        if (pointDist < dist_override || pointDist >= dist)
            break;
        points_out.push_back(point);
    }

    return !points_out.empty();
}
