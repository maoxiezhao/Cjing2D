
constexpr Point2::Point2()
{
}

constexpr Point2::Point2(const Vec2i& vec):
	x(vec.x),y(vec.y)
{
}

constexpr Point2::Point2(int x, int y):
		x(x),y(y)
{
}

inline const Point2 Point2::operator-() const
{
	return Point2(-x, -y);
}

inline Point2 & Point2::operator+=(const Point2 & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

inline Point2 & Point2::operator-=(const Point2 & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

inline Point2 & Point2::operator*=(int factor)
{
	x *= factor;
	y *= factor;
	return *this;
}

inline Point2 & Point2::operator/=(int divisor)
{
	x /= divisor;
	y /= divisor;
	return *this;
}

constexpr bool operator==(const Point2&lhs, const Point2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr bool operator!=(const Point2&lhs, const Point2& rhs)
{
	return lhs.x != rhs.x || lhs.x != rhs.x;
}

constexpr Point2 operator+(const Point2&lhs, const Point2& rhs)
{
	return Point2(lhs.x + rhs.x, lhs.y + rhs.y);
}

constexpr Point2 operator-(const Point2&lhs, const Point2& rhs)
{
	return Point2(lhs.x - rhs.x, lhs.y - rhs.y);
}

constexpr Point2 operator*(const Point2&lhs, int factor)
{
	return Point2(lhs.x * factor, lhs.y *factor);
}

constexpr Point2 operator/(const Point2&lhs, int divisor)
{
	return Point2(lhs.x / divisor, lhs.y / divisor);
}