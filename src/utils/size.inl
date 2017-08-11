
constexpr Size::Size(int w, int h) :
	width(w), height(h)
{}

inline Size& Size::operator+=(const Size& other)
{
	width += other.width;
	height += other.height;
	return *this;
}

inline Size& Size::operator-=(const Size& other)
{
	width -= other.width;
	height -= other.height;
	return *this;
}

inline Size& Size::operator*=(int factor)
{
	width  *= factor;
	height *= factor;
	return *this;
}

inline Size& Size::operator/=(int divisor)
{
	width  /= divisor;
	height /= divisor;
	return *this;
}

inline bool Size::IsFlat()const 
{
	return (width == 0 && height == 0);
}

constexpr Size operator+(const Size& lhs, const Size& rhs)
{
	return {lhs.width+rhs.width,lhs.height + rhs.height};
}

constexpr Size operator-(const Size& lhs, const Size& rhs)
{
	return{ lhs.width-rhs.width,lhs.height - rhs.height };
}

constexpr Size operator*(const Size& lhs, int factor)
{
	return{ lhs.width*factor,lhs.height *factor};
}

constexpr Size operator*(int factor, const Size& rhs)
{
	return{ rhs.width*factor,rhs.height *factor };
}

constexpr Size operator/(const Size& lhs, int divisor)
{
	return{lhs.width / divisor,lhs.height / divisor};
}

constexpr bool operator==(const Size& lhs, const Size& rhs)
{
	return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

constexpr bool operator!=(const Size& lhs, const Size& rhs)
{
	return (lhs.width != rhs.width) || (lhs.height != rhs.height);
}

constexpr bool operator<=(const Size& lhs, const Size& rhs)
{
	return (lhs.width <= rhs.width) && (lhs.height <= rhs.height);
}

constexpr bool operator>=(const Size& lhs, const Size& rhs)
{
	return (lhs.width >= rhs.width) && (lhs.height >= rhs.height);
}