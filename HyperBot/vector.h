#ifndef MATHING_VECTOR_H
#define MATHING_VECTOR_H

#include <iostream>

#include "scalar.h"

namespace mathing
{

/// 4-Component vector
/** Mathematical structure used to hold 3D points and vectors.
\sa ad::Matrix,
ad::Quaternion
*/
class Vec4
{
public:
	Scalar x,y,z,w;

	/// Initialize to 0,0,0,0
	Vec4();
	/// Initialize to the values of another vector
	Vec4(const Vec4 &v);
	/// Initialize to \p x,\p y,\p z,\p w
	Vec4(Scalar x, Scalar y, Scalar z, Scalar w=0);
	/// Set to \p x,\p y,\p z,\p w
	void Set(Scalar x, Scalar y, Scalar z, Scalar w=0);

	/// Assign to the values of another vector
	Vec4 &operator=(const Vec4 &v);
	/// Add to this vector, the values of another vector.
	Vec4 &operator+=(const Vec4 &v);
	/// Subtract from this vector the values of another vector.
	Vec4 &operator-=(const Vec4 &v);
	/// Multiply and store the values of this vector by a scalar.
	Vec4 &operator*=(Scalar f);
	/// Divide and store the values of this vector by a scalar.
	Vec4 &operator/=(Scalar f);

	/// Return the values of this vector plus another
	Vec4 operator+(const Vec4 &v) const;
	/// Return the values of this vector minus another
	Vec4 operator-(const Vec4 &v) const;
	/// Return a copy of this vector, negated
	Vec4 operator-() const;
	/// Return the values of this vector times a scalar
	Vec4 operator*(const Scalar f) const;
	/// Return the values of this vector divided by a scalar
	Vec4 operator/(const Scalar f) const;
	/// Return the values of this vector plus another

	/// Return the magnitude of this vector
	Scalar Length3() const;

	/// Return the squared magnitude of this vector
	Scalar Length3Sqr() const;

	/// Return the magnitude of this vector
	Scalar Length4() const;

	/// Return the magnitude of this vector
	Scalar Length4Sqr() const;

	/// Make this a unit-vector in the same direction
	Scalar Normalize3();

	/// Make this a unit-vector in the same direction, returns length, will check distance with threshold and return m_UnitX if less than threshold
	Scalar Normalize3Safe(Scalar theshold = 1e-7);

	/// Make this a unit-vector in the same direction
	Scalar Normalize4();

	/// Cross Product: returns the vector perpendicular to both \p v1 and \p v2 (the fourth component is ignored, and returns 0)
	static Vec4 Cross(const Vec4 &v1, const Vec4 &v2);

	/// Dot Product = | \p v1 || \p v2 | cos(a), cosine of the angle between two unit-vectors
	static Scalar Dot3(const Vec4 &v1, const Vec4 &v2);

	/// Dot Product = | \p v1 || \p v2 | cos(a), cosine of the angle between two unit-vectors
	static Scalar Dot4(const Vec4 &v1, const Vec4 &v2);

	/// Linear Interpolation between two vectors
	static Vec4 Lerp(const Vec4 &from, const Vec4 &to, Scalar t);

	/// {1, 0, 0, 0}
	static const Vec4 m_UnitX;
	/// {0, 1, 0, 0}
	static const Vec4 m_UnitY;
	/// {0, 0, 1, 0}
	static const Vec4 m_UnitZ;
	/// {0, 0, 0, 1}
	static const Vec4 m_UnitW;
	/// {0, 0, 0, 0}
	static const Vec4 m_Zero;
};

std::ostream &operator<<(std::ostream &os, const Vec4 &v);
Vec4 operator*(Scalar f, const Vec4 &rhs);

class Vec3
{
public:
	Scalar x, y, z;

	/// Initialize to 0,0,0,0
	Vec3();
	/// Initialize to the values of another vector
	Vec3(const Vec3& v);
	/// Initialize to \p x,\p y,\p z,\p w
	Vec3(Scalar x, Scalar y, Scalar z);
	/// Set to \p x,\p y,\p z,\p w
	void Set(Scalar x, Scalar y, Scalar z);

	Vec3& Get();

	/// Assign to the values of another vector
	Vec3& operator=(const Vec3& v);
	/// Add to this vector, the values of another vector.
	Vec3& operator+=(const Vec3& v);
	/// Subtract from this vector the values of another vector.
	Vec3& operator-=(const Vec3& v);
	/// Multiply and store the values of this vector by a scalar.
	Vec3& operator*=(Scalar f);
	/// Divide and store the values of this vector by a scalar.
	Vec3& operator/=(Scalar f);

	/// Return the values of this vector plus another
	Vec3 operator+(const Vec3& v) const;
	/// Return the values of this vector minus another
	Vec3 operator-(const Vec3& v) const;
	/// Return a copy of this vector, negated
	Vec3 operator-() const;
	/// Return the values of this vector times a scalar
	Vec3 operator*(const Scalar f) const;
	/// Return the values of this vector divided by a scalar
	Vec3 operator/(const Scalar f) const;
	/// Return the values of this vector plus another
	Vec3 operator+(const Scalar& v) const;
	/// Return the values of this vector minus another
	Vec3 operator-(const Scalar& v) const;

	/// Return the magnitude of this vector
	Scalar Length() const;

	/// Return the squared magnitude of this vector
	Scalar LengthSqr() const;

	Scalar Distance(const Vec3& v) const;
	
	/// Make this a unit-vector in the same direction, returns length, will check distance with threshold and return m_UnitX if less than threshold
	Scalar Normalize(Scalar theshold = 1e-7);

	Vec3 Normalized(Scalar theshold = 1e-7);

	/// Cross Product: returns the vector perpendicular to both \p v1 and \p v2 (the fourth component is ignored, and returns 0)
	static Vec3 Cross(const Vec3& v1, const Vec3& v2);

	/// Dot Product = | \p v1 || \p v2 | cos(a), cosine of the angle between two unit-vectors
	static Scalar Dot(const Vec3& v1, const Vec3& v2);

	/// Linear Interpolation between two vectors
	static Vec3 Lerp(const Vec3& from, const Vec3& to, Scalar t);

	/// {1, 0, 0, 0}
	static const Vec3 m_UnitX;
	/// {0, 1, 0, 0}
	static const Vec3 m_UnitY;
	/// {0, 0, 1, 0}
	static const Vec3 m_UnitZ;
	/// {0, 0, 0, 0}
	static const Vec3 m_Zero;
};

class Vec2
{
public:
	Scalar x, y;

	/// Initialize to 0,0,0,0
	Vec2();
	/// Initialize to the values of another vector
	Vec2(const Vec2& v);
	/// Initialize to \p x,\p y,\p z,\p w
	Vec2(Scalar x, Scalar y);
	/// Set to \p x,\p y,\p z,\p w
	void Set(Scalar x, Scalar y);

	/// Assign to the values of another vector
	Vec2& operator=(const Vec2& v);
	/// Add to this vector, the values of another vector.
	Vec2& operator+=(const Vec2& v);
	/// Subtract from this vector the values of another vector.
	Vec2& operator-=(const Vec2& v);
	/// Multiply and store the values of this vector by a scalar.
	Vec2& operator*=(Scalar f);
	/// Divide and store the values of this vector by a scalar.
	Vec2& operator/=(Scalar f);

	/// Return the values of this vector plus another
	Vec2 operator+(const Vec2& v) const;
	/// Return the values of this vector minus another
	Vec2 operator-(const Vec2& v) const;
	/// Return a copy of this vector, negated
	Vec2 operator-() const;
	/// Return the values of this vector times a scalar
	Vec2 operator*(const Scalar f) const;
	/// Return the values of this vector divided by a scalar
	Vec2 operator/(const Scalar f) const;

	/// Return the magnitude of this vector
	Scalar Length() const;

	/// Return the squared magnitude of this vector
	Scalar LengthSqr() const;

	/// Make this a unit-vector in the same direction
	Scalar Normalize();

	/// Make this a unit-vector in the same direction, returns length, will check distance with threshold and return m_UnitX if less than threshold
	Scalar NormalizeSafe(Scalar theshold = 1e-7);

	/// Dot Product = | \p v1 || \p v2 | cos(a), cosine of the angle between two unit-vectors
	static Scalar Dot(const Vec2& v1, const Vec2& v2);

	/// Linear Interpolation between two vectors
	static Vec2 Lerp(const Vec2& from, const Vec2& to, Scalar t);

	/// {1, 0, 0, 0}
	static const Vec2 m_UnitX;
	/// {0, 1, 0, 0}
	static const Vec2 m_UnitY;
	/// {0, 0, 0, 0}
	static const Vec2 m_Zero;
};



}  // namespace mathing

using Vector = mathing::Vec3;
using Vector2D = mathing::Vec2;
using Vector4D = mathing::Vec4;

#endif  // MATHING_VECTOR_H
