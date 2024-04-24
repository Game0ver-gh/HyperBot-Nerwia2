#include "pch.h"
#include "vector.h"
#include <math.h>

#include <iostream>

using namespace std;

namespace mathing
{

////////////////////////
// Vec4
////////////////////////
Vec4::Vec4()
	: x(0), y(0), z(0), w(0)
{
}

Vec4::Vec4(const Vec4 &v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{
}

Vec4::Vec4(Scalar x, Scalar y, Scalar z, Scalar w)
	: x(x), y(y), z(z), w(w)
{
}

void Vec4::Set(Scalar x_arg, Scalar y_arg, Scalar z_arg, Scalar w_arg)
{
	x = x_arg;
	y = y_arg;
	z = z_arg;
	w = w_arg;
}

Vec4 &Vec4::operator=(const Vec4 &v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

Vec4 &Vec4::operator+=(const Vec4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vec4 &Vec4::operator*=(Scalar f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

Vec4 &Vec4::operator/=(Scalar f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
	return *this;
}

Vec4 Vec4::operator+(const Vec4 &v) const
{
	return Vec4(
		x + v.x,
		y + v.y,
		z + v.z,
		w + v.w);
}

Vec4 Vec4::operator-(const Vec4 &v) const
{
	return Vec4(
		x - v.x,
		y - v.y,
		z - v.z,
		w - v.w);
}

Vec4 Vec4::operator-() const
{
	return Vec4(
		-x,
		-y,
		-z,
		-w);
}


Vec4 Vec4::operator*(const Scalar f) const
{
	return Vec4(
		x * f,
		y * f,
		z * f,
		w * f);
}

Vec4 Vec4::operator/(const Scalar f) const
{
	return Vec4(
		x / f,
		y / f,
		z / f,
		w / f);
}

Scalar Vec4::Length3() const
{
	return sqrt(Length3Sqr());
}

Scalar Vec4::Length3Sqr() const
{
	return Dot3(*this, *this);
}

Scalar Vec4::Length4() const
{
	return sqrt(Length4Sqr());
}

Scalar Vec4::Length4Sqr() const
{
	return Dot4(*this, *this);
}

Scalar Vec4::Normalize3()
{
	Scalar dist = Length3();
	x /= dist;
	y /= dist;
	z /= dist;
	return dist;
}

Scalar Vec4::Normalize3Safe(Scalar threshold)
{
	Scalar dist = Length3();
	if (dist < threshold)
	{
		x = m_UnitX.x;
		y = m_UnitX.y;
		z = m_UnitX.z;
		return 0;
	}
	x /= dist;
	y /= dist;
	z /= dist;
	return dist;
}

Scalar Vec4::Normalize4()
{
	Scalar dist = Length4();
	*this /= dist;
	return dist;
}

Vec4 Vec4::Cross(const Vec4 &v1, const Vec4 &v2)
{
	Vec4 ret;
	ret.x = v1.y * v2.z - v2.y * v1.z;
	ret.y = v1.z * v2.x - v2.z * v1.x;
	ret.z = v1.x * v2.y - v2.x * v1.y;
	ret.w = 0.0;
	return ret;
}

Scalar Vec4::Dot3(const Vec4 &v1, const Vec4 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Scalar Vec4::Dot4(const Vec4 &v1, const Vec4 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

Vec4 Vec4::Lerp(const Vec4 &from, const Vec4 &to, Scalar t)
{
	Vec4 ret;
	ret = from + (to - from)*t;
	return ret;
}

const Vec4 Vec4::m_UnitX(1, 0, 0, 0);
const Vec4 Vec4::m_UnitY(0, 1, 0, 0);
const Vec4 Vec4::m_UnitZ(0, 0, 1, 0);
const Vec4 Vec4::m_UnitW(0, 0, 0, 1);
const Vec4 Vec4::m_Zero(0, 0, 0, 0);

const Vec3 Vec3::m_UnitX(1, 0, 0);
const Vec3 Vec3::m_UnitY(0, 1, 0);
const Vec3 Vec3::m_UnitZ(0, 0, 1);
const Vec3 Vec3::m_Zero(0, 0, 0);

const Vec2 Vec2::m_UnitX(1, 0);
const Vec2 Vec2::m_UnitY(0, 1);
const Vec2 Vec2::m_Zero(0, 0);

ostream &operator<<(ostream &os, const Vec4 &v)
{
	os << v.x << "," << v.y << "," << v.z << "," << v.w;
	return os;
}

Vec4 operator*(Scalar f, const Vec4 &rhs)
{
	return rhs * f;
}

Vec3::Vec3()
	: x(0), y(0), z(0)
{
}

Vec3::Vec3(const Vec3& v)
	: x(v.x), y(v.y), z(v.z)
{
}

Vec3::Vec3(Scalar x, Scalar y, Scalar z)
	: x(x), y(y), z(z)
{
}

void Vec3::Set(Scalar x_arg, Scalar y_arg, Scalar z_arg)
{
	x = x_arg;
	y = y_arg;
	z = z_arg;
}

Vec3& Vec3::Get()
{
	return *this;
}

Vec3& Vec3::operator=(const Vec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vec3& Vec3::operator+=(const Vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vec3& Vec3::operator-=(const Vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vec3& Vec3::operator*=(Scalar f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Vec3& Vec3::operator/=(Scalar f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

Vec3 Vec3::operator+(const Vec3& v) const
{
	return Vec3(
		x + v.x,
		y + v.y,
		z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) const
{
	return Vec3(
		x - v.x,
		y - v.y,
		z - v.z);
}

Vec3 Vec3::operator-() const
{
	return Vec3(
		-x,
		-y,
		-z);
}


Vec3 Vec3::operator*(const Scalar f) const
{
	return Vec3(
		x * f,
		y * f,
		z * f);
}

Vec3 Vec3::operator/(const Scalar f) const
{
	return Vec3(
		x / f,
		y / f,
		z / f);
}

Vec3 Vec3::operator+(const Scalar& v) const
{
	return Vec3(
		x + v,
		y + v,
		z + v);
}

Vec3 Vec3::operator-(const Scalar& v) const
{
	return Vec3(
		x - v,
		y - v,
		z - v);
}

Scalar Vec3::Length() const
{
	return sqrt(LengthSqr());
}

Scalar Vec3::LengthSqr() const
{
	return Dot(*this, *this);
}

Scalar Vec3::Distance(const Vec3& v) const
{
	return (*this - v).Length();
}

Scalar Vec3::Normalize(Scalar threshold)
{
	Scalar dist = Length();
	if (dist < threshold)
	{
		x = m_UnitX.x;
		y = m_UnitX.y;
		z = m_UnitX.z;
		return 0;
	}
	x /= dist;
	y /= dist;
	z /= dist;
	return dist;
}

Vec3 Vec3::Normalized(Scalar threshold)
{
	Scalar dist = Length();
	if (dist < threshold)
	{
		x = m_UnitX.x;
		y = m_UnitX.y;
		z = m_UnitX.z;
		return *this;
	}
	x /= dist;
	y /= dist;
	z /= dist;
	return *this;
}

Vec3 Vec3::Cross(const Vec3& v1, const Vec3& v2)
{
	Vec3 ret;
	ret.x = v1.y * v2.z - v2.y * v1.z;
	ret.y = v1.z * v2.x - v2.z * v1.x;
	ret.z = v1.x * v2.y - v2.x * v1.y;
	return ret;
}

Scalar Vec3::Dot(const Vec3& v1, const Vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 Vec3::Lerp(const Vec3& from, const Vec3& to, Scalar t)
{
	Vec3 ret;
	ret = from + (to - from) * t;
	return ret;
}

Vec2::Vec2()
	: x(0), y(0)
{
}

Vec2::Vec2(const Vec2& v)
	: x(v.x), y(v.y)
{
}

Vec2::Vec2(Scalar x, Scalar y)
	: x(x), y(y)
{
}

void Vec2::Set(Scalar x_arg, Scalar y_arg)
{
	x = x_arg;
	y = y_arg;
}

Vec2& Vec2::operator=(const Vec2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

Vec2& Vec2::operator+=(const Vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vec2& Vec2::operator*=(Scalar f)
{
	x *= f;
	y *= f;
	return *this;
}

Vec2& Vec2::operator/=(Scalar f)
{
	x /= f;
	y /= f;
	return *this;
}

Vec2 Vec2::operator+(const Vec2& v) const
{
	return Vec2(
		x + v.x,
		y + v.y);
}

Vec2 Vec2::operator-(const Vec2& v) const
{
	return Vec2(
		x - v.x,
		y - v.y);
}

Vec2 Vec2::operator-() const
{
	return Vec2(
		-x,
		-y);
}


Vec2 Vec2::operator*(const Scalar f) const
{
	return Vec2(
		x * f,
		y * f);
}

Vec2 Vec2::operator/(const Scalar f) const
{
	return Vec2(
		x / f,
		y / f);
}

Scalar Vec2::Length() const
{
	return sqrt(LengthSqr());
}

Scalar Vec2::LengthSqr() const
{
	return Dot(*this, *this);
}

Scalar Vec2::NormalizeSafe(Scalar threshold)
{
	Scalar dist = Length();
	if (dist < threshold)
	{
		x = m_UnitX.x;
		y = m_UnitX.y;
		return 0;
	}
	x /= dist;
	y /= dist;
	return dist;
}

Scalar Vec2::Normalize()
{
	Scalar dist = Length();
	*this /= dist;
	return dist;
}

Scalar Vec2::Dot(const Vec2& v1, const Vec2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

Vec2 Vec2::Lerp(const Vec2& from, const Vec2& to, Scalar t)
{
	Vec2 ret;
	ret = from + (to - from) * t;
	return ret;
}

}  // namespace mathing