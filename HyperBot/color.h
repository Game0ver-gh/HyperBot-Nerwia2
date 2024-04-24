#pragma once
#pragma warning(disable : 26495)

//unsigned char RGBA color class 
class Color
{
public:
	Color()
	{
		*((int*)this) = 0;
	}
	unsigned int GetU32() const
	{
		return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8) + (_color[0] & 0xff);
	}
	Color(int color32)
	{
		*((int*)this) = color32;
	}

	Color(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 255);
	}

	Color(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	void SetColor(int _r, int _g, int _b, int _a = 255)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int& _r, int& _g, int& _b, int& _a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int*)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int*)this);
	}

	inline unsigned char r() const
	{
		return _color[0];
	}

	inline unsigned char g() const
	{
		return _color[1];
	}

	inline unsigned char b() const
	{
		return _color[2];
	}

	inline unsigned char a() const
	{
		return _color[3];
	}

	inline unsigned char& r()
	{
		return _color[0];
	}

	inline unsigned char& g()
	{
		return _color[1];
	}

	inline unsigned char& b()
	{
		return _color[2];
	}

	inline unsigned char& a()
	{
		return _color[3];
	}

	inline float rBase() const
	{
		return _color[0] / 255.0f;
	}

	inline float gBase() const
	{
		return _color[1] / 255.0f;
	}

	inline float bBase() const
	{
		return _color[2] / 255.0f;
	}

	inline float aBase() const
	{
		return _color[3] / 255.0f;
	}

	unsigned char& operator[](int index)
	{
		return _color[index];
	}

	const unsigned char& operator[](int index) const
	{
		return _color[index];
	}

	bool operator ==(const Color& rhs) const
	{
		return (*((int*)this) == *((int*)&rhs));
	}

	bool operator !=(const Color& rhs) const
	{
		return !(operator==(rhs));
	}

	Color& operator=(const Color& rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

	operator unsigned int() const
	{
		return GetU32();
	}

	Color& operator=(unsigned int value)
	{
		_color[0] = (value >> 24) & 0xFF;
		_color[1] = (value >> 16) & 0xFF;
		_color[2] = (value >> 8) & 0xFF;
		_color[3] = value & 0xFF;
		return *this;
	}

	Color operator=(unsigned int value) const
	{
		Color c;
		c[0] = (value >> 24) & 0xFF;
		c[1] = (value >> 16) & 0xFF;
		c[2] = (value >> 8) & 0xFF;
		c[3] = value & 0xFF;
		return c;
	}

	Color& operator()(unsigned char alpha)
	{
		_color[3] = alpha;
		return *this;
	}

	float* Base()
	{
		float clr[3];

		clr[0] = _color[0] / 255.0f;
		clr[1] = _color[1] / 255.0f;
		clr[2] = _color[2] / 255.0f;

		return &clr[0];
	}

	float* BaseAlpha()
	{
		float clr[4];

		clr[0] = _color[0] / 255.0f;
		clr[1] = _color[1] / 255.0f;
		clr[2] = _color[2] / 255.0f;
		clr[3] = _color[3] / 255.0f;

		return &clr[0];
	}

	float Hue() const
	{
		if (_color[0] == _color[1] && _color[1] == _color[2])
		{
			return 0.0f;
		}

		float r = _color[0] / 255.0f;
		float g = _color[1] / 255.0f;
		float b = _color[2] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
			min = r < g ? r : g < b ? g : b;
		float delta = max - min;
		float hue = 0.0f;

		if (r == max)
		{
			hue = (g - b) / delta;
		}
		else if (g == max)
		{
			hue = 2 + (b - r) / delta;
		}
		else if (b == max)
		{
			hue = 4 + (r - g) / delta;
		}
		hue *= 60;

		if (hue < 0.0f)
		{
			hue += 360.0f;
		}
		return hue;
	}

	float Saturation() const
	{
		float r = _color[0] / 255.0f;
		float g = _color[1] / 255.0f;
		float b = _color[2] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
			min = r < g ? r : g < b ? g : b;
		float l, s = 0;

		if (max != min)
		{
			l = (max + min) / 2;
			if (l <= 0.5f)
				s = (max - min) / (max + min);
			else
				s = (max - min) / (2 - max - min);
		}
		return s;
	}

	float Brightness() const
	{
		float r = _color[0] / 255.0f;
		float g = _color[1] / 255.0f;
		float b = _color[2] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
			min = r < g ? r : g < b ? g : b;
		return (max + min) / 2;
	}

	static Color FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2)
		{
			return Color(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3)
		{
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4)
		{
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return Color(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else
		{
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}

	static Color Red(unsigned char alpha = 255)
	{
		return Color(255, 0, 0, alpha);
	}

	static Color Green(unsigned char alpha = 255)
	{
		return Color(0, 255, 0, alpha);
	}

	static Color Blue(unsigned char alpha = 255)
	{
		return Color(0, 0, 255, alpha);
	}

	static Color LightBlue(unsigned char alpha = 255)
	{
		return Color(0, 53, 110, alpha);
	}

	static Color Orange(unsigned char alpha = 255)
	{
		return Color(255, 128, 0, alpha);
	}

	static Color Grey(unsigned char alpha = 255)
	{
		return Color(128, 128, 128, alpha);
	}

	static Color DarkGrey(unsigned char alpha = 255)
	{
		return Color(45, 45, 45, alpha);
	}

	static Color Black(unsigned char alpha = 255)
	{
		return Color(0, 0, 0, alpha);
	}

	static Color White(unsigned char alpha = 255)
	{
		return Color(255, 255, 255, alpha);
	}

	static Color Purple(unsigned char alpha = 255)
	{
		return Color(220, 0, 220, alpha);
	}

	static Color Pink(unsigned char alpha = 255)
	{
		return Color(255, 0, 255, alpha);
	}

	static Color Yellow(unsigned char alpha = 255)
	{
		return Color(255, 255, 0, alpha);
	}

	static Color ColdBlue(unsigned char alpha = 255)
	{
		return Color(50, 150, 215, alpha);
	}

	static Color FireRed(unsigned char alpha = 255)
	{
		return Color(215, 50, 50, alpha);
	}

private:
	unsigned char _color[4];
};