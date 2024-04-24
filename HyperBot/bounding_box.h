#pragma once

namespace mathing
{
	class BoundingBox
	{
	public:
		float left, top, right, bot;

		BoundingBox() : left(0.0f), top(0.0f), right(0.0f), bot(0.0f) {};

		BoundingBox(const float left, const float top, const float right, const float bot)
		{
			this->left = left;
			this->bot = bot;
			this->right = right;
			this->top = top;
		}

		BoundingBox(const Vector2D& min, const Vector2D& max)
		{
			this->left = min.x;
			this->top = min.y;
			this->right = max.x;
			this->bot = max.y;
		}

		static const BoundingBox Zero() { return BoundingBox(0.0f, 0.0f, 0.0f, 0.0f); }
		Vector2D GetMin() const { return Vector2D(left, top); }
		Vector2D GetMax() const { return Vector2D(right, bot); }
		Vector2D GetMinBot() const { return Vector2D(left, bot); }
		Vector2D GetMaxTop() const { return Vector2D(right, top); }
		Vector2D GetSize() const { return Vector2D(fabsf(right - left), fabsf(bot - top)); }
		Vector2D GetCenter() const { return (Vector2D(fabsf(right - left) / 2.f + right, fabsf(bot - top) / 2.f + top)); }
		bool IsValid() const { return (*this != 0.0f); }

		constexpr BoundingBox& operator=(const BoundingBox& rhs)
		{
			left = rhs.left;
			top = rhs.top;
			right = rhs.right;
			bot = rhs.bot;
			return *this;
		}
		constexpr BoundingBox& operator=(const float v)
		{
			left = v;
			top = v;
			right = v;
			bot = v;
			return *this;
		}
		constexpr bool operator==(const BoundingBox& rhs) const
		{
			return (
				this->left == rhs.left
				&& this->top == rhs.top
				&& this->right == rhs.right
				&& this->bot == rhs.bot);
		}
		constexpr bool operator==(const float v) const
		{
			return (
				this->left == v
				&& this->top == v
				&& this->right == v
				&& this->bot == v);
		}
		constexpr bool operator!=(const BoundingBox& rhs) const
		{
			return !(*this == rhs);
		}
		constexpr bool operator!=(const float v) const
		{
			return !(*this == v);
		}
		const BoundingBox operator+(const BoundingBox& rhs) const
		{
			return BoundingBox(
				this->left + rhs.left,
				this->top + rhs.top,
				this->right + rhs.right,
				this->bot + rhs.bot);
		}
		const BoundingBox operator-(const BoundingBox& rhs) const
		{
			return BoundingBox(
				this->left - rhs.left,
				this->top - rhs.top,
				this->right - rhs.right,
				this->bot - rhs.bot);
		}
		const BoundingBox operator*(const BoundingBox& rhs) const
		{
			return BoundingBox(
				this->left * rhs.left,
				this->top * rhs.top,
				this->right * rhs.right,
				this->bot * rhs.bot);
		}
		const BoundingBox operator/(const BoundingBox& rhs)
		{
			return rhs != 0.f ?
				BoundingBox(
					this->left / rhs.left,
					this->top / rhs.top,
					this->right / rhs.right,
					this->bot / rhs.bot) : *this = -1.f;
		}

		const BoundingBox& operator+=(const BoundingBox& rhs)
		{
			left += rhs.left;
			top += rhs.top;
			right += rhs.right;
			bot += rhs.bot;
			return *this;
		}
		const BoundingBox& operator-=(const BoundingBox& rhs)
		{
			left -= rhs.left;
			top -= rhs.top;
			right -= rhs.right;
			bot -= rhs.bot;
			return *this;
		}
		const BoundingBox& operator*=(const BoundingBox& rhs)
		{
			left *= rhs.left;
			top *= rhs.top;
			right *= rhs.right;
			bot *= rhs.bot;
			return *this;
		}
		const BoundingBox& operator/=(const BoundingBox& rhs)
		{
			if (rhs != 0.f)
			{
				left /= rhs.left;
				top /= rhs.top;
				right /= rhs.right;
				bot /= rhs.bot;
			}
			else *this = -1.f;
			return *this;
		}

		const BoundingBox operator+(const float v) const
		{
			return BoundingBox(
				this->left + v,
				this->top + v,
				this->right + v,
				this->bot + v);
		}
		const BoundingBox operator-(const float v) const
		{
			return BoundingBox(
				this->left - v,
				this->top - v,
				this->right - v,
				this->bot - v);
		}
		const BoundingBox operator*(const float v) const
		{
			return BoundingBox(
				this->left * v,
				this->top * v,
				this->right * v,
				this->bot * v);
		}
		const BoundingBox operator/(const float v)
		{
			return v != 0.f ?
				BoundingBox(
					this->left / v,
					this->top / v,
					this->right / v,
					this->bot / v) : *this = -1.f;
		}

		const BoundingBox& operator+=(const float v)
		{
			left += v;
			top += v;
			right += v;
			bot += v;
			return *this;
		}
		const BoundingBox& operator-=(const float v)
		{
			left -= v;
			top -= v;
			right -= v;
			bot -= v;
			return *this;
		}
		const BoundingBox& operator*=(const float v)
		{
			left *= v;
			top *= v;
			right *= v;
			bot *= v;
			return *this;
		}
		const BoundingBox& operator/=(const float v)
		{
			if (v != 0.f)
			{
				left /= v;
				top /= v;
				right /= v;
				bot /= v;
			}
			else *this = -1.f;
			return *this;
		}
	};
}

using Rect = mathing::BoundingBox;