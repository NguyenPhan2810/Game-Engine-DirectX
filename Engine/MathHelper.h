#pragma once


class MathHelper
{
public:
	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static float toRad(const float& angle)
	{
		return angle / 180.0f * Pi;
	}

	static float toAngle(const float& rad)
	{
		return rad / Pi * 180.0f;
	}

	static const float Infinity;
	static const float Pi;
};